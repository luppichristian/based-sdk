// MIT License
// Copyright (c) 2026 Christian Luppi

#include "containers/hash_map.h"
#include "basic/assert.h"
#include "based_core.h"
#include "basic/profiler.h"
#include "memory/memops.h"
#include <string.h>

// =========================================================================
// Internal Helpers
// =========================================================================

// Insert key/value into a raw slot array without touching a hash_map struct.
// Used both by hash_map_set and during rehashing.
// Returns 1 if a new key was inserted, 0 if an existing key was updated.
func b32 hash_map_raw_insert(
    hash_map_slot* slots,
    sz cap,
    u64 key,
    void* value) {
  profile_func_begin;
  sz pos = (sz)(hash_u64(key) & (u64)(cap - 1));
  u32 dist = 0;

  hash_map_slot incoming;
  incoming.key = key;
  incoming.value = value;
  incoming.probe_dist = 0;
  incoming.occupied = 1;

  safe_for (;;) {
    hash_map_slot* slot = &slots[pos];

    if (!slot->occupied) {
      *slot = incoming;
      profile_func_end;
      return true;
    }

    if (slot->key == incoming.key) {
      slot->value = incoming.value;
      profile_func_end;
      return false;
    }

    // Robin Hood: steal the slot from the luckier resident.
    if (slot->probe_dist < dist) {
      hash_map_slot tmp = *slot;
      *slot = incoming;
      incoming = tmp;
      dist = incoming.probe_dist;
    }

    dist++;
    incoming.probe_dist = dist;
    pos = (pos + 1) & (cap - 1);
  }
}

// Grow the backing array to new_cap slots and re-insert all entries.
// Returns 1 on success, 0 on allocation failure.
func b32 hash_map_rehash(hash_map* map, sz new_cap) {
  profile_func_begin;
  if (map == NULL || new_cap < 2) {
    profile_func_end;
    return false;
  }
  assert(map != NULL);
  hash_map_slot* new_slots =
      (hash_map_slot*)allocator_calloc(map->alloc, new_cap, size_of(hash_map_slot));
  if (!new_slots) {
    profile_func_end;
    return false;
  }

  safe_for (sz idx = 0; idx < map->cap; idx++) {
    hash_map_slot* slot = &map->slots[idx];
    if (slot->occupied) {
      hash_map_raw_insert(new_slots, new_cap, slot->key, slot->value);
    }
  }

  if (map->slots) {
    allocator_dealloc(map->alloc, map->slots);
  }
  map->slots = new_slots;
  map->cap = new_cap;
  profile_func_end;
  return true;
}

func b32 hash_map_reserve(hash_map* map, sz min_cap) {
  profile_func_begin;
  if (map == NULL) {
    profile_func_end;
    return false;
  }

  sz target_cap = map->cap > 0 ? map->cap : 16;
  safe_while (target_cap < min_cap) {
    target_cap *= 2;
  }

  if (target_cap <= map->cap) {
    profile_func_end;
    return true;
  }

  b32 result = hash_map_rehash(map, target_cap);
  profile_func_end;
  return result;
}

// Find the slot for key in map->slots; returns a pointer to the slot or NULL.
func hash_map_slot* hash_map_find_slot(hash_map* map, u64 key) {
  profile_func_begin;
  if (map == NULL || map->slots == NULL || map->cap == 0) {
    profile_func_end;
    return NULL;
  }
  assert(map->slots != NULL);
  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  safe_for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      profile_func_end;
      return NULL;
    }
    if (slot->key == key) {
      profile_func_end;
      return slot;
    }
    pos = (pos + 1) & (map->cap - 1);
    dist++;
  }
}

// =========================================================================
// Lifecycle
// =========================================================================

func hash_map hash_map_create(sz cap, allocator alloc) {
  profile_func_begin;
  hash_map map;
  mem_zero(&map, size_of(map));
  map.alloc = alloc;
  if (map.alloc.alloc_fn == NULL || map.alloc.dealloc_fn == NULL) {
    map.alloc = thread_get_allocator();
  }
  if (map.alloc.alloc_fn == NULL || map.alloc.dealloc_fn == NULL) {
    map.alloc = global_get_allocator();
  }

  sz actual = 16;
  safe_while (actual < cap) {
    actual *= 2;
  }
  map.cap = actual;
  if (map.alloc.alloc_fn != NULL && map.alloc.dealloc_fn != NULL) {
    map.slots = (hash_map_slot*)allocator_calloc(map.alloc, actual, size_of(hash_map_slot));
  }
  profile_func_end;
  return map;
}

