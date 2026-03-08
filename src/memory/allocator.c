// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/allocator.h"
#include "basic/assert.h"
#include "basic/profiler.h"
#include <string.h>

func void* _allocator_alloc(allocator* alloc, sz size, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (alloc == NULL || alloc->alloc_fn == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(alloc->alloc_fn != NULL);
  sz request_size = size > 0 ? size : 1;
  if (alloc->alloc_fn) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc->alloc_fn(alloc->user_data, site, request_size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return NULL;
}

func void* _allocator_calloc(allocator* alloc, sz count, sz size, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (alloc == NULL || count == 0 || size == 0 || count > SZ_MAX / size) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  sz total_size = count * size;
  void* ptr = _allocator_alloc(alloc, total_size, site);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return ptr;
}

func void _allocator_dealloc(allocator* alloc, void* ptr, sz size, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)size;
  if (alloc == NULL || ptr == NULL) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return;
  }
  assert(alloc->dealloc_fn != NULL);
  if (alloc->dealloc_fn) {
    alloc->dealloc_fn(alloc->user_data, site, ptr);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

func void* _allocator_realloc(allocator* alloc, void* ptr, sz old_size, sz new_size, callsite site) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (alloc == NULL || new_size == 0) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return NULL;
  }
  assert(alloc != NULL);
  if (alloc->realloc_fn) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return alloc->realloc_fn(alloc->user_data, site, ptr, old_size, new_size);
  }
  void* new_ptr = _allocator_alloc(alloc, new_size, site);
  if (new_ptr && ptr) {
    sz copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    _allocator_dealloc(alloc, ptr, old_size, site);
  }
  TracyCZoneEnd(__tracy_zone_ctx);
  return new_ptr;
}
