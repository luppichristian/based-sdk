// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "basic/keyword_defines.h"
#include "basic/primitive_types.h"
#include "memory/allocator.h"

// =========================================================================
c_begin;
// =========================================================================

typedef struct hash_map_slot {
  u64 key;
  void* value;
  u32 probe_dist;
  b32 occupied;
} hash_map_slot;

typedef struct hash_map {
  hash_map_slot* slots;
  sz count;
  sz cap;
  allocator alloc;
} hash_map;

typedef sz hash_map_iter;

// Lifecycle.
func hash_map hash_map_create(sz cap, allocator alloc);
func void hash_map_destroy(hash_map* map);
func void hash_map_clear(hash_map* map);

// Capacity and occupancy.
func sz hash_map_count(hash_map const* map);
func sz hash_map_capacity(hash_map const* map);
func f32 hash_map_load_factor(hash_map const* map);

// Capacity management.
func b32 hash_map_rehash(hash_map* map, sz new_cap);
func b32 hash_map_reserve(hash_map* map, sz min_cap);

// Key/value operations.
func b32 hash_map_set(hash_map* map, u64 key, void* value);
func void* hash_map_get(hash_map* map, u64 key);
func b32 hash_map_has(hash_map* map, u64 key);
func b32 hash_map_remove(hash_map* map, u64 key);

// Iteration over occupied slots.
func hash_map_iter hash_map_iter_begin(void);
func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter);

// =========================================================================
c_end;
// =========================================================================
