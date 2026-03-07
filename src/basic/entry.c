// MIT License
// Copyright (c) 2026 Christian Luppi

#if defined(ENTRY_TYPE_APP)
#  define SDL_MAIN_USE_CALLBACKS 1
#endif

#include "basic/entry.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "memory/vmem.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"

#include <stdlib.h>

#if defined(ENTRY_TYPE_MAIN)
func b32 entry_main(cmdline cmdl);
#endif

// =========================================================================
// Entry runtime state
// =========================================================================

typedef struct entry_shared_state {
  sz sdl_init_depth;
  b32 owns_sdl;
  sz global_ctx_init_depth;
  b32 owns_global_ctx;
} entry_shared_state;

typedef struct entry_thread_state {
  sz thread_ctx_init_depth;
  b32 owns_thread_ctx;
} entry_thread_state;

global_var entry_shared_state entry_shared = {0};
thread_local global_var entry_thread_state entry_thread = {0};

// =========================================================================
// Common lifecycle hooks
// =========================================================================

func b32 entry_init(cmdline cmdline) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  (void)cmdline;

#ifdef OVERRIDE_GLOBAL_DEFAULT_ALLOCATOR
  allocator default_global_allocator = global_allocator_default();
#else
  allocator default_global_allocator = vmem_get_allocator();
#endif

  if (entry_shared.sdl_init_depth == 0) {
    if (!SDL_WasInit(0)) {
      if (!SDL_Init(0)) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }
      thread_log_trace("entry_init: SDL initialized");
      entry_shared.owns_sdl = true;
    } else {
      entry_shared.owns_sdl = false;
    }
  }
  entry_shared.sdl_init_depth += 1;

  if (entry_shared.global_ctx_init_depth == 0) {
    if (!global_ctx_is_init()) {
      if (!global_ctx_init(default_global_allocator)) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }

      entry_shared.owns_global_ctx = true;
      thread_log_trace("entry_init: global_ctx initialized");
    } else {
      entry_shared.owns_global_ctx = false;
    }
  }
  entry_shared.global_ctx_init_depth += 1;

  if (entry_thread.thread_ctx_init_depth == 0) {
    if (!thread_ctx_is_init()) {
      if (!thread_ctx_init(global_get_allocator())) {
        TracyCZoneEnd(__tracy_zone_ctx);
        return false;
      }

      entry_thread.owns_thread_ctx = true;
      (void)thread_log_sync();
      thread_log_trace("entry_init: thread_ctx initialized");
    } else {
      entry_thread.owns_thread_ctx = false;
    }
  }
  entry_thread.thread_ctx_init_depth += 1;

  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func void entry_quit(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (entry_thread.thread_ctx_init_depth > 0) {
    entry_thread.thread_ctx_init_depth -= 1;
    if (entry_thread.thread_ctx_init_depth == 0 && entry_thread.owns_thread_ctx) {
      thread_log_trace("entry_quit: thread_ctx");
      thread_ctx_quit();
      entry_thread.owns_thread_ctx = false;
    }
  }

  if (entry_shared.global_ctx_init_depth > 0) {
    entry_shared.global_ctx_init_depth -= 1;
    if (entry_shared.global_ctx_init_depth == 0 && entry_shared.owns_global_ctx) {
      thread_log_trace("entry_quit: global_ctx");
      global_ctx_quit();
      entry_shared.owns_global_ctx = false;
    }
  }

  if (entry_shared.sdl_init_depth > 0) {
    entry_shared.sdl_init_depth -= 1;
    if (entry_shared.sdl_init_depth == 0 && entry_shared.owns_sdl) {
      thread_log_trace("entry_quit: SDL");
      SDL_Quit();
      entry_shared.owns_sdl = false;
    }
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

// =========================================================================
// Application entry point
// =========================================================================

#if defined(ENTRY_TYPE_APP)

global_var b32 app_entry_initialized = false;
global_var b32 app_callbacks_started = false;

func SDL_AppResult app_result_to_sdl_result(app_result result) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  switch (result) {
    case APP_RESULT_CONTINUE:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_CONTINUE;
    case APP_RESULT_SUCCESS:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_SUCCESS;
    case APP_RESULT_FAIL:
    case APP_RESULT_MAX:
      TracyCZoneEnd(__tracy_zone_ctx);
      return SDL_APP_FAILURE;
  }

  TracyCZoneEnd(__tracy_zone_ctx);
  return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  cmdline cmdl = cmdline_build((sz)argc, (c8**)argv);

  if (appstate != NULL) {
    *appstate = NULL;
  }

  app_entry_initialized = false;
  app_callbacks_started = false;

  if (!entry_init(cmdl)) {
    return SDL_APP_FAILURE;
  }
  app_entry_initialized = true;

  app_callbacks_started = true;
  return app_result_to_sdl_result(app_init(cmdl, appstate));
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  (void)result;

  if (app_callbacks_started) {
    app_quit(appstate);
    app_callbacks_started = false;
  }

  if (app_entry_initialized) {
    entry_quit();
    app_entry_initialized = false;
  }
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  return app_result_to_sdl_result(app_update(appstate));
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  msg based_msg = {0};
  (void)appstate;
  if (event->type == SDL_EVENT_QUIT) {
    if (msg_from_native(event, &based_msg)) {
      (void)msg_post(&based_msg);
    }
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

// =========================================================================
// Command-line entry point
// =========================================================================

#elif defined(ENTRY_TYPE_MAIN)

int main(int argc, char** argv) {
  cmdline cmdl = cmdline_build((sz)argc, (c8**)argv);
  if (!entry_init(cmdl)) {
    return EXIT_FAILURE;
  }

  b32 result = entry_main(cmdl);
  entry_quit();
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

// =========================================================================
// Module entry point
// =========================================================================

#elif defined(ENTRY_TYPE_MOD)

global_var b32 module_entry_initialized = false;

func dll_export b32 mod_init(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  cmdline empty_cmdline = cmdline_build(0, NULL);

  module_entry_initialized = false;

  if (!entry_init(empty_cmdline)) {
    TracyCZoneEnd(__tracy_zone_ctx);
    return false;
  }
  module_entry_initialized = true;
  TracyCZoneEnd(__tracy_zone_ctx);
  return true;
}

func dll_export void mod_quit(void) {
  TracyCZoneN(__tracy_zone_ctx, __func__, 1);
  if (module_entry_initialized) {
    entry_quit();
    module_entry_initialized = false;
  }
  TracyCZoneEnd(__tracy_zone_ctx);
}

#endif
