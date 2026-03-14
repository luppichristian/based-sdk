// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/global_ctx.h"
#include "basic/assert.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "threads/atomics.h"
#include "basic/profiler.h"
#include "memory/memops.h"

#include <string.h>
#include "basic/safe.h"

global_var global_ctx process_global_ctx = {0};
global_var atomic_i32 process_global_ctx_init = {0};
thread_local global_var b8 global_user_data_access[CTX_USER_DATA_COUNT] = {0};

func void global_user_data_access_set_all_local(b8 has_access) {
  profile_func_begin;
  safe_for (sz idx = 0; idx < CTX_USER_DATA_COUNT; idx += 1) {
    global_user_data_access[idx] = has_access ? true : false;
  }
  profile_func_end;
}

func b32 global_ctx_init(ctx_setup setup) {
  profile_func_begin;
  i32 state = atomic_i32_get(&process_global_ctx_init);
  if (state != false) {
    profile_func_end;
    return true;
  }

  msg_core_global_ctx_data ctx_data = {
      .event_kind = MSG_CORE_GLOBAL_CTX_EVENT_INIT,
      .global_ctx_ptr = &process_global_ctx,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_global_ctx(&lifecycle_msg, &ctx_data);
  if (!msg_post(&lifecycle_msg)) {
    atomic_i32_set(&process_global_ctx_init, false);
    profile_func_end;
    return true;
  }

  mem_zero(&process_global_ctx, size_of(process_global_ctx));
  process_global_ctx.mutex_handle = mutex_create();
  ctx_setup global_setup = setup;
  global_setup.allocator_mutex = process_global_ctx.mutex_handle;
  global_setup.use_log_mutex = true;
  if (!process_global_ctx.mutex_handle ||
      !ctx_init(&process_global_ctx.shared_ctx, global_setup)) {
    if (process_global_ctx.mutex_handle) {
      mutex_destroy(process_global_ctx.mutex_handle);
    }
    mem_zero(&process_global_ctx, size_of(process_global_ctx));
    profile_func_end;
    return false;
  }

  // Initializer thread (main thread via entry_init) starts with full userdata access.
  global_user_data_access_set_all_local(true);
  process_global_ctx.is_init = true;
  atomic_i32_set(&process_global_ctx_init, true);
  profile_func_end;
  return true;
}

func b32 global_ctx_quit(void) {
  profile_func_begin;
  i32 expected = 1;
  if (!atomic_i32_cmpex(&process_global_ctx_init, &expected, false)) {
    profile_func_end;
    return false;
  }

  msg_core_global_ctx_data ctx_data = {
      .event_kind = MSG_CORE_GLOBAL_CTX_EVENT_QUIT,
      .global_ctx_ptr = &process_global_ctx,
  };

  msg lifecycle_msg = {0};
  msg_core_fill_global_ctx(&lifecycle_msg, &ctx_data);
  if (!msg_post(&lifecycle_msg)) {
    atomic_i32_set(&process_global_ctx_init, true);
    profile_func_end;
    return false;
  }

  mutex wrapper_mutex = process_global_ctx.mutex_handle;
  ctx_quit(&process_global_ctx.shared_ctx);
  process_global_ctx.is_init = false;

  if (wrapper_mutex) {
    mutex_destroy(wrapper_mutex);
  }

  mem_zero(&process_global_ctx, size_of(process_global_ctx));
  global_user_data_access_set_all_local(false);
  atomic_i32_set(&process_global_ctx_init, false);
  profile_func_end;
  return true;
}

func b32 global_ctx_is_init(void) {
  return atomic_i32_get(&process_global_ctx_init) == 1;
}

func global_ctx* global_ctx_get(void) {
  if (!global_ctx_is_init()) {
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
  profile_func_begin;
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }
  profile_func_end;
}

func void global_ctx_unlock(void) {
  profile_func_begin;
  global_ctx* wrapper = global_ctx_get();
  if (wrapper && wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }
  profile_func_end;
}

func allocator global_get_allocator(void) {
  return ctx_get_allocator(global_ctx_get_shared());
}

func ctx_setup global_get_setup(void) {
  return ctx_get_setup(global_ctx_get_shared());
}

func allocator global_get_main_allocator(void) {
  profile_func_begin;
  allocator alloc = {0};
  i32 state = atomic_i32_get(&process_global_ctx_init);
  if (state == 0) {
    profile_func_end;
    return alloc;
  }

  profile_func_end;
  return process_global_ctx.shared_ctx.setup.main_allocator;
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

func void* global_get_user_data(ctx_user_data_idx idx) {
  profile_func_begin;
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper || idx >= CTX_USER_DATA_COUNT) {
    profile_func_end;
    return NULL;
  }
  assert(idx < CTX_USER_DATA_COUNT);

  if (wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }

  b32 has_access = global_user_data_access[idx] != false;
  void* user_data = has_access ? wrapper->shared_ctx.user_data[idx] : NULL;

  if (wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }

  profile_func_end;
  return user_data;
}

func b32 global_set_user_data(ctx_user_data_idx idx, void* user_data) {
  profile_func_begin;
  global_ctx* wrapper = global_ctx_get();
  if (!wrapper || idx >= CTX_USER_DATA_COUNT) {
    profile_func_end;
    return false;
  }
  assert(idx < CTX_USER_DATA_COUNT);

  if (wrapper->mutex_handle) {
    mutex_lock(wrapper->mutex_handle);
  }

  b32 has_access = global_user_data_access[idx] != false;
  if (has_access) {
    wrapper->shared_ctx.user_data[idx] = user_data;
  }

  if (wrapper->mutex_handle) {
    mutex_unlock(wrapper->mutex_handle);
  }

  profile_func_end;
  return has_access;
}

func b32 global_has_user_data_access(ctx_user_data_idx idx) {
  if (!global_ctx_get() || idx >= CTX_USER_DATA_COUNT) {
    return false;
  }
  return global_user_data_access[idx] != false;
}

func b32 global_set_user_data_access(ctx_user_data_idx idx, b8 has_access) {
  profile_func_begin;
  if (!global_ctx_get() || idx >= CTX_USER_DATA_COUNT) {
    profile_func_end;
    return false;
  }
  global_user_data_access[idx] = has_access ? true : false;
  profile_func_end;
  return true;
}

func void global_set_user_data_access_all(b8 has_access) {
  profile_func_begin;
  if (!global_ctx_get()) {
    profile_func_end;
    return;
  }
  global_user_data_access_set_all_local(has_access);
  profile_func_end;
}

func void global_clear_temp(void) {
  profile_func_begin;
  ctx_clear_temp(global_ctx_get_shared());
  profile_func_end;
}

func void global_log_set_level(log_level level) {
  profile_func_begin;
  log_state_set_level(global_get_log_state(), level);
  profile_func_end;
}

func void global_log_begin_frame(void) {
  profile_func_begin;
  log_state_begin_frame(global_get_log_state());
  profile_func_end;
}

func log_frame* global_log_end_frame(u32 severity_mask) {
  return log_state_end_frame(global_get_log_state(), severity_mask);
}
