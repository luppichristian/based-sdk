// MIT License
// Copyright (c) 2026 Christian Luppi

#include "containers/hash_map.h"
#include "basic/assert.h"
#include "based.h"
#include "basic/profiler.h"
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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
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
      TracyCZoneEnd(__tracy_zone_ctx);
      return 1;
    }

    if (slot->key == incoming.key) {
      slot->value = incoming.value;
      TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || new_cap < 2) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(map != NULL);
  hash_map_slot* new_slots =
      (hash_map_slot*)allocator_calloc(&map->alloc, new_cap, size_of(hash_map_slot));
  if (!new_slots) {
    TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func b32 hash_map_reserve(hash_map* map, sz min_cap) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  sz target_cap = map->cap > 0 ? map->cap : 16;
  while (target_cap < min_cap) {
    target_cap *= 2;
  }

  if (target_cap <= map->cap) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 1;
  }

  b32 result = hash_map_rehash(map, target_cap);
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

// Find the slot for key in map->slots; returns a pointer to the slot or NULL.
func hash_map_slot* hash_map_find_slot(hash_map* map, u64 key) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || map->slots == NULL || map->cap == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(map->slots != NULL);
  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return NULL;
    }
    if (slot->key == key) {
      TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
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
  TracyCZoneEnd(__tracy_zone_ctx);
  return map;
}

func void hash_map_destroy(hash_map* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if (map->slots) {
    allocator_dealloc(&map->alloc, map->slots, map->cap * size_of(hash_map_slot));
    map->slots = NULL;
  }
  map->count = 0;
  map->cap = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void hash_map_clear(hash_map* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  if (map->slots) {
    memset(map->slots, 0, map->cap * size_of(hash_map_slot));
  }
  map->count = 0;
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Capacity and Occupancy
// =========================================================================

func sz hash_map_count(hash_map const* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return map->count;
}

func sz hash_map_capacity(hash_map const* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return map->cap;
}

func f32 hash_map_load_factor(hash_map const* map) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || map->cap == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0.0f;
  }
  f32 result = (f32)map->count / (f32)map->cap;
  TracyCZoneEnd(__tracy_zone_ctx);
  return result;
}

// =========================================================================
// Key/Value Operations
// =========================================================================

func b32 hash_map_set(hash_map* map, u64 key, void* value) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || !map->slots) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  assert(map->cap > 0);

  // Rehash before inserting once load reaches 75%.
  if (map->count >= map->cap - (map->cap / 4)) {
    if (!hash_map_reserve(map, map->cap * 2)) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return 0;
    }
  }

  b32 inserted = hash_map_raw_insert(map->slots, map->cap, key, value);
  if (inserted) {
    map->count++;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return 1;
}

func void* hash_map_get(hash_map* map, u64 key) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || !map->slots || map->count == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  hash_map_slot* slot = hash_map_find_slot(map, key);
  TracyCZoneEnd(__tracy_zone_ctx);
  return slot ? slot->value : NULL;
}

func b32 hash_map_has(hash_map* map, u64 key) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || !map->slots || map->count == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return hash_map_find_slot(map, key) != NULL;
}

func b32 hash_map_remove(hash_map* map, u64 key) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || !map->slots || map->count == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return 0;
  }

  sz pos = (sz)(hash_u64(key) & (u64)(map->cap - 1));
  u32 dist = 0;

  for (;;) {
    hash_map_slot* slot = &map->slots[pos];
    if (!slot->occupied || slot->probe_dist < dist) {
      TracyCZoneEnd(__tracy_zone_ctx);
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
      TracyCZoneEnd(__tracy_zone_ctx);
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
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  TracyCZoneEnd(__tracy_zone_ctx);
  return 0;
}

func hash_map_slot* hash_map_next(hash_map* map, hash_map_iter* iter) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (map == NULL || iter == NULL || !map->slots) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  while (*iter < map->cap) {
    hash_map_slot* slot = &map->slots[*iter];
    (*iter)++;
    if (slot->occupied) {
      TracyCZoneEnd(__tracy_zone_ctx);
      return slot;
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}
