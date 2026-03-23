// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/allocator.h"
#include "memory/alloc_tracker.h"
#include "basic/assert.h"
#include "basic/profiler.h"

typedef union allocator_tracked_header {
  struct {
    sz requested_size;
  } info;
  void* force_align;
} allocator_tracked_header;

func allocator_tracked_header* allocator_get_tracked_header(void* ptr) {
  return ((allocator_tracked_header*)ptr) - 1;
}

func void* allocator_get_user_data(allocator* alloc) {
  if (alloc == NULL) {
    return NULL;
  }

  return alloc->user_data;
}

func void allocator_set_user_data(allocator* alloc, void* user_data) {
  profile_func_begin;
  if (alloc == NULL) {
    profile_func_end;
    return;
  }

  alloc->user_data = user_data;
  profile_func_end;
}

func alloc_tracker* allocator_get_tracker(allocator* alloc) {
  if (alloc == NULL) {
    return NULL;
  }

  return alloc->tracker;
}

func void allocator_set_tracker(allocator* alloc, alloc_tracker* tracker) {
  profile_func_begin;
  if (alloc == NULL) {
    profile_func_end;
    return;
  }

  alloc->tracker = tracker;
  profile_func_end;
}

func void* _allocator_alloc(allocator alloc, sz size, callsite site) {
  profile_func_begin;
  alloc_tracker_on_alloc_call(alloc.tracker);

  if (alloc.alloc_fn == NULL) {
    profile_func_end;
    return NULL;
  }
  assert(alloc.alloc_fn != NULL);
  sz request_size = size > 0 ? size : 1;
  if (alloc.tracker != NULL) {
    if (request_size > SZ_MAX - size_of(allocator_tracked_header)) {
      profile_func_end;
      return NULL;
    }

    sz tracked_size = request_size + size_of(allocator_tracked_header);
    allocator_tracked_header* header = (allocator_tracked_header*)alloc.alloc_fn(alloc.user_data, site, tracked_size);
    if (header == NULL) {
      profile_func_end;
      return NULL;
    }

    header->info.requested_size = request_size;
    alloc_tracker_on_alloc_success(alloc.tracker, site, (void*)(header + 1), request_size);
    profile_func_end;
    return (void*)(header + 1);
  }

  if (alloc.alloc_fn) {
    profile_func_end;
    return alloc.alloc_fn(alloc.user_data, site, request_size);
  }
  profile_func_end;
  return NULL;
}

func void* _allocator_calloc(allocator alloc, sz count, sz size, callsite site) {
  profile_func_begin;
  alloc_tracker_on_calloc_call(alloc.tracker);

  if (count == 0 || size == 0 || count > SZ_MAX / size) {
    profile_func_end;
    return NULL;
  }
  sz total_size = count * size;
  void* ptr = _allocator_alloc(alloc, total_size, site);
  if (ptr) {
    mem_zero(ptr, total_size);
  }
  profile_func_end;
  return ptr;
}

func void _allocator_dealloc(allocator alloc, void* ptr, callsite site) {
  profile_func_begin;
  alloc_tracker_on_free_call(alloc.tracker);

  if (ptr == NULL) {
    profile_func_end;
    return;
  }
  assert(alloc.dealloc_fn != NULL);
  if (alloc.dealloc_fn) {
    if (alloc.tracker != NULL) {
      allocator_tracked_header* header = allocator_get_tracked_header(ptr);
      sz request_size = header->info.requested_size;
      alloc.dealloc_fn(alloc.user_data, site, header);
      alloc_tracker_on_free_success(alloc.tracker, site, ptr, request_size);
    } else {
      alloc.dealloc_fn(alloc.user_data, site, ptr);
    }
  }
  profile_func_end;
}

func void* _allocator_realloc(allocator alloc, void* ptr, sz new_size, callsite site) {
  profile_func_begin;
  alloc_tracker_on_realloc_call(alloc.tracker);

  if (new_size == 0) {
    profile_func_end;
    return NULL;
  }
  if (ptr == NULL) {
    profile_func_end;
    return _allocator_alloc(alloc, new_size, site);
  }
  assert(alloc.realloc_fn != NULL);
  if (alloc.realloc_fn) {
    if (alloc.tracker != NULL) {
      allocator_tracked_header* old_header = allocator_get_tracked_header(ptr);
      sz old_size = old_header->info.requested_size;
      if (new_size > SZ_MAX - size_of(allocator_tracked_header)) {
        profile_func_end;
        return NULL;
      }

      sz tracked_size = new_size + size_of(allocator_tracked_header);
      allocator_tracked_header* new_header =
          (allocator_tracked_header*)alloc.realloc_fn(alloc.user_data, site, old_header, tracked_size);
      if (new_header == NULL) {
        profile_func_end;
        return NULL;
      }

      new_header->info.requested_size = new_size;
      alloc_tracker_on_realloc_success(alloc.tracker, site, ptr, (void*)(new_header + 1), old_size, new_size);
      profile_func_end;
      return (void*)(new_header + 1);
    }

    profile_func_end;
    return alloc.realloc_fn(alloc.user_data, site, ptr, new_size);
  }
  profile_func_end;
  return NULL;
}