func void hash_map_destroy(hash_map* map) {
  profile_func_begin;
  if (map == NULL) {
    profile_func_end;
    return;
  }
  if (map->slots) {
    allocator_dealloc(map->alloc, map->slots);
    map->slots = NULL;
  }
  map->count = 0;
  map->cap = 0;
  profile_func_end;
}

func void hash_map_clear(hash_map* map) {
  profile_func_begin;
  if (map == NULL) {
    profile_func_end;
    return;
  }
  if (map->slots) {
    mem_zero(map->slots, map->cap * size_of(hash_map_slot));
  }
  map->count = 0;
  profile_func_end;
}

// =========================================================================
// Capacity and Occupancy
// =========================================================================

func sz hash_map_count(hash_map const* map) {
  if (map == NULL) {
    return 0;
  }
  return map->count;
}

func sz hash_map_capacity(hash_map const* map) {
  if (map == NULL) {
    return 0;
  }
  return map->cap;
}

func f32 hash_map_load_factor(hash_map const* map) {
  if (map == NULL || map->cap == 0) {
    return 0.0F;
  }
  f32 result = (f32)map->count / (f32)map->cap;
  return result;
}

// =========================================================================
// Key/Value Operations
// =========================================================================

func b32 hash_map_set(hash_map* map, u64 key, void* value) {
  profile_func_begin;
  if (map == NULL || !map->slots) {
    profile_func_end;
    return false;
  }
  assert(map->cap > 0);

  // Rehash before inserting once load reaches 75%.
  if (map->count >= map->cap - (map->cap / 4)) {
    if (!hash_map_reserve(map, map->cap * 2)) {
      profile_func_end;
      return false;
    }
  }

  b32 inserted = hash_map_raw_insert(map->slots, map->cap, key, value);
  if (inserted) {
    map->count++;
  }
  profile_func_end;
  return true;
}

func void* hash_map_get(hash_map* map, u64 key) {
  profile_func_begin;
  if (map == NULL || !map->slots || map->count == 0) {
    profile_func_end;
    return NULL;
  }
  hash_map_slot* slot = hash_map_find_slot(map, key);
  profile_func_end;
  return slot ? slot->value : NULL;
}

func b32 hash_map_has(hash_map* map, u64 key) {
  if (map == NULL || !map->slots || map->count == 0) {
    return false;
  }
  return hash_map_find_slot(map, key) != NULL;
}

func b32 hash_map_remove(hash_map* map, u64 key) {
  profile_func_begin;
  if (map == NULL || !map->slots || map->count == 0) {
    profile_func_end;
    return false;
  }

  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  safe_for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      profile_func_end;
      return false;
    }
    if (slot->key == key) {
      // Backward-shift deletion: pull subsequent entries one step closer to
      // their ideal positions, maintaining the Robin Hood invariant without
      // needing tombstones.
      sz cur = pos;
      safe_for (;;) {
        sz nxt = (cur + 1) & (map->cap - 1);
        hash_map_slot* next_slot = &map->slots[nxt];
        if (!next_slot->occupied || next_slot->probe_dist == 0) {
          mem_zero(&map->slots[cur], size_of(hash_map_slot));
          break;
        }
        map->slots[cur] = *next_slot;
        map->slots[cur].probe_dist--;
        cur = nxt;
      }
      map->count--;
      profile_func_end;
      return true;
    }
    pos = (pos + 1) & (map->cap - 1);
    dist++;
  }
}

func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter) {
  profile_func_begin;
  if (map == NULL || iter == NULL || !map->slots) {
    profile_func_end;
    return NULL;
  }
  safe_while (*iter < map->cap) {
    hash_map_slot* slot = &map->slots[*iter];
    (*iter)++;
    if (slot->occupied) {
      profile_func_end;
      return slot;
    }
  }
  profile_func_end;
  return NULL;
}
