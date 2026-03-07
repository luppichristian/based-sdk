// MIT License
// Copyright (c) 2026 Christian Luppi

#include "memory/ring.h"
#include "basic/assert.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include <string.h>

// =========================================================================
// Create / Destroy
// =========================================================================

func ring ring_create(void* ptr, sz capacity, mutex opt_mutex) {
  ring rng;
  memset(&rng, 0, size_of(rng));
  rng.ptr = (u8*)ptr;
  rng.capacity = capacity;
  rng.opt_mutex = opt_mutex;
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_RING,
                                                     .object_ptr = &rng,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    memset(&rng, 0, size_of(rng));
  }
  thread_log_trace("ring_create: capacity=%zu", (size_t)capacity);
  return rng;
}

func ring ring_create_mutexed(void* ptr, sz capacity) {
  ring rng = ring_create(ptr, capacity, mutex_create());
  rng.mutex_owned = 1;
  return rng;
}

func ring ring_create_alloc(allocator parent_alloc, sz capacity, mutex opt_mutex) {
  ring rng;
  memset(&rng, 0, size_of(rng));
  rng.parent = parent_alloc;
  rng.capacity = capacity;
  rng.opt_mutex = opt_mutex;
  if (parent_alloc.alloc_fn) {
    rng.ptr = (u8*)_allocator_alloc(&parent_alloc, capacity, CALLSITE_HERE);
    rng.buf_owned = rng.ptr != NULL ? 1 : 0;
  }
  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_CREATE,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_RING,
                                                     .object_ptr = &rng,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    if (rng.buf_owned && rng.parent.alloc_fn) {
      _allocator_dealloc(&rng.parent, rng.ptr, rng.capacity, CALLSITE_HERE);
    }
    memset(&rng, 0, size_of(rng));
  }
  thread_log_trace("ring_create_alloc: capacity=%zu", (size_t)capacity);
  return rng;
}

func ring ring_create_alloc_mutexed(allocator parent_alloc, sz capacity) {
  ring rng = ring_create_alloc(parent_alloc, capacity, mutex_create());
  rng.mutex_owned = 1;
  return rng;
}

func void ring_destroy(ring* rng) {
  if (rng == NULL) {
    return;
  }
  assert(rng != NULL);

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_CORE_TYPE_OBJECT_LIFECYCLE;
  msg_core_fill_object_lifecycle(&lifecycle_msg, &(msg_core_object_lifecycle_data) {
                                                     .event_kind = MSG_CORE_OBJECT_EVENT_DESTROY,
                                                     .object_type = MSG_CORE_OBJECT_TYPE_RING,
                                                     .object_ptr = rng,
                                                 });
  if (!msg_post(&lifecycle_msg)) {
    return;
  }

  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  if (rng->buf_owned && rng->parent.alloc_fn) {
    _allocator_dealloc(&rng->parent, rng->ptr, rng->capacity, CALLSITE_HERE);
    rng->ptr = NULL;
    rng->buf_owned = 0;
  }

  mutex mtx_owned = rng->mutex_owned ? rng->opt_mutex : NULL;

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  if (mtx_owned) {
    mutex_destroy(mtx_owned);
  }

  rng->opt_mutex = NULL;
  rng->mutex_owned = 0;
  thread_log_trace("ring_destroy: rng=%p", (void*)rng);
}

// =========================================================================
// Capacity Queries
// =========================================================================

func sz ring_size(ring* rng) {
  if (rng == NULL) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }
  sz result = rng->count;
  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return result;
}

func sz ring_space(ring* rng) {
  if (rng == NULL) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }
  sz result = rng->capacity - rng->count;
  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return result;
}

// =========================================================================
// Internal Copy Helper
// =========================================================================

// Copies byte_count bytes starting at ring_offset (wrapping at rng->capacity)
// into dst. Does not update any ring state.
func void ring_copy_out(ring* rng, sz ring_offset, void* dst, sz byte_count) {
  sz to_end = rng->capacity - ring_offset;
  u8* out_bytes = (u8*)dst;
  if (byte_count <= to_end) {
    memcpy(out_bytes, rng->ptr + ring_offset, byte_count);
  } else {
    memcpy(out_bytes, rng->ptr + ring_offset, to_end);
    memcpy(out_bytes + to_end, rng->ptr, byte_count - to_end);
  }
}

// Copies byte_count bytes from src into the ring starting at ring_offset
// (wrapping at rng->capacity). Does not update any ring state.
func void ring_copy_in(ring* rng, sz ring_offset, void* src, sz byte_count) {
  sz to_end = rng->capacity - ring_offset;
  u8* src_bytes = (u8*)src;
  if (byte_count <= to_end) {
    memcpy(rng->ptr + ring_offset, src_bytes, byte_count);
  } else {
    memcpy(rng->ptr + ring_offset, src_bytes, to_end);
    memcpy(rng->ptr, src_bytes + to_end, byte_count - to_end);
  }
}

// =========================================================================
// I/O
// =========================================================================

func sz ring_write(ring* rng, void* data, sz size) {
  if (rng == NULL || data == NULL || size == 0 || rng->capacity == 0 || rng->ptr == NULL) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  sz write_sz = size < rng->capacity - rng->count ? size : rng->capacity - rng->count;
  if (write_sz > 0) {
    ring_copy_in(rng, rng->write_pos, data, write_sz);
    rng->write_pos = (rng->write_pos + write_sz) % rng->capacity;
    rng->count += write_sz;
  }

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return write_sz;
}

func sz ring_read(ring* rng, void* out, sz size) {
  if (rng == NULL || out == NULL || size == 0 || rng->capacity == 0 || rng->ptr == NULL) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  sz read_sz = size < rng->count ? size : rng->count;
  if (read_sz > 0) {
    ring_copy_out(rng, rng->read_pos, out, read_sz);
    rng->read_pos = (rng->read_pos + read_sz) % rng->capacity;
    rng->count -= read_sz;
  }

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return read_sz;
}

func sz ring_peek(ring* rng, void* out, sz size) {
  if (rng == NULL || out == NULL || size == 0 || rng->capacity == 0 || rng->ptr == NULL) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  sz peek_sz = size < rng->count ? size : rng->count;
  if (peek_sz > 0) {
    ring_copy_out(rng, rng->read_pos, out, peek_sz);
  }

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return peek_sz;
}

func sz ring_skip(ring* rng, sz size) {
  if (rng == NULL || size == 0 || rng->capacity == 0) {
    return 0;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  sz skip_sz = size < rng->count ? size : rng->count;
  if (skip_sz > 0) {
    rng->read_pos = (rng->read_pos + skip_sz) % rng->capacity;
    rng->count -= skip_sz;
  }

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
  return skip_sz;
}

// =========================================================================
// Lifecycle
// =========================================================================

func void ring_clear(ring* rng) {
  if (rng == NULL) {
    return;
  }
  if (rng->opt_mutex) {
    mutex_lock(rng->opt_mutex);
  }

  rng->read_pos = 0;
  rng->write_pos = 0;
  rng->count = 0;

  if (rng->opt_mutex) {
    mutex_unlock(rng->opt_mutex);
  }
}

