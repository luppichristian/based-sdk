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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (stm != NULL) {
    stm->error_code = error_code;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return error_code;
}

func allocator filestream_allocator_resolve(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = thread_get_allocator();
  if (alloc.alloc_fn != NULL && alloc.dealloc_fn != NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return global_get_allocator();
}

func filestream filestream_empty(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  filestream stm;
  memset(&stm, 0, size_of(stm));
  stm.archive_path = path_from_cstr("");
  TracyCZoneEnd(__tracy_zone_ctx);
  return stm;
}

func cstr8 filestream_mode_string(u32 mode_flags) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if ((mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (mode_flags & FILESTREAM_OPEN_READ) != 0 ? "a+b" : "ab";
  }

  if ((mode_flags & FILESTREAM_OPEN_WRITE) != 0) {
    if ((mode_flags & FILESTREAM_OPEN_READ) != 0) {
      if ((mode_flags & FILESTREAM_OPEN_TRUNCATE) != 0) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return "w+b";
      }
      TracyCZoneEnd(__tracy_zone_ctx);
      return "r+b";
    }

    TracyCZoneEnd(__tracy_zone_ctx);
    return "wb";
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return "rb";
}

func sz filestream_native_size(SDL_IOStream* file_ptr) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  Sint64 size_value = 0;

  if (file_ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  size_value = SDL_GetIOSize(file_ptr);
  if (size_value < 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return (sz)size_value;
}

func path filestream_normalize_archive_path(const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path item_path = path_from_cstr(src != NULL ? src->buf : "");
  path_normalize(&item_path);
  path_remove_trailing_slash(&item_path);
  TracyCZoneEnd(__tracy_zone_ctx);
  return item_path;
}

func archive_entry* filestream_find_archive_entry(archive* arc, const path* src) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  path item_path = filestream_normalize_archive_path(src);
  sz item_idx = 0;

  if (arc == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    path ent_path = filestream_normalize_archive_path(&arc->entries[item_idx].item_path);
    if (cstr8_cmp(ent_path.buf, item_path.buf) == 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return &arc->entries[item_idx];
    }
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

func b32 filestream_reserve_memory(filestream* stm, sz min_capacity) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = {0};
  u8* new_ptr = NULL;
  sz new_capacity = 0;

  if (stm == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (stm->memory_capacity >= min_capacity) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
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

  new_ptr = (u8*)allocator_realloc(&alloc, stm->memory_ptr, stm->memory_capacity, new_capacity);
  if (new_ptr == NULL) {
    filestream_set_error(stm, FILESTREAM_ERROR_ALLOC);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  stm->memory_ptr = new_ptr;
  stm->memory_capacity = new_capacity;
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func filestream filestream_open(const path* src, u32 mode_flags) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
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
    thread_log_debug("filestream_open: failed src=%s mode=%s", src != NULL ? src->buf : "<null>", mode_str);
    TracyCZoneEnd(__tracy_zone_ctx);
    return stm;
  }
  if (src == NULL) {
    SDL_CloseIO(file_ptr);
    TracyCZoneEnd(__tracy_zone_ctx);
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
  if (!msg_post(&lifecycle_msg)) {
    filestream_close(&stm);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filestream_empty();
  }
  thread_log_trace("filestream_open: native src=%s flags=0x%08x", src->buf, mode_flags);
  TracyCZoneEnd(__tracy_zone_ctx);
  return stm;
}

func filestream filestream_open_archive(archive* arc, const path* src, u32 mode_flags) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  filestream stm = filestream_empty();
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return stm;
  }
  assert(src->buf[0] != '\0');

  ent = filestream_find_archive_entry(arc, src);
  if (ent == NULL && (mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return stm;
  }

  stm.kind = FILESTREAM_KIND_ARCHIVE;
  stm.mode_flags = mode_flags;
  stm.archive_ref = arc;
  stm.archive_path = filestream_normalize_archive_path(src);

  if (ent != NULL && !ent->is_directory && ent->data_size > 0) {
    if (!filestream_reserve_memory(&stm, ent->data_size)) {
      filestream_close(&stm);
      TracyCZoneEnd(__tracy_zone_ctx);
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
  if (!msg_post(&lifecycle_msg)) {
    filestream_close(&stm);
    TracyCZoneEnd(__tracy_zone_ctx);
    return filestream_empty();
  }
  thread_log_trace("filestream_open_archive: path=%s flags=0x%08x", stm.archive_path.buf, mode_flags);

  TracyCZoneEnd(__tracy_zone_ctx);
  return stm;
}

func b32 filestream_flush(filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    if (!SDL_FlushIO((SDL_IOStream*)stm->native_handle)) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    thread_log_trace("filestream_flush: native handle=%p", stm->native_handle);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (stm->dirty && stm->archive_ref != NULL) {
    buffer data = buffer_from(stm->memory_ptr, stm->memory_size);
    if (!archive_write_all(stm->archive_ref, &stm->archive_path, data)) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    stm->dirty = 0;
  }

  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void filestream_close(filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  allocator alloc = filestream_allocator_resolve();
  SDL_IOStream* file_ptr = NULL;

  if (stm == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_FILESTREAM,
                                                     .object_ptr = stm,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    file_ptr = (SDL_IOStream*)stm->native_handle;
    if (file_ptr != NULL) {
      (void)filestream_flush(stm);
      SDL_CloseIO(file_ptr);
    }
  } else if (stm->kind == FILESTREAM_KIND_ARCHIVE) {
    (void)filestream_flush(stm);
    if (stm->memory_ptr != NULL) {
      allocator_dealloc(&alloc, stm->memory_ptr, stm->memory_capacity);
    }
  }

  thread_log_trace("filestream_close: kind=%u", (u32)stm->kind);
  *stm = filestream_empty();
  TracyCZoneEnd(__tracy_zone_ctx);
}

func b32 filestream_is_open(const filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (stm == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return stm->native_handle != NULL ? 1 : 0;
  }

  if (stm->kind == FILESTREAM_KIND_ARCHIVE) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return stm->archive_ref != NULL ? 1 : 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func sz filestream_read(filestream* stm, void* dst, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz available = 0;
  sz read_size = 0;

  if (!filestream_is_open(stm) || dst == NULL || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(stm->kind == FILESTREAM_KIND_NATIVE || stm->kind == FILESTREAM_KIND_ARCHIVE);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    read_size = (sz)SDL_ReadIO((SDL_IOStream*)stm->native_handle, dst, (size_t)size);
    if (read_size == 0 && size > 0) {
      if (filestream_eof(stm)) {
        filestream_set_error(stm, FILESTREAM_ERROR_EOF);
      } else {
        filestream_set_error(stm, FILESTREAM_ERROR_IO);
      }
    } else {
      filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return read_size;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_READ) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (stm->cursor >= stm->memory_size) {
    filestream_set_error(stm, FILESTREAM_ERROR_EOF);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  available = stm->memory_size - stm->cursor;
  read_size = size < available ? size : available;
  memcpy(dst, stm->memory_ptr + stm->cursor, read_size);
  stm->cursor += read_size;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return read_size;
}

func b32 filestream_read_exact(filestream* stm, void* dst, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz total_read = 0;
  u8* dst_ptr = (u8*)dst;

  while (total_read < size) {
    sz step_size = filestream_read(stm, dst_ptr + total_read, size - total_read);
    if (step_size == 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    total_read += step_size;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func sz filestream_write(filestream* stm, const void* src, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz end_pos = 0;

  if (!filestream_is_open(stm) || (src == NULL && size > 0) || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(stm->kind == FILESTREAM_KIND_NATIVE || stm->kind == FILESTREAM_KIND_ARCHIVE);

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    end_pos = (sz)SDL_WriteIO((SDL_IOStream*)stm->native_handle, src, (size_t)size);
    if (end_pos != size) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
    } else {
      filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return end_pos;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (stm->mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    stm->cursor = stm->memory_size;
  }

  end_pos = stm->cursor + size;
  if (!filestream_reserve_memory(stm, end_pos)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  memcpy(stm->memory_ptr + stm->cursor, src, size);
  stm->cursor = end_pos;
  if (stm->memory_size < end_pos) {
    stm->memory_size = end_pos;
  }
  stm->dirty = 1;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return size;
}

func b32 filestream_write_exact(filestream* stm, const void* src, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz total_write = 0;
  const u8* src_ptr = (const u8*)src;

  while (total_write < size) {
    sz step_size = filestream_write(stm, src_ptr + total_write, size - total_write);
    if (step_size == 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    total_write += step_size;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  i64 base_pos = 0;
  i64 new_pos = 0;
  SDL_IOWhence io_basis = SDL_IO_SEEK_SET;

  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
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
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (basis == FILESTREAM_SEEK_BASIS_CURRENT) {
    base_pos = (i64)stm->cursor;
  } else if (basis == FILESTREAM_SEEK_BASIS_END) {
    base_pos = (i64)stm->memory_size;
  }

  new_pos = base_pos + offset;
  if (new_pos < 0 || (sz)new_pos > stm->memory_size) {
    filestream_set_error(stm, FILESTREAM_ERROR_SEEK);
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  stm->cursor = (sz)new_pos;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func sz filestream_tell(const filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  Sint64 cur_pos = 0;

  if (!filestream_is_open(stm)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    cur_pos = SDL_TellIO((SDL_IOStream*)stm->native_handle);
    if (cur_pos < 0) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
    TracyCZoneEnd(__tracy_zone_ctx);
    return (sz)cur_pos;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return stm->cursor;
}

func sz filestream_size(const filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!filestream_is_open(stm)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return filestream_native_size((SDL_IOStream*)stm->native_handle);
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return stm->memory_size;
}

func b32 filestream_eof(const filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (!filestream_is_open(stm)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return filestream_tell(stm) >= filestream_size(stm) ? 1 : 0;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return stm->cursor >= stm->memory_size ? 1 : 0;
}

func filestream_error filestream_get_error(const filestream* stm) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (stm == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return FILESTREAM_ERROR_NOT_OPEN;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return stm->error_code;
}
