// MIT License
// Copyright (c) 2026 Christian Luppi

#include "context/ctx.h"
#include "basic/assert.h"
#include "basic/utility_defines.h"
#include "context/thread_ctx.h"
#include "basic/profiler.h"
#include "memory/memops.h"

#include <string.h>

// Default reserved growth size for each context-local allocator.
static const sz CTX_DEFAULT_BLOCK_SIZE = kb(64);

func b32 ctx_setup_is_valid(ctx_setup* setup) {
  profile_func_begin;
  if (setup == NULL) {
    thread_log_error("Cannot validate NULL context setup");
    profile_func_end;
    return false;
  }

  if (!setup->main_allocator.alloc_fn || !setup->main_allocator.dealloc_fn) {
    thread_log_error("Context setup is missing a valid main allocator");
    profile_func_end;
    return false;
  }

  if ((setup->use_arena_allocs != false && setup->use_arena_allocs != true) ||
      (setup->use_heap_allocs != false && setup->use_heap_allocs != true) ||
      (setup->use_temp_allocs != false && setup->use_temp_allocs != true)) {
    thread_log_error("Context setup contains invalid boolean flags");
    profile_func_end;
    return false;
  }

  if (setup->use_arena_allocs && setup->perm_arena_block_size == 0) {
    thread_log_error("Context setup permanent arena block size is zero");
    profile_func_end;
    return false;
  }

  if (setup->use_heap_allocs && setup->perm_heap_block_size == 0) {
    thread_log_error("Context setup permanent heap block size is zero");
    profile_func_end;
    return false;
  }

  if (setup->use_temp_allocs && setup->use_arena_allocs && setup->temp_arena_block_size == 0) {
    thread_log_error("Context setup temporary arena block size is zero");
    profile_func_end;
    return false;
  }

  if (setup->use_temp_allocs && setup->use_heap_allocs && setup->temp_heap_block_size == 0) {
    thread_log_error("Context setup temporary heap block size is zero");
    profile_func_end;
    return false;
  }

  profile_func_end;
  return true;
}

func void ctx_setup_fill_defaults(ctx_setup* setup) {
  profile_func_begin;
  if (setup == NULL) {
    thread_log_warn("Cannot fill defaults for NULL context setup");
    profile_func_end;
    return;
  }

  if (!setup->use_arena_allocs && !setup->use_heap_allocs && !setup->use_temp_allocs) {
    setup->use_arena_allocs = true;
    setup->use_heap_allocs = true;
    setup->use_temp_allocs = true;
  }

  if (setup->perm_arena_block_size == 0) {
    setup->perm_arena_block_size = CTX_DEFAULT_BLOCK_SIZE;
  }
  if (setup->temp_arena_block_size == 0) {
    setup->temp_arena_block_size = CTX_DEFAULT_BLOCK_SIZE;
  }
  if (setup->perm_heap_block_size == 0) {
    setup->perm_heap_block_size = CTX_DEFAULT_BLOCK_SIZE;
  }
  if (setup->temp_heap_block_size == 0) {
    setup->temp_heap_block_size = CTX_DEFAULT_BLOCK_SIZE;
  }

  profile_func_end;
}

func b32 ctx_init(ctx* context, ctx_setup setup) {
  profile_func_begin;

  ctx_setup_fill_defaults(&setup);

  if (!context || context->is_init || !ctx_setup_is_valid(&setup)) {
    profile_func_end;
    return false;
  }

  mem_zero(context, size_of(*context));
  context->setup = setup;
  if (!log_state_init(&context->log, setup.mutex_handle, setup.main_allocator)) {
    mem_zero(context, size_of(*context));
    profile_func_end;
    return false;
  }

  if (setup.use_arena_allocs) {
    context->perm_arena = arena_create(setup.main_allocator, setup.mutex_handle, setup.perm_arena_block_size);
    if (setup.use_temp_allocs) {
      context->temp_arena = arena_create(setup.main_allocator, setup.mutex_handle, setup.temp_arena_block_size);
    }
  }

  if (setup.use_heap_allocs) {
    context->perm_heap = heap_create(setup.main_allocator, setup.mutex_handle, setup.perm_heap_block_size);
    if (setup.use_temp_allocs) {
      context->temp_heap = heap_create(setup.main_allocator, setup.mutex_handle, setup.temp_heap_block_size);
    }
  }

  context->is_init = true;
  profile_func_end;
  return true;
}

