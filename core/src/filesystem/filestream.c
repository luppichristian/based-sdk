// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/filestream.h"

#include "basic/assert.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "filesystem/archive.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "memory/allocator.h"
#include "memory/buffer.h"

#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <string.h>

func filestream_error filestream_set_error(filestream* stm, filestream_error error_code) {
  profile_func_begin;
  if (stm != NULL) {
    stm->error_code = error_code;
  }

  profile_func_end;
  return error_code;
}

func allocator filestream_allocator_resolve(void) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    profile_func_end;
    return alloc;
  }
  profile_func_end;
  return global_get_allocator();
}

func filestream filestream_empty(void) {
  filestream stm;
  memset(&stm, 0, size_of(stm));
  stm.archive_path = path_from_cstr("");
  return stm;
}

func cstr8 filestream_mode_string(u32 mode_flags) {
  profile_func_begin;
  if ((mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    profile_func_end;
    return (mode_flags & FILESTREAM_OPEN_READ) != 0 ? "a+b" : "ab";
  }

  if ((mode_flags & FILESTREAM_OPEN_WRITE) != 0) {
    if ((mode_flags & FILESTREAM_OPEN_READ) != 0) {
      if ((mode_flags & FILESTREAM_OPEN_TRUNCATE) != 0) {
        profile_func_end;
        return "w+b";
      }
      profile_func_end;
      return "r+b";
    }

    profile_func_end;
    return "wb";
  }

  profile_func_end;
  return "rb";
}

func sz filestream_native_size(SDL_IOStream* file_ptr) {
  profile_func_begin;
  Sint64 size_value = 0;

  if (file_ptr == NULL) {
    profile_func_end;
    return 0;
  }

  size_value = SDL_GetIOSize(file_ptr);
  if (size_value < 0) {
    profile_func_end;
    return 0;
  }

  profile_func_end;
  return (sz)size_value;
}

func path filestream_normalize_archive_path(const path* src) {
  profile_func_begin;
  path item_path = path_from_cstr(src != NULL ? src->buf : "");
  path_normalize(&item_path);
  path_remove_trailing_slash(&item_path);
  profile_func_end;
  return item_path;
}

func archive_entry* filestream_find_archive_entry(archive* arc, const path* src) {
  profile_func_begin;
  path item_path = filestream_normalize_archive_path(src);
  sz item_idx = 0;

  if (arc == NULL) {
    profile_func_end;
    return NULL;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    path ent_path = filestream_normalize_archive_path(&arc->entries[item_idx].item_path);
    if (cstr8_cmp(ent_path.buf, item_path.buf)) {
      profile_func_end;
      return &arc->entries[item_idx];
    }
  }

  profile_func_end;
  return NULL;
}

func b32 filestream_reserve_memory(filestream* stm, sz min_capacity) {
  profile_func_begin;
  allocator alloc = {0};
  u8* new_ptr = NULL;
  sz new_capacity = 0;

  if (stm == NULL) {
    profile_func_end;
    return false;
  }
  if (stm->memory_capacity >= min_capacity) {
    profile_func_end;
    return true;
  }
  assert(stm->memory_size <= stm->memory_capacity);
  alloc = filestream_allocator_resolve();

  new_capacity = stm->memory_capacity == 0 ? 64 : stm->memory_capacity;
  while (new_capacity < min_capacity) {
    if (new_capacity > SZ_MAX / 2) {
      new_capacity = min_capacity;
      break;
    }
    new_capacity *= 2;
  }

  new_ptr = (u8*)allocator_realloc(alloc, stm->memory_ptr, stm->memory_capacity, new_capacity);
  if (new_ptr == NULL) {
    thread_log_error("Failed to grow filestream buffer current=%zu requested=%zu",
                     (size_t)stm->memory_capacity,
                     (size_t)new_capacity);
    filestream_set_error(stm, FILESTREAM_ERROR_ALLOC);
    profile_func_end;
    return false;
  }

  stm->memory_ptr = new_ptr;
  stm->memory_capacity = new_capacity;
  profile_func_end;
  return true;
}

func filestream filestream_open(const path* src, u32 mode_flags) {
  profile_func_begin;
  filestream stm = filestream_empty();
  SDL_IOStream* file_ptr = NULL;
  cstr8 mode_str = filestream_mode_string(mode_flags);

  file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", mode_str);
  if (file_ptr == NULL && (mode_flags & FILESTREAM_OPEN_CREATE) != 0 &&
      (mode_flags & FILESTREAM_OPEN_APPEND) == 0 &&
      (mode_flags & FILESTREAM_OPEN_READ) != 0 &&
      (mode_flags & FILESTREAM_OPEN_WRITE) != 0) {
    file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", "w+b");
  }

  if (file_ptr == NULL) {
    thread_log_error("Failed to open filestream path=%s mode=%s error=%s",
                     src != NULL ? src->buf : "<null>",
                     mode_str,
                     SDL_GetError());
    profile_func_end;
    return stm;
  }
  if (src == NULL) {
    thread_log_error("Rejected filestream open for NULL path after SDL open");
    SDL_CloseIO(file_ptr);
    profile_func_end;
    return filestream_empty();
  }
  assert(file_ptr != NULL);

  stm.kind = FILESTREAM_KIND_NATIVE;
  stm.mode_flags = mode_flags;
  stm.native_handle = file_ptr;
  stm.error_code = FILESTREAM_ERROR_NONE;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILESTREAM,
                                                     .object_ptr = &stm,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("Opened native filestream path=%s flags=0x%08x", src->buf, mode_flags);
  profile_func_end;
  return stm;
}

func filestream filestream_open_archive(archive* arc, const path* src, u32 mode_flags) {
  profile_func_begin;
  filestream stm = filestream_empty();
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL) {
    thread_log_error("Rejected archive filestream open path=%s", src != NULL ? src->buf : "<null>");
    profile_func_end;
    return stm;
  }
  assert(src->buf[0] != '\0');

  ent = filestream_find_archive_entry(arc, src);
  if (ent == NULL && (mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    thread_log_warn("Missing archive entry for filestream path=%s", src->buf);
    profile_func_end;
    return stm;
  }

  stm.kind = FILESTREAM_KIND_ARCHIVE;
  stm.mode_flags = mode_flags;
  stm.archive_ref = arc;
  stm.archive_path = filestream_normalize_archive_path(src);

  if (ent != NULL && !ent->is_directory && ent->data_size > 0) {
    if (!filestream_reserve_memory(&stm, ent->data_size)) {
      thread_log_error("Failed to allocate archive filestream buffer path=%s size=%zu",
                       src->buf,
                       (size_t)ent->data_size);
      filestream_close(&stm);
      profile_func_end;
      return filestream_empty();
    }

    memcpy(stm.memory_ptr, ent->data_ptr, ent->data_size);
    stm.memory_size = ent->data_size;
  }

  if ((mode_flags & FILESTREAM_OPEN_TRUNCATE) != 0) {
    stm.memory_size = 0;
  }

  if ((mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    stm.cursor = stm.memory_size;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILESTREAM,
                                                     .object_ptr = &stm,
                                                 });
  (void)msg_post(&lifecycle_msg);
  thread_log_trace("Opened archive filestream path=%s flags=0x%08x", stm.archive_path.buf, mode_flags);

  profile_func_end;
  return stm;
}

func b32 filestream_flush(filestream* stm) {
  profile_func_begin;
  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    thread_log_error("Cannot flush closed filestream");
    profile_func_end;
    return false;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    if (!SDL_FlushIO((SDL_IOStream*)stm->native_handle)) {
      thread_log_error("Failed to flush native filestream handle=%p error=%s",
                       stm->native_handle,
                       SDL_GetError());
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      profile_func_end;
      return false;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    thread_log_trace("Flushed native filestream handle=%p", stm->native_handle);
    profile_func_end;
    return true;
  }

  if (stm->dirty && stm->archive_ref != NULL) {
    thread_log_verbose("Flushing archive filestream path=%s size=%zu",
                       stm->archive_path.buf,
                       (size_t)stm->memory_size);
    buffer data = buffer_from(stm->memory_ptr, stm->memory_size);
    if (!archive_write_all(stm->archive_ref, &stm->archive_path, data)) {
      thread_log_error("Failed to flush archive filestream path=%s", stm->archive_path.buf);
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      profile_func_end;
      return false;
    }

    stm->dirty = 0;
  }

  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  profile_func_end;
  return true;
}

func void filestream_close(filestream* stm) {
  profile_func_begin;
  allocator alloc = filestream_allocator_resolve();
  SDL_IOStream* file_ptr = NULL;

  if (stm == NULL) {
    profile_func_end;
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILESTREAM,
                                                     .object_ptr = stm,
                                                 });
  (void)msg_post(&lifecycle_msg);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    file_ptr = (SDL_IOStream*)stm->native_handle;
    if (file_ptr != NULL) {
      (void)filestream_flush(stm);
      SDL_CloseIO(file_ptr);
    }
  } else if (stm->kind == FILESTREAM_KIND_ARCHIVE) {
    (void)filestream_flush(stm);
    if (stm->memory_ptr != NULL) {
      allocator_dealloc(alloc, stm->memory_ptr, stm->memory_capacity);
    }
  }

  thread_log_trace("Closed filestream kind=%u", (u32)stm->kind);
  *stm = filestream_empty();
  profile_func_end;
}

