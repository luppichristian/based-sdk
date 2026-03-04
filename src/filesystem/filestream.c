// MIT License
// Copyright (c) 2026 Christian Luppi

#include "filesystem/filestream.h"

#include "filesystem/archive.h"
#include "memory/buffer.h"

#include "../sdl3_include.h"

#include <stdlib.h>
#include <string.h>

func filestream_error filestream_set_error(filestream* stm, filestream_error error_code) {
  if (stm != NULL) {
    stm->error_code = error_code;
  }

  return error_code;
}

func filestream filestream_empty(void) {
  filestream stm;
  memset(&stm, 0, size_of(stm));
  stm.archive_path = path_from_cstr("");
  return stm;
}

func const c8* filestream_mode_string(u32 mode_flags) {
  if ((mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    return (mode_flags & FILESTREAM_OPEN_READ) != 0 ? "a+b" : "ab";
  }

  if ((mode_flags & FILESTREAM_OPEN_WRITE) != 0) {
    if ((mode_flags & FILESTREAM_OPEN_READ) != 0) {
      if ((mode_flags & FILESTREAM_OPEN_TRUNCATE) != 0) {
        return "w+b";
      }
      return "r+b";
    }

    return "wb";
  }

  return "rb";
}

func sz filestream_native_size(SDL_IOStream* file_ptr) {
  Sint64 size_value = 0;

  if (file_ptr == NULL) {
    return 0;
  }

  size_value = SDL_GetIOSize(file_ptr);
  if (size_value < 0) {
    return 0;
  }

  return (sz)size_value;
}

func path filestream_normalize_archive_path(const path* src) {
  path item_path = path_from_cstr(src != NULL ? src->buf : "");
  path_normalize(&item_path);
  path_remove_trailing_slash(&item_path);
  return item_path;
}

func archive_entry* filestream_find_archive_entry(archive* arc, const path* src) {
  path item_path = filestream_normalize_archive_path(src);
  sz item_idx = 0;

  if (arc == NULL) {
    return NULL;
  }

  for (item_idx = 0; item_idx < arc->entry_count; item_idx += 1) {
    path ent_path = filestream_normalize_archive_path(&arc->entries[item_idx].item_path);
    if (cstr8_cmp(ent_path.buf, item_path.buf) == 0) {
      return &arc->entries[item_idx];
    }
  }

  return NULL;
}

func b32 filestream_reserve_memory(filestream* stm, sz min_capacity) {
  u8* new_ptr = NULL;
  sz new_capacity = 0;

  if (stm->memory_capacity >= min_capacity) {
    return 1;
  }

  new_capacity = stm->memory_capacity == 0 ? 64 : stm->memory_capacity;
  while (new_capacity < min_capacity) {
    if (new_capacity > SZ_MAX / 2) {
      new_capacity = min_capacity;
      break;
    }
    new_capacity *= 2;
  }

  new_ptr = (u8*)realloc(stm->memory_ptr, (size_t)new_capacity);
  if (new_ptr == NULL) {
    filestream_set_error(stm, FILESTREAM_ERROR_ALLOC);
    return 0;
  }

  stm->memory_ptr = new_ptr;
  stm->memory_capacity = new_capacity;
  return 1;
}

func filestream filestream_open(const path* src, u32 mode_flags) {
  filestream stm = filestream_empty();
  SDL_IOStream* file_ptr = NULL;
  const c8* mode_str = filestream_mode_string(mode_flags);

  file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", mode_str);
  if (file_ptr == NULL && (mode_flags & FILESTREAM_OPEN_CREATE) != 0 &&
      (mode_flags & FILESTREAM_OPEN_APPEND) == 0 &&
      (mode_flags & FILESTREAM_OPEN_READ) != 0 &&
      (mode_flags & FILESTREAM_OPEN_WRITE) != 0) {
    file_ptr = SDL_IOFromFile(src != NULL ? src->buf : "", "w+b");
  }

  if (file_ptr == NULL) {
    return stm;
  }

  stm.kind = FILESTREAM_KIND_NATIVE;
  stm.mode_flags = mode_flags;
  stm.native_handle = file_ptr;
  stm.error_code = FILESTREAM_ERROR_NONE;
  return stm;
}

func filestream filestream_open_archive(archive* arc, const path* src, u32 mode_flags) {
  filestream stm = filestream_empty();
  archive_entry* ent = NULL;

  if (arc == NULL || src == NULL) {
    return stm;
  }

  ent = filestream_find_archive_entry(arc, src);
  if (ent == NULL && (mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    return stm;
  }

  stm.kind = FILESTREAM_KIND_ARCHIVE;
  stm.mode_flags = mode_flags;
  stm.archive_ref = arc;
  stm.archive_path = filestream_normalize_archive_path(src);

  if (ent != NULL && !ent->is_directory && ent->data_size > 0) {
    if (!filestream_reserve_memory(&stm, ent->data_size)) {
      filestream_close(&stm);
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

  return stm;
}

func b32 filestream_flush(filestream* stm) {
  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    if (!SDL_FlushIO((SDL_IOStream*)stm->native_handle)) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      return 0;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    return 1;
  }

  if (stm->dirty && stm->archive_ref != NULL) {
    buffer data = buffer_from(stm->memory_ptr, stm->memory_size);
    if (!archive_write_all(stm->archive_ref, &stm->archive_path, data)) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
      return 0;
    }

    stm->dirty = 0;
  }

  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  return 1;
}

func void filestream_close(filestream* stm) {
  SDL_IOStream* file_ptr = NULL;

  if (stm == NULL) {
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
    free(stm->memory_ptr);
  }

  *stm = filestream_empty();
}

func b32 filestream_is_open(const filestream* stm) {
  if (stm == NULL) {
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    return stm->native_handle != NULL ? 1 : 0;
  }

  if (stm->kind == FILESTREAM_KIND_ARCHIVE) {
    return stm->archive_ref != NULL ? 1 : 0;
  }

  return 0;
}

func sz filestream_read(filestream* stm, void* dst, sz size) {
  sz available = 0;
  sz read_size = 0;

  if (!filestream_is_open(stm) || dst == NULL || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    return 0;
  }

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
    return read_size;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_READ) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    return 0;
  }

  if (stm->cursor >= stm->memory_size) {
    filestream_set_error(stm, FILESTREAM_ERROR_EOF);
    return 0;
  }

  available = stm->memory_size - stm->cursor;
  read_size = size < available ? size : available;
  memcpy(dst, stm->memory_ptr + stm->cursor, read_size);
  stm->cursor += read_size;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  return read_size;
}

