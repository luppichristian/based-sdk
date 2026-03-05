// MIT License
// Copyright (c) 2026 Christian Luppi

#include "containers/hash_map.h"
#include "basic/assert.h"
#include "based.h"
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
  sz pos = (sz)(lm2_hash_u64(key) & (u64)(cap - 1));
  u32 dist = 0;

  hash_map_slot incoming;
  incoming.key = key;
  incoming.value = value;
  incoming.probe_dist = 0;
  incoming.occupied = 1;

  for (;;) {
    hash_map_slot* slot = &slots[pos];

    if (!slot->occupied) {
      *slot = incoming;
      return 1;
    }

    if (slot->key == incoming.key) {
      slot->value = incoming.value;
      return 0;
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
  if (map == NULL || new_cap < 2) {
    return 0;
  }
  assert(map != NULL);
  hash_map_slot* new_slots =
      (hash_map_slot*)allocator_calloc(&map->alloc, new_cap, size_of(hash_map_slot));
  if (!new_slots) {
    return 0;
  }

  for (sz idx = 0; idx < map->cap; idx++) {
    hash_map_slot* slot = &map->slots[idx];
    if (slot->occupied) {
      hash_map_raw_insert(new_slots, new_cap, slot->key, slot->value);
    }
  }

  if (map->slots) {
    allocator_dealloc(&map->alloc, map->slots, map->cap * size_of(hash_map_slot));
  }
  map->slots = new_slots;
  map->cap = new_cap;
  return 1;
}

// Find the slot for key in map->slots; returns a pointer to the slot or NULL.
func hash_map_slot* hash_map_find_slot(hash_map* map, u64 key) {
  if (map == NULL || map->slots == NULL || map->cap == 0) {
    return NULL;
  }
  assert(map->slots != NULL);
  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      return NULL;
    }
    if (slot->key == key) {
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
  hash_map map;
  memset(&map, 0, size_of(map));
  map.alloc = alloc;
  assert(alloc.alloc_fn != NULL);
  assert(alloc.dealloc_fn != NULL);

  sz actual = 16;
  while (actual < cap) {
    actual *= 2;
  }
  map.cap = actual;
  map.slots = (hash_map_slot*)allocator_calloc(&map.alloc, actual, size_of(hash_map_slot));
  return map;
}

func void hash_map_destroy(hash_map* map) {
  if (map == NULL) {
    return;
  }
  if (map->slots) {
    allocator_dealloc(&map->alloc, map->slots, map->cap * size_of(hash_map_slot));
    map->slots = NULL;
  }
  map->count = 0;
  map->cap = 0;
}

func void hash_map_clear(hash_map* map) {
  if (map == NULL) {
    return;
  }
  if (map->slots) {
    memset(map->slots, 0, map->cap * size_of(hash_map_slot));
  }
  map->count = 0;
}

// =========================================================================
// Operations
// =========================================================================

func b32 hash_map_set(hash_map* map, u64 key, void* value) {
  if (map == NULL || !map->slots) {
    return 0;
  }
  assert(map->cap > 0);

  // Rehash before inserting once load reaches 75%.
  if (map->count >= map->cap - (map->cap / 4)) {
    if (!hash_map_rehash(map, map->cap * 2)) {
      return 0;
    }
  }

  b32 inserted = hash_map_raw_insert(map->slots, map->cap, key, value);
  if (inserted) {
    map->count++;
  }
  return 1;
}

func void* hash_map_get(hash_map* map, u64 key) {
  if (map == NULL || !map->slots || map->count == 0) {
    return NULL;
  }
  hash_map_slot* slot = hash_map_find_slot(map, key);
  return slot ? slot->value : NULL;
}

func b32 hash_map_has(hash_map* map, u64 key) {
  if (map == NULL || !map->slots || map->count == 0) {
    return 0;
  }
  return hash_map_find_slot(map, key) != NULL;
}

func b32 hash_map_remove(hash_map* map, u64 key) {
  if (map == NULL || !map->slots || map->count == 0) {
    return 0;
  }

  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      return 0;
    }
    if (slot->key == key) {
      // Backward-shift deletion: pull subsequent entries one step closer to
      // their ideal positions, maintaining the Robin Hood invariant without
      // needing tombstones.
      sz cur = pos;
      for (;;) {
        sz nxt = (cur + 1) & (map->cap - 1);
        hash_map_slot* next_slot = &map->slots[nxt];
        if (!next_slot->occupied || next_slot->probe_dist == 0) {
          memset(&map->slots[cur], 0, size_of(hash_map_slot));
          break;
        }
        map->slots[cur] = *next_slot;
        map->slots[cur].probe_dist--;
        cur = nxt;
      }
      map->count--;
      return 1;
    }
    pos = (pos + 1) & (map->cap - 1);
    dist++;
  }
}

// =========================================================================
// Iteration
// =========================================================================

func hash_map_iter hash_map_iter_begin(void) {
  return 0;
}

func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter) {
  if (map == NULL || iter == NULL || !map->slots) {
    return NULL;
  }
  while (*iter < map->cap) {
    hash_map_slot* slot = &map->slots[*iter];
    (*iter)++;
    if (slot->occupied) {
      return slot;
    }
  }
  return NULL;
}