func b32 filestream_is_open(const filestream* stm) {
  profile_func_begin;
  if (stm == NULL) {
    profile_func_end;
    return false;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    profile_func_end;
    return stm->native_handle != NULL ? true : false;
  }

  if (stm->kind == FILESTREAM_KIND_ARCHIVE) {
    profile_func_end;
    return stm->archive_ref != NULL ? true : false;
  }

  profile_func_end;
  return false;
}

func sz filestream_read(filestream* stm, void* dst, sz size) {
  profile_func_begin;
  sz available = 0;
  sz read_size = 0;

  if (!filestream_is_open(stm) || dst == NULL || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    thread_log_error("Rejected filestream read handle=%p size=%zu", (void*)stm, (size_t)size);
    profile_func_end;
    return 0;
  }
  assert(stm->kind == FILESTREAM_KIND_NATIVE || stm->kind == FILESTREAM_KIND_ARCHIVE);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    read_size = (sz)SDL_ReadIO((SDL_IOStream*)stm->native_handle, dst, (size_t)size);
    if (read_size == 0 && size > 0) {
      if (filestream_eof(stm)) {
        filestream_set_error(stm, FILESTREAM_ERROR_EOF);
        thread_log_debug("Reached native filestream EOF handle=%p", stm->native_handle);
      } else {
        filestream_set_error(stm, FILESTREAM_ERROR_IO);
        thread_log_error("Failed to read native filestream handle=%p error=%s",
                         stm->native_handle,
                         SDL_GetError());
      }
    } else {
      filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    }
    profile_func_end;
    return read_size;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_READ) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    thread_log_warn("Rejected archive filestream read without read access path=%s", stm->archive_path.buf);
    profile_func_end;
    return 0;
  }

  if (stm->cursor >= stm->memory_size) {
    filestream_set_error(stm, FILESTREAM_ERROR_EOF);
    thread_log_debug("Reached archive filestream EOF path=%s", stm->archive_path.buf);
    profile_func_end;
    return 0;
  }

  available = stm->memory_size - stm->cursor;
  read_size = size < available ? size : available;
  memcpy(dst, stm->memory_ptr + stm->cursor, read_size);
  stm->cursor += read_size;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  profile_func_end;
  return read_size;
}

