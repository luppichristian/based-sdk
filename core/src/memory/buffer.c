// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/buffer.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include <string.h>

// =========================================================================
// Constructors
// =========================================================================

func buffer buffer_from(void* ptr, sz size) {
  profile_func_begin;
  if (size > 0 && ptr == NULL) {
    profile_func_end;
    return (buffer) {0};
  }
  buffer result = {.size = size, .ptr = ptr};
  profile_func_end;
  return result;
}

func buffer buffer_sub_from(buffer buff, sz offset) {
  profile_func_begin;
  if (offset >= buff.size) {
    buffer result = {.size = 0, .ptr = NULL};
    profile_func_end;
    return result;
  }

  profile_func_end;
  return buffer_from((u8*)buff.ptr + offset, buff.size - offset);
}

func buffer buffer_sub_from_sized(buffer buff, sz offset, sz size) {
  profile_func_begin;
  if (offset >= buff.size || size > buff.size - offset) {
    buffer result = {0};
    profile_func_end;
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + offset, size);
  profile_func_end;
  return result;
}

func buffer buffer_slice(buffer buff, sz start, sz end) {
  profile_func_begin;
  if (start > end || end > buff.size) {
    buffer result = {0};
    profile_func_end;
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + start, end - start);
  profile_func_end;
  return result;
}

func buffer buffer_split_offset(buffer* buff, sz offset) {
  profile_func_begin;
  if (buff == NULL) {
    profile_func_end;
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (offset > buff->size) {
    offset = buff->size;
  }
  buffer split = buffer_from(buff->ptr, offset);
  buff->ptr = (u8*)buff->ptr + offset;
  buff->size -= offset;
  profile_func_end;
  return split;
}

func buffer buffer_split_size(buffer* buff, sz size) {
  profile_func_begin;
  if (buff == NULL) {
    profile_func_end;
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (size > buff->size) {
    size = buff->size;
  }
  buffer split = buffer_from(buff->ptr, size);
  buff->ptr = (u8*)buff->ptr + size;
  buff->size -= size;
  profile_func_end;
  return split;
}

func b32 buffer_cmp(buffer aaa, buffer bbb) {
  profile_func_begin;
  if (aaa.size != bbb.size) {
    profile_func_end;
    return false;
  }
  if (aaa.size == 0) {
    profile_func_end;
    return true;
  }
  profile_func_end;
  return mem_cmp(aaa.ptr, bbb.ptr, aaa.size);
}

func b32 buffer_cmp_common(buffer aaa, buffer bbb) {
  profile_func_begin;
  sz common_size = aaa.size < bbb.size ? aaa.size : bbb.size;
  if (common_size == 0) {
    profile_func_end;
    return true;
  }
  profile_func_end;
  return mem_cmp(aaa.ptr, bbb.ptr, common_size);
}

func void* buffer_get_ptr(buffer buff, sz offset) {
  profile_func_begin;
  if (offset >= buff.size) {
    profile_func_end;
    return NULL;
  }
  profile_func_end;
  return (u8*)buff.ptr + offset;
}

func void* buffer_get_data(buffer buff, sz offset, sz read_size) {
  profile_func_begin;
  if (offset >= buff.size || read_size > buff.size - offset) {
    profile_func_end;
    return NULL;
  }
  profile_func_end;
  return (u8*)buff.ptr + offset;
}

func void buffer_set8(buffer buff, u8 value) {
  profile_func_begin;
  if (buff.ptr == NULL || buff.size == 0) {
    profile_func_end;
    return;
  }
  mem_set8(buff.ptr, value, buff.size);
  profile_func_end;
}

func void buffer_set16(buffer buff, u16 value) {
  profile_func_begin;
  mem_set16(buff.ptr, value, buff.size / size_of(u16));
  profile_func_end;
}

func void buffer_set32(buffer buff, u32 value) {
  profile_func_begin;
  mem_set32(buff.ptr, value, buff.size / size_of(u32));
  profile_func_end;
}

func void buffer_set64(buffer buff, u64 value) {
  profile_func_begin;
  mem_set64(buff.ptr, value, buff.size / size_of(u64));
  profile_func_end;
}

func void buffer_zero(buffer buff) {
  profile_func_begin;
  if (buff.ptr == NULL || buff.size == 0) {
    profile_func_end;
    return;
  }
  mem_zero(buff.ptr, buff.size);
  profile_func_end;
}
