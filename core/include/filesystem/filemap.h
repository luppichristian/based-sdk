// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

typedef enum filemap_access {
  FILEMAP_ACCESS_READ = 1,
  FILEMAP_ACCESS_READ_WRITE = 2,
  FILEMAP_ACCESS_COPY_ON_WRITE = 3,
} filemap_access;

typedef enum filemap_error {
  FILEMAP_ERROR_NONE = 0,
  FILEMAP_ERROR_INVALID_ARGUMENT = 1,
  FILEMAP_ERROR_OPEN_FAILED = 2,
  FILEMAP_ERROR_MAP_FAILED = 3,
  FILEMAP_ERROR_IO_FAILED = 4,
} filemap_error;

typedef struct filemap {
  void* data_ptr;
  sz data_size;
  void* native_file;
  void* native_mapping;
  b32 writable;
  b32 dirty;
  b32 uses_fallback_copy;
  path source_path;
} filemap;

// Opens src as a mapped file view. Returns an empty map on failure.
func filemap filemap_open(const path* src, filemap_access access);

// Returns 1 when map currently references an open mapping.
func b32 filemap_is_open(const filemap* map);

// Flushes pending writes back to the underlying file. Returns 1 on success, 0 otherwise.
func b32 filemap_flush(filemap* map);

// Releases the mapping and resets map to an empty value.
func void filemap_close(filemap* map);

// Returns 1 if map was opened with write permissions.
func b32 filemap_is_writable(const filemap* map);
// Marks a writable mapping as dirty so filemap_flush can persist updates.
func void filemap_mark_dirty(filemap* map);
// Returns the last filemap error reported by the current thread.
func filemap_error filemap_get_last_error(void);