func b32 filestream_read_exact(filestream* stm, void* dst, sz size) {
  sz total_read = 0;
  u8* dst_ptr = (u8*)dst;

  while (total_read < size) {
    sz step_size = filestream_read(stm, dst_ptr + total_read, size - total_read);
    if (step_size == 0) {
      return 0;
    }

    total_read += step_size;
  }

  return 1;
}

func sz filestream_write(filestream* stm, const void* src, sz size) {
  sz end_pos = 0;

  if (!filestream_is_open(stm) || (src == NULL && size > 0) || size == 0) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    end_pos = (sz)SDL_WriteIO((SDL_IOStream*)stm->native_handle, src, (size_t)size);
    if (end_pos != size) {
      filestream_set_error(stm, FILESTREAM_ERROR_IO);
    } else {
      filestream_set_error(stm, FILESTREAM_ERROR_NONE);
    }
    return end_pos;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_WRITE) == 0 &&
      (stm->mode_flags & FILESTREAM_OPEN_APPEND) == 0) {
    filestream_set_error(stm, FILESTREAM_ERROR_ACCESS);
    return 0;
  }

  if ((stm->mode_flags & FILESTREAM_OPEN_APPEND) != 0) {
    stm->cursor = stm->memory_size;
  }

  end_pos = stm->cursor + size;
  if (!filestream_reserve_memory(stm, end_pos)) {
    return 0;
  }

  memcpy(stm->memory_ptr + stm->cursor, src, size);
  stm->cursor = end_pos;
  if (stm->memory_size < end_pos) {
    stm->memory_size = end_pos;
  }
  stm->dirty = 1;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  return size;
}

func b32 filestream_write_exact(filestream* stm, const void* src, sz size) {
  sz total_write = 0;
  const u8* src_ptr = (const u8*)src;

  while (total_write < size) {
    sz step_size = filestream_write(stm, src_ptr + total_write, size - total_write);
    if (step_size == 0) {
      return 0;
    }

    total_write += step_size;
  }

  return 1;
}

func b32 filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis) {
  i64 base_pos = 0;
  i64 new_pos = 0;
  SDL_IOWhence io_basis = SDL_IO_SEEK_SET;

  if (!filestream_is_open(stm)) {
    if (stm != NULL) {
      filestream_set_error(stm, FILESTREAM_ERROR_NOT_OPEN);
    }
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    if (basis == FILESTREAM_SEEK_BASIS_CURRENT) {
      io_basis = SDL_IO_SEEK_CUR;
    } else if (basis == FILESTREAM_SEEK_BASIS_END) {
      io_basis = SDL_IO_SEEK_END;
    }

    if (SDL_SeekIO((SDL_IOStream*)stm->native_handle, (Sint64)offset, io_basis) < 0) {
      filestream_set_error(stm, FILESTREAM_ERROR_SEEK);
      return 0;
    }

    filestream_set_error(stm, FILESTREAM_ERROR_NONE);
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
    return 0;
  }

  stm->cursor = (sz)new_pos;
  filestream_set_error(stm, FILESTREAM_ERROR_NONE);
  return 1;
}

func sz filestream_tell(const filestream* stm) {
  Sint64 cur_pos = 0;

  if (!filestream_is_open(stm)) {
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    cur_pos = SDL_TellIO((SDL_IOStream*)stm->native_handle);
    if (cur_pos < 0) {
      return 0;
    }
    return (sz)cur_pos;
  }

  return stm->cursor;
}

func sz filestream_size(const filestream* stm) {
  if (!filestream_is_open(stm)) {
    return 0;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    return filestream_native_size((SDL_IOStream*)stm->native_handle);
  }

  return stm->memory_size;
}

func b32 filestream_eof(const filestream* stm) {
  if (!filestream_is_open(stm)) {
    return 1;
  }

  if (stm->kind == FILESTREAM_KIND_NATIVE) {
    return filestream_tell(stm) >= filestream_size(stm) ? 1 : 0;
  }

  return stm->cursor >= stm->memory_size ? 1 : 0;
}

func filestream_error filestream_get_error(const filestream* stm) {
  if (stm == NULL) {
    return FILESTREAM_ERROR_NOT_OPEN;
  }

  return stm->error_code;
}
