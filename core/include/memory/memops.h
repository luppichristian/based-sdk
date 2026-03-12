// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../basic/utility_defines.h"

// =========================================================================
c_begin;
// =========================================================================

// Byte-wise fill. Mirrors memset semantics for raw byte ranges.
func void mem_set8(void* ptr, u8 value, sz size);

// Element-wise fills for fixed-width values.
// count is the number of 16-bit / 32-bit / 64-bit elements to write.
func void mem_set16(void* ptr, u16 value, sz count);
func void mem_set32(void* ptr, u32 value, sz count);
func void mem_set64(void* ptr, u64 value, sz count);

// Zeroes a raw byte range.
func void mem_zero(void* ptr, sz size);

// Copy / move / equality wrappers for raw byte ranges.
func void* mem_cpy(void* dst, const void* src, sz size);
func void* mem_move(void* dst, const void* src, sz size);
func b32 mem_cmp(const void* lhs, const void* rhs, sz size);

// Pointer alignment helpers for generic memory addresses.
// align should be a power of two when alignment is required.
func void* mem_align_forward(void* ptr, sz align);
func void* mem_align_backward(void* ptr, sz align);

// Integer-address alignment helpers. Useful when pointer arithmetic needs to
// stay in integer form before a final cast.
func up mem_align_forward_up(up ptr, sz align);
func up mem_align_backward_up(up ptr, sz align);

// Typed helpers for common single-object and array fills.
#define mem_set8_type(ptr, value, type) \
  mem_set8((ptr), (u8)(value), size_of(type))
#define mem_set8_array(ptr, value, count, type) \
  mem_set8((ptr), (u8)(value), size_of(type) * (count))
#define mem_set16_type(ptr, value) \
  mem_set16((ptr), (u16)(value), 1)
#define mem_set16_array(ptr, value, count) \
  mem_set16((ptr), (u16)(value), (count))
#define mem_set32_type(ptr, value) \
  mem_set32((ptr), (u32)(value), 1)
#define mem_set32_array(ptr, value, count) \
  mem_set32((ptr), (u32)(value), (count))
#define mem_set64_type(ptr, value) \
  mem_set64((ptr), (u64)(value), 1)
#define mem_set64_array(ptr, value, count) \
  mem_set64((ptr), (u64)(value), (count))
#define mem_zero_value(ptr) \
  mem_zero((ptr), size_of(*ptr))
#define mem_zero_type(ptr, type) \
  mem_zero((ptr), size_of(type))
#define mem_zero_array(ptr, type, count) \
  mem_zero((ptr), size_of(type) * (count))

// =========================================================================
c_end;
// =========================================================================
