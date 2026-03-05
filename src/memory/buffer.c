// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/buffer.h"
#include "basic/assert.h"
#include <string.h>

// =========================================================================
// Constructors
// =========================================================================

func buffer buffer_from(void* ptr, sz size) {
  if (size > 0 && ptr == NULL) {
    return (buffer) {0};
  }
  buffer result = {.size = size, .ptr = ptr};
  return result;
}

func buffer buffer_sub_from(buffer buff, sz offset) {
  if (offset >= buff.size) {
    buffer result = {.size = 0, .ptr = NULL};
    return result;
  }

  return buffer_from((u8*)buff.ptr + offset, buff.size - offset);
}

func buffer buffer_sub_from_sized(buffer buff, sz offset, sz size) {
  if (offset >= buff.size || size > buff.size - offset) {
    buffer result = {0};
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + offset, size);
  return result;
}

func buffer buffer_slice(buffer buff, sz start, sz end) {
  if (start > end || end > buff.size) {
    buffer result = {0};
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + start, end - start);
  return result;
}

func buffer buffer_split_offset(buffer* buff, sz offset) {
  if (buff == NULL) {
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (offset > buff->size) {
    offset = buff->size;
  }
  buffer split = buffer_from(buff->ptr, offset);
  buff->ptr = (u8*)buff->ptr + offset;
  buff->size -= offset;
  return split;
}

func buffer buffer_split_size(buffer* buff, sz size) {
  if (buff == NULL) {
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (size > buff->size) {
    size = buff->size;
  }
  buffer split = buffer_from(buff->ptr, size);
  buff->ptr = (u8*)buff->ptr + size;
  buff->size -= size;
  return split;
}

func b32 buffer_cmp(buffer aaa, buffer bbb) {
  if (aaa.size != bbb.size) {
    return 0;
  }
  if (aaa.size == 0) {
    return 1;
  }
  return memcmp(aaa.ptr, bbb.ptr, aaa.size) == 0 ? 1 : 0;
}

func b32 buffer_cmp_common(buffer aaa, buffer bbb) {
  sz common_size = aaa.size < bbb.size ? aaa.size : bbb.size;
  if (common_size == 0) {
    return 1;
  }
  return memcmp(aaa.ptr, bbb.ptr, common_size) == 0 ? 1 : 0;
}

func void* buffer_get_ptr(buffer buff, sz offset) {
  if (offset >= buff.size) {
    return NULL;
  }
  return (u8*)buff.ptr + offset;
}

func void* buffer_get_data(buffer buff, sz offset, sz read_size) {
  if (offset >= buff.size || read_size > buff.size - offset) {
    return NULL;
  }
  return (u8*)buff.ptr + offset;
}

func void buffer_set8(buffer buff, u8 value) {
  if (buff.ptr == NULL || buff.size == 0) {
    return;
  }
  memset(buff.ptr, value, buff.size);
}

func void buffer_set16(buffer buff, u16 value) {
  u16* data = (u16*)buff.ptr;
  sz count = buff.size / size_of(u16);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
}

func void buffer_set32(buffer buff, u32 value) {
  u32* data = (u32*)buff.ptr;
  sz count = buff.size / size_of(u32);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
}

func void buffer_set64(buffer buff, u64 value) {
  u64* data = (u64*)buff.ptr;
  sz count = buff.size / size_of(u64);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
}

func void buffer_zero(buffer buff) {
  if (buff.ptr == NULL || buff.size == 0) {
    return;
  }
  memset(buff.ptr, 0, buff.size);
}