func b32 ctx_quit(ctx* context) {
  profile_func_begin;
  if (!context || !context->is_init) {
    profile_func_end;
    return false;
  }

  log_state_quit(&context->log);
  if (context->setup.use_heap_allocs && context->setup.use_temp_allocs) {
    heap_destroy(&context->temp_heap);
  }
  if (context->setup.use_heap_allocs) {
    heap_destroy(&context->perm_heap);
  }
  if (context->setup.use_arena_allocs && context->setup.use_temp_allocs) {
    arena_destroy(&context->temp_arena);
  }
  if (context->setup.use_arena_allocs) {
    arena_destroy(&context->perm_arena);
  }
  thread_log_trace("Context released context=%p", (void*)context);
  mem_zero(context, size_of(*context));
  profile_func_end;
  return true;
}

func b32 ctx_is_init(ctx* context) {
  return context != NULL && context->is_init;
}

func allocator ctx_get_allocator(ctx* context) {
  allocator alloc = {0};
  if (!ctx_is_init(context)) {
    return alloc;
  }
  return context->setup.main_allocator;
}

func ctx_setup ctx_get_setup(ctx* context) {
  ctx_setup setup = {0};
  if (!ctx_is_init(context)) {
    return setup;
  }
  return context->setup;
}

func log_state* ctx_get_log_state(ctx* context) {
  if (!ctx_is_init(context)) {
    return NULL;
  }
  return &context->log;
}

func arena* ctx_get_perm_arena(ctx* context) {
  return ctx_is_init(context) && context->setup.use_arena_allocs ? &context->perm_arena : NULL;
}

func arena* ctx_get_temp_arena(ctx* context) {
  return ctx_is_init(context) && context->setup.use_arena_allocs && context->setup.use_temp_allocs ? &context->temp_arena : NULL;
}

func heap* ctx_get_perm_heap(ctx* context) {
  return ctx_is_init(context) && context->setup.use_heap_allocs ? &context->perm_heap : NULL;
}

func heap* ctx_get_temp_heap(ctx* context) {
  return ctx_is_init(context) && context->setup.use_heap_allocs && context->setup.use_temp_allocs ? &context->temp_heap : NULL;
}

func void* ctx_get_user_data(ctx* context, ctx_user_data_idx idx) {
  profile_func_begin;
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    profile_func_end;
    return NULL;
  }
  assert(idx < CTX_USER_DATA_COUNT);
  profile_func_end;
  return context->user_data[idx];
}

func b32 ctx_set_user_data(ctx* context, ctx_user_data_idx idx, void* user_data) {
  profile_func_begin;
  if (!ctx_is_init(context) || idx >= CTX_USER_DATA_COUNT) {
    profile_func_end;
    return false;
  }
  assert(idx < CTX_USER_DATA_COUNT);

  context->user_data[idx] = user_data;
  profile_func_end;
  return true;
}

func void ctx_clear_temp(ctx* context) {
  profile_func_begin;
  if (!ctx_is_init(context)) {
    profile_func_end;
    return;
  }

  if (context->setup.use_arena_allocs && context->setup.use_temp_allocs) {
    arena_clear(&context->temp_arena);
  }
  if (context->setup.use_heap_allocs && context->setup.use_temp_allocs) {
    heap_clear(&context->temp_heap);
  }
  profile_func_end;
}
