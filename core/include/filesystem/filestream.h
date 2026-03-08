// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/utility_defines.h"
#include "path.h"

typedef struct archive archive;

typedef enum filestream_kind {
  FILESTREAM_KIND_NONE = 0,
  FILESTREAM_KIND_NATIVE = 1,
  FILESTREAM_KIND_ARCHIVE = 2,
} filestream_kind;

typedef enum filestream_seek_basis {
  FILESTREAM_SEEK_BASIS_BEGIN = 0,
  FILESTREAM_SEEK_BASIS_CURRENT = 1,
  FILESTREAM_SEEK_BASIS_END = 2,
} filestream_seek_basis;

typedef enum filestream_error {
  FILESTREAM_ERROR_NONE = 0,
  FILESTREAM_ERROR_NOT_OPEN = 1,
  FILESTREAM_ERROR_ACCESS = 2,
  FILESTREAM_ERROR_EOF = 3,
  FILESTREAM_ERROR_SEEK = 4,
  FILESTREAM_ERROR_IO = 5,
  FILESTREAM_ERROR_ALLOC = 6,
} filestream_error;

enum {
  FILESTREAM_OPEN_READ = bit(0),
  FILESTREAM_OPEN_WRITE = bit(1),
  FILESTREAM_OPEN_APPEND = bit(2),
  FILESTREAM_OPEN_CREATE = bit(3),
  FILESTREAM_OPEN_TRUNCATE = bit(4),
};

typedef struct filestream {
  filestream_kind kind;
  u32 mode_flags;
  void* native_handle;
  archive* archive_ref;
  path archive_path;
  u8* memory_ptr;
  sz memory_size;
  sz memory_capacity;
  sz cursor;
  b32 dirty;
  filestream_error error_code;
} filestream;

// Opens a native filesystem stream from src.
func filestream filestream_open(const path* src, u32 mode_flags);

// Opens a stream that targets a file entry inside arc.
func filestream filestream_open_archive(archive* arc, const path* src, u32 mode_flags);

// Flushes pending archive writes and releases the stream.
func void filestream_close(filestream* stm);

// Returns 1 when stm currently references an open stream.
func b32 filestream_is_open(const filestream* stm);

// Flushes pending writes without closing the stream. Returns 1 on success, 0 otherwise.
func b32 filestream_flush(filestream* stm);

// Reads up to size bytes into dst. Returns the number of bytes read.
func sz filestream_read(filestream* stm, void* dst, sz size);

// Reads exactly size bytes into dst. Returns 1 on success, 0 otherwise.
func b32 filestream_read_exact(filestream* stm, void* dst, sz size);

// Writes up to size bytes from src. Returns the number of bytes written.
func sz filestream_write(filestream* stm, const void* src, sz size);

// Writes exactly size bytes from src. Returns 1 on success, 0 otherwise.
func b32 filestream_write_exact(filestream* stm, const void* src, sz size);

// Seeks the current cursor. Returns 1 on success, 0 otherwise.
func b32 filestream_seek(filestream* stm, i64 offset, filestream_seek_basis basis);

// Returns the current cursor position.
func sz filestream_tell(const filestream* stm);

// Returns the logical byte size of the underlying stream.
func sz filestream_size(const filestream* stm);

// Returns 1 when no more bytes can be read, 0 otherwise.
func b32 filestream_eof(const filestream* stm);

// Returns the last stream error.
func filestream_error filestream_get_error(const filestream* stm);
