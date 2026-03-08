// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../memory/buffer.h"
#include "path.h"

// Creates or truncates the file at src. Returns 1 on success, 0 otherwise.
func b32 file_create(const path* src);

// Removes the file at src. Returns 1 on success, 0 otherwise.
func b32 file_delete(const path* src);

// Renames the file at old_src to new_src. Returns 1 on success, 0 otherwise.
func b32 file_rename(const path* old_src, const path* new_src);

// Copies src to dst. Returns 1 on success, 0 otherwise.
func b32 file_copy(const path* src, const path* dst, b32 overwrite_existing);

// Returns 1 if src exists and is a regular file, 0 otherwise.
func b32 file_exists(const path* src);

// Writes the size of the file at src into out_size. Returns 1 on success, 0 otherwise.
func b32 file_get_size(const path* src, sz* out_size);

// Reads the entire file into memory allocated from alloc.
// On success out_data receives an owned buffer. Caller frees it with allocator_dealloc(alloc, ptr, size).
func b32 file_read_all(const path* src, allocator* alloc, buffer* out_data);

// Overwrites the file at src with data. Returns 1 on success, 0 otherwise.
func b32 file_write_all(const path* src, buffer data);

// Appends data to the file at src. Returns 1 on success, 0 otherwise.
func b32 file_append_all(const path* src, buffer data);

// Writes data to a temporary sibling file, then replaces src. Returns 1 on success, 0 otherwise.
func b32 file_write_all_atomic(const path* src, buffer data);
