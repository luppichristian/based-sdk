// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct buffer {
  sz size;
  void* ptr;
} buffer;

// Simple constructor.
func buffer buffer_from(void* ptr, sz size);

// Returns a new buffer that is a slice of the original buffer from the specified offset to the end.
func buffer buffer_sub_from(buffer buff, sz offset);

// Returns a new buffer that is a slice of the original buffer from the specified offset and size.
func buffer buffer_sub_from_sized(buffer buff, sz offset, sz size);

// Returns a new buffer that is a slice of the original buffer from start to end.
func buffer buffer_slice(buffer buff, sz start, sz end);

// Splits the buffer at the specified offset.
func buffer buffer_split_offset(buffer* buff, sz offset);

// Splits the buffer at the specified size.
// The original buffer is modified to point to the remaining data, and a new buffer is returned that points to the split data.
func buffer buffer_split_size(buffer* buff, sz size);

// Compares two buffers for equality. Returns 1 if they are equal, 0 otherwise.
func b32 buffer_cmp(buffer a, buffer b);

// Compares two buffers for equality, ignoring their sizes. Returns 1 if they are equal, 0 otherwise.
// Ignores size, compares up to the smaller size of the two buffers.
func b32 buffer_cmp_common(buffer a, buffer b);

// Returns a pointer to the data at the specified offset, or nullptr if the offset is out of bounds.
func void* buffer_get_ptr(buffer buff, sz offset);

// Returns a pointer to the data at the specified offset, or nullptr if the offset is out of bounds.
// Also checks that the read size does not exceed the buffer size.
func void* buffer_get_data(buffer buff, sz offset, sz read_size);

// Buffer contents setters.
// Make sure the size of the buffer mod the value size is 0, otherwise the behavior is undefined.
func void buffer_set8(buffer buff, u8 value);
func void buffer_set16(buffer buff, u16 value);
func void buffer_set32(buffer buff, u32 value);
func void buffer_set64(buffer buff, u64 value);
// Fills the entire buffer with zeros.
func void buffer_zero(buffer buff);

// =========================================================================
c_end;
// =========================================================================
