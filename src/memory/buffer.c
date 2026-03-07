// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/buffer.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include <string.h>

// =========================================================================
// Constructors
// =========================================================================

func buffer buffer_from(void* ptr, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (size > 0 && ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (buffer) {0};
  }
  buffer result = {.size = size, .ptr = ptr};
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func buffer buffer_sub_from(buffer buff, sz offset) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (offset >= buff.size) {
    buffer result = {.size = 0, .ptr = NULL};
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return buffer_from((u8*)buff.ptr + offset, buff.size - offset);
}

func buffer buffer_sub_from_sized(buffer buff, sz offset, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (offset >= buff.size || size > buff.size - offset) {
    buffer result = {0};
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + offset, size);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func buffer buffer_slice(buffer buff, sz start, sz end) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (start > end || end > buff.size) {
    buffer result = {0};
    TracyCZoneEnd(__tracy_zone_ctx);
    return result;
  }

  buffer result = buffer_from((u8*)buff.ptr + start, end - start);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

func buffer buffer_split_offset(buffer* buff, sz offset) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (buff == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (offset > buff->size) {
    offset = buff->size;
  }
  buffer split = buffer_from(buff->ptr, offset);
  buff->ptr = (u8*)buff->ptr + offset;
  buff->size -= offset;
  TracyCZoneEnd(__tracy_zone_ctx);
  return split;
}

func buffer buffer_split_size(buffer* buff, sz size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (buff == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return (buffer) {0};
  }
  assert(buff != NULL);
  if (size > buff->size) {
    size = buff->size;
  }
  buffer split = buffer_from(buff->ptr, size);
  buff->ptr = (u8*)buff->ptr + size;
  buff->size -= size;
  TracyCZoneEnd(__tracy_zone_ctx);
  return split;
}

func b32 buffer_cmp(buffer aaa, buffer bbb) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (aaa.size != bbb.size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  if (aaa.size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return memcmp(aaa.ptr, bbb.ptr, aaa.size) == 0 ? 1 : 0;
}

func b32 buffer_cmp_common(buffer aaa, buffer bbb) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  sz common_size = aaa.size < bbb.size ? aaa.size : bbb.size;
  if (common_size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return memcmp(aaa.ptr, bbb.ptr, common_size) == 0 ? 1 : 0;
}

func void* buffer_get_ptr(buffer buff, sz offset) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (offset >= buff.size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u8*)buff.ptr + offset;
}

func void* buffer_get_data(buffer buff, sz offset, sz read_size) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (offset >= buff.size || read_size > buff.size - offset) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return (u8*)buff.ptr + offset;
}

func void buffer_set8(buffer buff, u8 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (buff.ptr == NULL || buff.size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  memset(buff.ptr, value, buff.size);
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void buffer_set16(buffer buff, u16 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u16* data = (u16*)buff.ptr;
  sz count = buff.size / size_of(u16);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void buffer_set32(buffer buff, u32 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u32* data = (u32*)buff.ptr;
  sz count = buff.size / size_of(u32);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void buffer_set64(buffer buff, u64 value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  u64* data = (u64*)buff.ptr;
  sz count = buff.size / size_of(u64);
  for (sz idx = 0; idx < count; idx++) {
    data[idx] = value;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void buffer_zero(buffer buff) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (buff.ptr == NULL || buff.size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  memset(buff.ptr, 0, buff.size);
  TracyCZoneEnd(__tracy_zone_ctx);
}
