// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/memops.h"
#include "basic/profiler.h"

#include <string.h>

typedef union mem_ptr_cast {
  void* ptr;
  up addr;
} mem_ptr_cast;

func void mem_set8(void* ptr, u8 value, sz size) {
  profile_func_begin;
  if (ptr == NULL || size == 0) {
    profile_func_end;
    return;
  }
  memset(ptr, value, size);
  profile_func_end;
}

func void mem_set16(void* ptr, u16 value, sz count) {
  profile_func_begin;
  if (ptr == NULL || count == 0) {
    profile_func_end;
    return;
  }
  u16* dst = (u16*)ptr;
  for (sz idx = 0; idx < count; idx++) {
    dst[idx] = value;
  }
  profile_func_end;
}

func void mem_set32(void* ptr, u32 value, sz count) {
  profile_func_begin;
  if (ptr == NULL || count == 0) {
    profile_func_end;
    return;
  }
  u32* dst = (u32*)ptr;
  for (sz idx = 0; idx < count; idx++) {
    dst[idx] = value;
  }
  profile_func_end;
}

func void mem_set64(void* ptr, u64 value, sz count) {
  profile_func_begin;
  if (ptr == NULL || count == 0) {
    profile_func_end;
    return;
  }
  u64* dst = (u64*)ptr;
  for (sz idx = 0; idx < count; idx++) {
    dst[idx] = value;
  }
  profile_func_end;
}

func void mem_zero(void* ptr, sz size) {
  profile_func_begin;
  if (ptr == NULL || size == 0) {
    profile_func_end;
    return;
  }
  memset(ptr, 0, size);
  profile_func_end;
}

func void* mem_cpy(void* dst, const void* src, sz size) {
  profile_func_begin;
  if (size == 0) {
    profile_func_end;
    return dst;
  }
  void* result = memcpy(dst, src, size);
  profile_func_end;
  return result;
}

func void* mem_move(void* dst, const void* src, sz size) {
  profile_func_begin;
  if (size == 0) {
    profile_func_end;
    return dst;
  }
  void* result = memmove(dst, src, size);
  profile_func_end;
  return result;
}

func b32 mem_cmp(const void* lhs, const void* rhs, sz size) {
  profile_func_begin;
  if (size == 0) {
    profile_func_end;
    return true;
  }
  b32 result = memcmp(lhs, rhs, size) == 0 ? true : false;
  profile_func_end;
  return result;
}

func void* mem_align_forward(void* ptr, sz align) {
  profile_func_begin;
  mem_ptr_cast cast = {.ptr = ptr};
  cast.addr = mem_align_forward_up(cast.addr, align);
  profile_func_end;
  return cast.ptr;
}

func void* mem_align_backward(void* ptr, sz align) {
  profile_func_begin;
  mem_ptr_cast cast = {.ptr = ptr};
  cast.addr = mem_align_backward_up(cast.addr, align);
  profile_func_end;
  return cast.ptr;
}

func up mem_align_forward_up(up ptr, sz align) {
  return align <= 1 ? ptr : align_up(ptr, align);
}

func up mem_align_backward_up(up ptr, sz align) {
  return align <= 1 ? ptr : align_down(ptr, align);
}
