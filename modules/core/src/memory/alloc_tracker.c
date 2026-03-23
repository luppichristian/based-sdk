// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/alloc_tracker.h"
#include "basic/profiler.h"
#include "basic/safe.h"
#include "memory/memops.h"

func void alloc_tracker_sub_clamped(atomic_u64* atom, u64 delta) {
  profile_func_begin;
  if (atom == NULL) {
    profile_func_end;
    return;
  }

  u64 current = atomic_u64_get(atom);
  safe_while (true) {
    u64 next = current > delta ? current - delta : 0;
    if (atomic_u64_cmpex(atom, &current, next)) {
      profile_func_end;
      return;
    }
  }
}

func void alloc_tracker_update_peak_allocated_bytes(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  u64 candidate = atomic_u64_get(&tracker->live_allocated_bytes);
  u64 current_peak = atomic_u64_get(&tracker->peak_live_allocated_bytes);
  safe_while (candidate > current_peak) {
    u64 expected = current_peak;
    if (atomic_u64_cmpex(&tracker->peak_live_allocated_bytes, &expected, candidate)) {
      profile_func_end;
      return;
    }
    current_peak = expected;
  }

  profile_func_end;
}

func alloc_tracker alloc_tracker_create(void) {
  profile_func_begin;
  alloc_tracker tracker;
  mem_zero(&tracker, size_of(tracker));
  profile_func_end;
  return tracker;
}

func void* alloc_tracker_get_user_data(alloc_tracker* tracker) {
  if (tracker == NULL) {
    return NULL;
  }

  return tracker->user_data;
}

func void alloc_tracker_set_user_data(alloc_tracker* tracker, void* user_data) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  tracker->user_data = user_data;
  profile_func_end;
}

func void alloc_tracker_reset(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  mem_zero(tracker, size_of(*tracker));
  profile_func_end;
}

func alloc_tracker_stats alloc_tracker_get_stats(alloc_tracker* tracker) {
  profile_func_begin;
  alloc_tracker_stats stats;
  mem_zero(&stats, size_of(stats));

  if (tracker == NULL) {
    profile_func_end;
    return stats;
  }

  stats.alloc_calls = atomic_u64_get(&tracker->alloc_calls);
  stats.calloc_calls = atomic_u64_get(&tracker->calloc_calls);
  stats.realloc_calls = atomic_u64_get(&tracker->realloc_calls);
  stats.free_calls = atomic_u64_get(&tracker->free_calls);

  stats.live_allocations = atomic_u64_get(&tracker->live_allocations);
  stats.live_allocated_bytes = atomic_u64_get(&tracker->live_allocated_bytes);
  stats.peak_live_allocated_bytes = atomic_u64_get(&tracker->peak_live_allocated_bytes);
  stats.total_allocated_bytes = atomic_u64_get(&tracker->total_allocated_bytes);
  stats.total_freed_bytes = atomic_u64_get(&tracker->total_freed_bytes);
  profile_func_end;
  return stats;
}

func void alloc_tracker_on_alloc_call(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker != NULL) {
    atomic_u64_add(&tracker->alloc_calls, 1);
  }
  profile_func_end;
}

func void alloc_tracker_on_calloc_call(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker != NULL) {
    atomic_u64_add(&tracker->calloc_calls, 1);
  }
  profile_func_end;
}

func void alloc_tracker_on_realloc_call(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker != NULL) {
    atomic_u64_add(&tracker->realloc_calls, 1);
  }
  profile_func_end;
}

func void alloc_tracker_on_free_call(alloc_tracker* tracker) {
  profile_func_begin;
  if (tracker != NULL) {
    atomic_u64_add(&tracker->free_calls, 1);
  }
  profile_func_end;
}

func void alloc_tracker_on_alloc_success(alloc_tracker* tracker, callsite site, void* ptr, sz size) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  u64 delta = (u64)size;
  atomic_u64_add(&tracker->live_allocations, 1);
  atomic_u64_add(&tracker->live_allocated_bytes, delta);
  atomic_u64_add(&tracker->total_allocated_bytes, delta);
  alloc_tracker_update_peak_allocated_bytes(tracker);
  if (tracker->alloc_fn != NULL) {
    tracker->alloc_fn(tracker->user_data, site, ptr, size);
  }
  profile_func_end;
}

func void alloc_tracker_on_free_success(alloc_tracker* tracker, callsite site, void* ptr, sz size) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  u64 delta = (u64)size;
  alloc_tracker_sub_clamped(&tracker->live_allocations, 1);
  alloc_tracker_sub_clamped(&tracker->live_allocated_bytes, delta);
  atomic_u64_add(&tracker->total_freed_bytes, delta);
  if (tracker->dealloc_fn != NULL) {
    tracker->dealloc_fn(tracker->user_data, site, ptr, size);
  }
  profile_func_end;
}

func void alloc_tracker_on_realloc_success(
    alloc_tracker* tracker,
    callsite site,
    void* old_ptr,
    void* new_ptr,
    sz old_size,
    sz new_size) {
  profile_func_begin;
  if (tracker == NULL) {
    profile_func_end;
    return;
  }

  u64 old_bytes = (u64)old_size;
  u64 new_bytes = (u64)new_size;
  if (new_bytes > old_bytes) {
    u64 delta = new_bytes - old_bytes;
    atomic_u64_add(&tracker->live_allocated_bytes, delta);
    atomic_u64_add(&tracker->total_allocated_bytes, delta);
    alloc_tracker_update_peak_allocated_bytes(tracker);
  } else if (old_bytes > new_bytes) {
    u64 delta = old_bytes - new_bytes;
    alloc_tracker_sub_clamped(&tracker->live_allocated_bytes, delta);
    atomic_u64_add(&tracker->total_freed_bytes, delta);
  }

  if (tracker->realloc_fn != NULL) {
    tracker->realloc_fn(tracker->user_data, site, old_ptr, new_ptr, old_size, new_size);
  }

  profile_func_end;
}
