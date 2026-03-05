// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/global_ctx.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "memory/vmem.h"
#include "threads/atomics.h"

#include <string.h>

global_var global_ctx process_global_ctx = {0};
global_var atomic_i32 process_global_ctx_init = {0};
thread_local global_var b8 global_user_data_access[CTX_USER_DATA_COUNT] = {0};

func void global_user_data_access_set_all_local(b8 has_access) {
  for (sz index = 0; index < CTX_USER_DATA_COUNT; index += 1) {
    global_user_data_access[index] = has_access ? true : false;
  }
}

func b32 global_ctx_init(allocator main_allocator) {
  if (!main_allocator.alloc_fn) {
    return false;
  }
  assert(main_allocator.dealloc_fn != NULL);

  i32 state = atomic_i32_get(&process_global_ctx_init);
  if (state == 2) {
    return true;
  }

  i32 expected = 0;
  if (atomic_i32_cmpex(&process_global_ctx_init, &expected, 1)) {
    msg lifecycle_msg = {0};
    lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
    lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_CREATE;
    lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_GLOBAL_CTX;
    lifecycle_msg.object_lifecycle.object_ptr = &process_global_ctx;
    if (!msg_post(&lifecycle_msg)) {
      atomic_fence_release();
      atomic_i32_set(&process_global_ctx_init, 0);
      return false;
    }

    memset(&process_global_ctx, 0, size_of(process_global_ctx));
    process_global_ctx.mutex_handle = mutex_create();
    if (!process_global_ctx.mutex_handle || !ctx_init(&process_global_ctx.shared_ctx, main_allocator, true)) {
      if (process_global_ctx.mutex_handle) {
        mutex_destroy(process_global_ctx.mutex_handle);
      }
      memset(&process_global_ctx, 0, size_of(process_global_ctx));
      atomic_fence_release();
      atomic_i32_set(&process_global_ctx_init, 0);
      return false;
    }

    // Initializer thread (main thread via entry_init) starts with full userdata access.
    global_user_data_access_set_all_local(true);
    process_global_ctx.is_init = true;
    atomic_fence_release();
    atomic_i32_set(&process_global_ctx_init, 2);
    return true;
  }

  while (atomic_i32_get(&process_global_ctx_init) == 1) {
    atomic_pause();
  }
  atomic_fence_acquire();
  return atomic_i32_get(&process_global_ctx_init) == 2;
}

func void global_ctx_quit(void) {
  i32 expected = 2;
  if (!atomic_i32_cmpex(&process_global_ctx_init, &expected, 1)) {
    return;
  }

  msg lifecycle_msg = {0};
  lifecycle_msg.type = MSG_TYPE_OBJECT_LIFECYCLE;
  lifecycle_msg.object_lifecycle.event_kind = (u32)MSG_OBJECT_EVENT_DESTROY;
  lifecycle_msg.object_lifecycle.object_type = (u32)MSG_OBJECT_TYPE_GLOBAL_CTX;
  lifecycle_msg.object_lifecycle.object_ptr = &process_global_ctx;
  if (!msg_post(&lifecycle_msg)) {
    atomic_fence_release();
    atomic_i32_set(&process_global_ctx_init, 2);
    return;
  }

  mutex wrapper_mutex = process_global_ctx.mutex_handle;
  if (wrapper_mutex) {
    mutex_lock(wrapper_mutex);
  }

  ctx_quit(&process_global_ctx.shared_ctx);
  process_global_ctx.is_init = false;

  if (wrapper_mutex) {
    mutex_unlock(wrapper_mutex);
    mutex_destroy(wrapper_mutex);
  }

  memset(&process_global_ctx, 0, size_of(process_global_ctx));
  global_user_data_access_set_all_local(false);
  atomic_fence_release();
  atomic_i32_set(&process_global_ctx_init, 0);
}

func b32 global_ctx_is_init(void) {
  return atomic_i32_get(&process_global_ctx_init) == 2;
}

func global_ctx* global_ctx_get(void) {
  if (!global_ctx_is_init() && !global_ctx_init(vmem_get_allocator())) {
    return NULL;
  }
  return &process_global_ctx;
}

func ctx* global_ctx_get_shared(void) {
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper) {
    return NULL;
  }
  return &wrapper->shared_ctx;
}

func log_state* global_get_log_state(void) {
  return ctx_get_log_state(global_ctx_get_shared());
}

func void global_ctx_lock(void) {
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }
}

func void global_ctx_unlock(void) {
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }
}

func allocator global_get_allocator(void) {
  return ctx_get_allocator(global_ctx_get_shared());
}

func arena* global_get_perm_arena(void) {
  return ctx_get_perm_arena(global_ctx_get_shared());
}

func arena* global_get_temp_arena(void) {
  return ctx_get_temp_arena(global_ctx_get_shared());
}

func heap* global_get_perm_heap(void) {
  return ctx_get_perm_heap(global_ctx_get_shared());
}

func heap* global_get_temp_heap(void) {
  return ctx_get_temp_heap(global_ctx_get_shared());
}

func void* global_get_user_data(ctx_user_data_idx index) {
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper || index >= CTX_USER_DATA_COUNT) {
    return NULL;
  }
  assert(index < CTX_USER_DATA_COUNT);

  if (wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }

  b32 has_access = global_user_data_access[index] != 0;
  void* user_data = has_access ? wrapper->shared_ctx.user_data[index] : NULL;

  if (wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }

  return user_data;
}

func b32 global_set_user_data(ctx_user_data_idx index, void* user_data) {
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper || index >= CTX_USER_DATA_COUNT) {
    return false;
  }
  assert(index < CTX_USER_DATA_COUNT);

  if (wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }

  b32 has_access = global_user_data_access[index] != 0;
  if (has_access) {
    wrapper->shared_ctx.user_data[index] = user_data;
  }

  if (wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }

  return has_access;
}

func b32 global_has_user_data_access(ctx_user_data_idx index) {
  if (!global_ctx_get() || index >= CTX_USER_DATA_COUNT) {
    return false;
  }
  return global_user_data_access[index] != 0;
}

func b32 global_set_user_data_access(ctx_user_data_idx index, b8 has_access) {
  if (!global_ctx_get() || index >= CTX_USER_DATA_COUNT) {
    return false;
  }
  global_user_data_access[index] = has_access ? true : false;
  return true;
}

func void global_set_user_data_access_all(b8 has_access) {
  if (!global_ctx_get()) {
    return;
  }
  global_user_data_access_set_all_local(has_access);
}

func void global_clear_temp(void) {
  ctx_clear_temp(global_ctx_get_shared());
}

func void global_log_set_level(log_level level) {
  log_state_set_level(global_get_log_state(), level);
}

func void global_log_begin_frame(void) {
  log_state_begin_frame(global_get_log_state());
}

func log_frame* global_log_end_frame(u32 severity_mask) {
  return log_state_end_frame(global_get_log_state(), severity_mask);
}
