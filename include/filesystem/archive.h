// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../memory/buffer.h"
#include "path.h"

typedef struct archive_entry {
  path item_path;
  u8* data_ptr;
  sz data_size;
  sz data_capacity;
  b32 is_directory;
} archive_entry;

typedef struct archive {
  allocator* opt_alloc;
  archive_entry* entries;
  sz entry_count;
  sz entry_capacity;
} archive;

typedef struct archive_entry_info {
  path item_path;
  sz data_size;
  b32 is_directory;
} archive_entry_info;

// Returns 1 to continue iterating, 0 to stop.
typedef b32 archive_iterate_callback(const archive_entry_info* info, void* user_data);

// Creates an empty archive value. opt_alloc may be null when only write-once transient storage is needed.
func archive archive_create(allocator* opt_alloc);

// Releases every entry buffer owned by arc.
func void archive_destroy(archive* arc);

// Removes every entry from arc but keeps the archive value valid for reuse.
func void archive_clear(archive* arc);

// Returns the number of entries currently stored in arc.
func sz archive_count(const archive* arc);

// Returns 1 if src exists inside arc, 0 otherwise.
func b32 archive_exists(const archive* arc, const path* src);

// Loads a zip archive from disk into arc. Existing entries are discarded.
func b32 archive_load_file(archive* arc, const path* src);

// Saves arc to disk as a zip archive.
func b32 archive_save_file(const archive* arc, const path* dst);

// Adds or replaces a file entry by copying data into arc.
func b32 archive_write_all(archive* arc, const path* src, buffer data);

// Removes an entry from arc. Returns 1 when the entry existed, 0 otherwise.
func b32 archive_remove(archive* arc, const path* src);

// Reads a file entry into memory allocated from alloc.
// On success out_data receives an owned buffer. Caller frees it with allocator_dealloc(alloc, ptr, size).
func b32 archive_read_all(const archive* arc, const path* src, allocator* alloc, buffer* out_data);

// Copies a disk file into arc at archive_path.
func b32 archive_add_file(archive* arc, const path* archive_path, const path* disk_path);

// Iterates every entry in arc until callback returns 0 or the list ends.
func b32 archive_iterate(const archive* arc, archive_iterate_callback* callback, void* user_data);

// Queries metadata for one entry.
func b32 archive_get_entry_info(const archive* arc, const path* src, archive_entry_info* out_info);

// Returns a non-owning data span for an in-memory entry.
func b32 archive_get_entry_data(const archive* arc, const path* src, buffer* out_data);
