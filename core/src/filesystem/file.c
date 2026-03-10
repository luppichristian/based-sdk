// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/file.h"

#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "basic/env_defines.h"

#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <stdio.h>

#if defined(PLATFORM_WINDOWS)
#  include <windows.h>
#endif

func b32 file_paths_equal(const path* lhs, const path* rhs) {
  profile_func_begin;
  if (lhs == NULL || rhs == NULL) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return cstr8_cmp(lhs->buf, rhs->buf);
}

func b32 file_replace_path(const path* src, const path* dst) {
  profile_func_begin;
#if defined(PLATFORM_WINDOWS)
  profile_func_end;
  return MoveFileExA(src->buf, dst->buf, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) ? true : false;
#elif defined(PLATFORM_UNIX)
  profile_func_end;
  return rename(src->buf, dst->buf) == 0 ? true : false;
#else
  if (path_exists(dst) && !path_remove(dst)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return SDL_RenamePath(src->buf, dst->buf) ? true : false;
#endif
}

func b32 file_make_temp_path(const path* src, path* out_path) {
  profile_func_begin;
  path tmp_path;
  u32 attempt_idx = 0;

  if (src == NULL || out_path == NULL) {
    profile_func_end;
    return false;
  }

  for (attempt_idx = 0; attempt_idx < 32; attempt_idx += 1) {
    tmp_path = *src;
    if (!cstr8_append_format(tmp_path.buf, size_of(tmp_path.buf), ".tmp.%u", attempt_idx)) {
      profile_func_end;
      return false;
    }

    if (!path_exists(&tmp_path)) {
      *out_path = tmp_path;
      profile_func_end;
      return true;
    }
  }

  profile_func_end;
  return false;
}

func cstr8 file_path_cstr(const path* src) {
  profile_func_begin;
  if (src == NULL) {
    profile_func_end;
    return "";
  }
  profile_func_end;
  return src->buf;
}

func b32 file_create(const path* src) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0') {
    thread_log_error("Rejected file create for invalid path");
    profile_func_end;
    return false;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  if (file_ptr == NULL) {
    thread_log_error("Failed to create file path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }

  SDL_CloseIO(file_ptr);
  thread_log_trace("Created file path=%s", src->buf);
  profile_func_end;
  return true;
}

func b32 file_delete(const path* src) {
  profile_func_begin;
  if (!file_exists(src)) {
    thread_log_warn("Cannot delete missing file path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }
  assert(src != NULL);

  b32 success = SDL_RemovePath(file_path_cstr(src)) ? true : false;
  if (!success) {
    thread_log_error("Failed to delete file path=%s error=%s", src->buf, SDL_GetError());
  } else {
    thread_log_trace("Deleted file path=%s", src->buf);
  }
  profile_func_end;
  return success;
}

func b32 file_rename(const path* old_src, const path* new_src) {
  profile_func_begin;
  if (!file_exists(old_src)) {
    thread_log_warn("Cannot rename missing file path=%s", old_src != NULL ? old_src->buf : "<null>");
    profile_func_end;
    return false;
  }
  if (new_src == NULL || new_src->buf[0] == '\0') {
    thread_log_error("Rejected file rename for invalid destination source=%s",
                     old_src != NULL ? old_src->buf : "<null>");
    profile_func_end;
    return false;
  }
  assert(new_src->buf[0] != '\0');

  b32 success = SDL_RenamePath(file_path_cstr(old_src), file_path_cstr(new_src)) ? true : false;
  if (!success) {
    thread_log_error("Failed to rename file from=%s to=%s error=%s", old_src->buf, new_src->buf, SDL_GetError());
  } else {
    thread_log_trace("Renamed file from=%s to=%s", old_src->buf, new_src->buf);
  }
  profile_func_end;
  return success;
}

func b32 file_copy(const path* src, const path* dst, b32 overwrite_existing) {
  profile_func_begin;
  SDL_IOStream* src_file = NULL;
  SDL_IOStream* dst_file = NULL;
  u8 copy_buf[16 * 1024];
  sz read_size = 0;

  if (!file_exists(src) || dst == NULL) {
    thread_log_error("Rejected file copy source=%s destination=%s",
                     src != NULL ? src->buf : "<null>",
                     dst != NULL ? dst->buf : "<null>");
    profile_func_end;
    return false;
  }
  assert(src != NULL);
  assert(dst != NULL);

  if (file_paths_equal(src, dst)) {
    profile_func_end;
    return true;
  }

  if (path_exists(dst)) {
    if (!overwrite_existing || !file_exists(dst)) {
      thread_log_warn("Refused to overwrite destination file path=%s", dst->buf);
      profile_func_end;
      return false;
    }
  }

  src_file = SDL_IOFromFile(file_path_cstr(src), "rb");
  if (src_file == NULL) {
    thread_log_error("Failed to open source file for copy path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }

  dst_file = SDL_IOFromFile(file_path_cstr(dst), "wb");
  if (dst_file == NULL) {
    thread_log_error("Failed to open destination file for copy path=%s error=%s", dst->buf, SDL_GetError());
    SDL_CloseIO(src_file);
    profile_func_end;
    return false;
  }

  for (;;) {
    read_size = (sz)SDL_ReadIO(src_file, copy_buf, size_of(copy_buf));
    if (read_size == 0) {
      break;
    }

    if ((sz)SDL_WriteIO(dst_file, copy_buf, (size_t)read_size) != read_size) {
      thread_log_error("Failed to write copied file data destination=%s expected=%zu error=%s",
                       dst->buf,
                       (size_t)read_size,
                       SDL_GetError());
      SDL_CloseIO(dst_file);
      SDL_CloseIO(src_file);
      profile_func_end;
      return false;
    }
  }

  if (!SDL_CloseIO(dst_file)) {
    thread_log_error("Failed to finalize copied file destination=%s error=%s", dst->buf, SDL_GetError());
    SDL_CloseIO(src_file);
    profile_func_end;
    return false;
  }

  SDL_CloseIO(src_file);
  thread_log_info("Copied file from=%s to=%s", src->buf, dst->buf);
  profile_func_end;
  return true;
}

func b32 file_exists(const path* src) {
  profile_func_begin;
  SDL_PathInfo path_info;
  if (src == NULL || src->buf[0] == '\0') {
    profile_func_end;
    return false;
  }

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info)) {
    profile_func_end;
    return false;
  }

  profile_func_end;
  return path_info.type == SDL_PATHTYPE_FILE ? true : false;
}

func b32 file_get_size(const path* src, sz* out_size) {
  profile_func_begin;
  SDL_PathInfo path_info;

  if (out_size == NULL || !file_exists(src)) {
    profile_func_end;
    return false;
  }
  assert(out_size != NULL);

  if (!SDL_GetPathInfo(file_path_cstr(src), &path_info) || path_info.type != SDL_PATHTYPE_FILE) {
    profile_func_end;
    return false;
  }

  *out_size = (sz)path_info.size;
  profile_func_end;
  return true;
}

func b32 file_read_all(const path* src, allocator* alloc, buffer* out_data) {
  profile_func_begin;
  SDL_IOStream* file_ptr = NULL;
  void* data_ptr = NULL;
  sz file_size = 0;
  sz read_size = 0;

  if (alloc == NULL || out_data == NULL) {
    thread_log_error("Rejected file read with invalid output state path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }
  if (alloc->alloc_fn == NULL || alloc->dealloc_fn == NULL) {
    thread_log_error("Rejected file read without allocator path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }
  assert(alloc->alloc_fn != NULL);
  assert(alloc->dealloc_fn != NULL);

  out_data->ptr = NULL;
  out_data->size = 0;

  if (!file_get_size(src, &file_size)) {
    thread_log_error("Failed to query file size before read path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return false;
  }

  file_ptr = SDL_IOFromFile(file_path_cstr(src), "rb");
  if (file_ptr == NULL) {
    thread_log_error("Failed to open file for read path=%s error=%s", src != NULL ? src->buf : "<null>", SDL_GetError());
    profile_func_end;
    return false;
  }

  if (file_size > 0) {
    data_ptr = allocator_alloc(*alloc, file_size);
    if (data_ptr == NULL) {
      thread_log_error("Failed to allocate read buffer path=%s size=%zu",
                       src != NULL ? src->buf : "<null>",
                       (size_t)file_size);
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return false;
    }

    read_size = (sz)SDL_ReadIO(file_ptr, data_ptr, (size_t)file_size);
    if (read_size != file_size) {
      thread_log_error("Failed to read full file path=%s expected=%zu actual=%zu error=%s",
                       src != NULL ? src->buf : "<null>",
                       (size_t)file_size,
                       (size_t)read_size,
                       SDL_GetError());
      allocator_dealloc(*alloc, data_ptr, file_size);
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return false;
    }
  }

  SDL_CloseIO(file_ptr);
  out_data->ptr = data_ptr;
  out_data->size = file_size;
  thread_log_trace("Read file path=%s size=%zu", src != NULL ? src->buf : "<null>", (size_t)file_size);
  profile_func_end;
  return true;
}

func b32 file_write_all(const path* src, buffer data) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0' || (data.size > 0 && data.ptr == NULL)) {
    thread_log_error("Rejected file write path=%s size=%zu", src != NULL ? src->buf : "<null>", (size_t)data.size);
    profile_func_end;
    return false;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "wb");
  sz write_size = 0;

  if (file_ptr == NULL) {
    thread_log_error("Failed to open file for write path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      thread_log_error("Failed to write full file path=%s expected=%zu actual=%zu error=%s",
                       src->buf,
                       (size_t)data.size,
                       (size_t)write_size,
                       SDL_GetError());
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return false;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    thread_log_error("Failed to finalize file write path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }
  thread_log_trace("Wrote file path=%s size=%zu", src->buf, (size_t)data.size);
  profile_func_end;
  return true;
}

func b32 file_append_all(const path* src, buffer data) {
  profile_func_begin;
  if (src == NULL || src->buf[0] == '\0' || (data.size > 0 && data.ptr == NULL)) {
    thread_log_error("Rejected file append path=%s size=%zu", src != NULL ? src->buf : "<null>", (size_t)data.size);
    profile_func_end;
    return false;
  }
  assert(src->buf[0] != '\0');
  SDL_IOStream* file_ptr = SDL_IOFromFile(file_path_cstr(src), "ab");
  sz write_size = 0;

  if (file_ptr == NULL) {
    thread_log_error("Failed to open file for append path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }

  if (data.size > 0) {
    write_size = (sz)SDL_WriteIO(file_ptr, data.ptr, (size_t)data.size);
    if (write_size != data.size) {
      thread_log_error("Failed to append full file path=%s expected=%zu actual=%zu error=%s",
                       src->buf,
                       (size_t)data.size,
                       (size_t)write_size,
                       SDL_GetError());
      SDL_CloseIO(file_ptr);
      profile_func_end;
      return false;
    }
  }

  if (!SDL_CloseIO(file_ptr)) {
    thread_log_error("Failed to finalize file append path=%s error=%s", src->buf, SDL_GetError());
    profile_func_end;
    return false;
  }
  thread_log_trace("Appended file path=%s size=%zu", src->buf, (size_t)data.size);
  profile_func_end;
  return true;
}

func b32 file_write_all_atomic(const path* src, buffer data) {
  profile_func_begin;
  path tmp_path;

  if (src == NULL) {
    thread_log_error("Rejected atomic file write for NULL path");
    profile_func_end;
    return false;
  }

  if (!file_make_temp_path(src, &tmp_path)) {
    thread_log_error("Failed to create temporary file path for atomic write path=%s", src->buf);
    profile_func_end;
    return false;
  }

  if (!file_write_all(&tmp_path, data)) {
    thread_log_error("Failed to write temporary file for atomic write path=%s temp=%s", src->buf, tmp_path.buf);
    (void)path_remove(&tmp_path);
    profile_func_end;
    return false;
  }

  if (!file_replace_path(&tmp_path, src)) {
    thread_log_error("Failed to replace file during atomic write path=%s temp=%s", src->buf, tmp_path.buf);
    (void)path_remove(&tmp_path);
    profile_func_end;
    return false;
  }

  thread_log_info("Completed atomic file write path=%s size=%zu", src->buf, (size_t)data.size);
  profile_func_end;
  return true;
}