func b32 filestream_read_exact(filestream* stm, void* dst, sz size) {
  profile_func_begin;
  sz total_read = 0;
  u8* dst_ptr = (u8*)dst;

  while (total_read < size) {
    sz step_size = filestream_read(stm, dst_ptr + total_read, size - total_read);
    if (step_size == 0) {
      profile_func_end;
      return false;
    }

    total_read += step_size;
  }

  profile_func_end;
  return true;
}

func sz filestream_write(filestream* stm, const void* src, sz size) {
  profile_func_begin;
  sz end_pos = 0;

  if (!filestream_is_open(stm) || (src == NULL && size > 0) || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    thread_log_error("Rejected filestream write handle=%p size=%zu", (void*)stm, (size_t)size);
    profile_func_end;
    return 0;
  }
  assert(stm->kind == FILESTREAM_KIND_NATIVE || stm->kind == FILESTREAM_KIND_ARCHIVE);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    end_pos = (sz)SDL_WriteIO((SDL_IOStream*)stm->native_handle, src, (size_t)size);
    if (end_pos != size) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      thread_log_error("Failed to write native filestream handle=%p expected=%zu actual=%zu error=%s",
                       stm->native_handle,
                       (size_t)size,
                       (size_t)end_pos,
                       SDL_GetError());
    } else {
      filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    }
    profile_func_end;
    return end_pos;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (stm->mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    thread_log_warn("Rejected archive filestream write without write access path=%s", stm->archive_path.buf);
    profile_func_end;
    return 0;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    stm->cursor = stm->memory_size;
  }

  end_pos = stm->cursor + size;
  if (!filestream_reserve_memory(stm, end_pos)) {
    thread_log_error("Failed to reserve archive filestream capacity path=%s size=%zu",
                     stm->archive_path.buf,
                     (size_t)end_pos);
    profile_func_end;
    return 0;
  }

  memcpy(stm->memory_ptr + stm->cursor, src, size);
  stm->cursor = end_pos;
  if (stm->memory_size < end_pos) {
    stm->memory_size = end_pos;
  }
  stm->dirty = 1;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  profile_func_end;
  return size;
}

func b32 filestream_write_exact(filestream* stm, const void* src, sz size) {
  profile_func_begin;
  sz total_write = 0;
  const u8* src_ptr = (const u8*)src;

  while (total_write < size) {
    sz step_size = filestream_write(stm, src_ptr + total_write, size - total_write);
    if (step_size == 0) {
      profile_func_end;
      return false;
    }

    total_write += step_size;
  }

  profile_func_end;
  return true;
}

func b32 filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis) {
  profile_func_begin;
  i64 base_pos = 0;
  i64 new_pos = 0;
  SDL_IOWhence io_basis = SDL_IO_SEEK_SET;

  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    thread_log_error("Cannot seek closed filestream");
    profile_func_end;
    return false;
  }
  assert(basis == FILESTREAM_SEEK_BASIS_BEGIN || basis == FILESTREAM_SEEK_BASIS_CURRENT || basis == FILESTREAM_SEEK_BASIS_END);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    if (basis == FILESTREAM_SEEK_BASIS_CURRENT) {
      io_basis = SDL_IO_SEEK_CUR;
    } else if (basis == FILESTREAM_SEEK_BASIS_END) {
      io_basis = SDL_IO_SEEK_END;
    }

    if (SDL_SeekIO((SDL_IOStream*)stm->native_handle, (Sint64)offset, io_basis) < 0) {
      filestream_set_error(stm, FILESTREAM_ERROR_SEEK);
      thread_log_error("Failed to seek native filestream handle=%p offset=%lld error=%s",
                       stm->native_handle,
                       (long long)offset,
                       SDL_GetError());
      profile_func_end;
      return false;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    profile_func_end;
    return true;
  }

  if (basis == FILESTREAM_SEEK_BASIS_CURRENT) {
    base_pos = (i64)stm->cursor;
  } else if (basis == FILESTREAM_SEEK_BASIS_END) {
    base_pos = (i64)stm->memory_size;
  }

  new_pos = base_pos + offset;
  if (new_pos < 0 || (sz)new_pos > stm->memory_size) {
    filestream_set_error(stm, FILESTREAM_ERROR_SEEK);
    thread_log_warn("Rejected archive filestream seek path=%s offset=%lld basis=%u",
                    stm->archive_path.buf,
                    (long long)offset,
                    (u32)basis);
    profile_func_end;
    return false;
  }

  stm->cursor = (sz)new_pos;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  profile_func_end;
  return true;
}

func sz filestream_tell(const filestream* stm) {
  profile_func_begin;
  Sint64 cur_pos = 0;

  if (!filestream_is_open(stm)) {
    profile_func_end;
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    cur_pos = SDL_TellIO((SDL_IOStream*)stm->native_handle);
    if (cur_pos < 0) {
      profile_func_end;
      return 0;
    }
    profile_func_end;
    return (sz)cur_pos;
  }

  profile_func_end;
  return stm->cursor;
}

func sz filestream_size(const filestream* stm) {
  profile_func_begin;
  if (!filestream_is_open(stm)) {
    profile_func_end;
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    profile_func_end;
    return filestream_native_size((SDL_IOStream*)stm->native_handle);
  }

  profile_func_end;
  return stm->memory_size;
}

func b32 filestream_eof(const filestream* stm) {
  profile_func_begin;
  if (!filestream_is_open(stm)) {
    profile_func_end;
    return true;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    profile_func_end;
    return filestream_tell(stm) >= filestream_size(stm) ? true : false;
  }

  profile_func_end;
  return stm->cursor >= stm->memory_size ? true : false;
}

func filestream_error filestream_get_error(const filestream* stm) {
  if (stm == NULL) {
    return FILESTREAM_ERROR_NOT_OPEN;
  }

  return stm->error_code;
}
