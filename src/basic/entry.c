// MIT License
// Copyright (c) 2026 Christian Luppi

#if defined(ENTRY_TYPE_APP)
#  define SDL_MAIN_USE_CALLBACKS 1
#endif

#include "basic/entry.h"
#include "memory/vmem.h"
#include "threads/thread_ctx.h"
#include "../sdl3_include.h"

#include <stdlib.h>

// =========================================================================
// Entry runtime state
// =========================================================================

global_var sz entry_sdl_init_depth = 0;
global_var b32 entry_owns_sdl = false;

thread_local global_var sz entry_thread_ctx_init_depth = 0;
thread_local global_var b32 entry_owns_thread_ctx = false;

// =========================================================================
// Common lifecycle hooks
// =========================================================================

func b32 entry_init(cmdline cmdline) {
  (void)cmdline;

  if (entry_sdl_init_depth == 0) {
    if (!SDL_WasInit(0)) {
      if (!SDL_Init(0)) {
        return false;
      }
      entry_owns_sdl = true;
    } else {
      entry_owns_sdl = false;
    }
  }
  entry_sdl_init_depth += 1;

  if (entry_thread_ctx_init_depth == 0) {
    if (!thread_ctx_is_initialized()) {
      if (!thread_ctx_init(vmem_get_allocator())) {
        entry_sdl_init_depth -= 1;
        if (entry_sdl_init_depth == 0 && entry_owns_sdl) {
          SDL_Quit();
          entry_owns_sdl = false;
        }
        return false;
      }

      entry_owns_thread_ctx = true;
      (void)thread_ctx_log_sync_from_main();
    } else {
      entry_owns_thread_ctx = false;
    }
  }
  entry_thread_ctx_init_depth += 1;

  return true;
}

func void entry_quit(void) {
  if (entry_thread_ctx_init_depth > 0) {
    entry_thread_ctx_init_depth -= 1;
    if (entry_thread_ctx_init_depth == 0 && entry_owns_thread_ctx) {
      thread_ctx_quit();
      entry_owns_thread_ctx = false;
    }
  }

  if (entry_sdl_init_depth > 0) {
    entry_sdl_init_depth -= 1;
    if (entry_sdl_init_depth == 0 && entry_owns_sdl) {
      SDL_Quit();
      entry_owns_sdl = false;
    }
  }
}

// =========================================================================
// Application entry point
// =========================================================================

#if defined(ENTRY_TYPE_APP)

global_var b32 app_entry_initialized = false;
global_var b32 app_callbacks_started = false;

func SDL_AppResult app_result_to_sdl_result(app_result result) {
  switch (result) {
    case APP_RESULT_CONTINUE:
      return SDL_APP_CONTINUE;
    case APP_RESULT_SUCCESS:
      return SDL_APP_SUCCESS;
    case APP_RESULT_FAIL:
    case APP_RESULT_MAX:
      return SDL_APP_FAILURE;
  }

  return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  cmdline cmdl = cmdline_make((sz)argc, (c8**)argv);

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
  return app_result_to_sdl_result(init(cmdl, appstate));
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  (void)result;

  if (app_callbacks_started) {
    quit(appstate);
    app_callbacks_started = false;
  }

  if (app_entry_initialized) {
    entry_quit();
    app_entry_initialized = false;
  }
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  return app_result_to_sdl_result(update(appstate));
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  (void)appstate;

  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;
  return SDL_APP_CONTINUE;
}

// =========================================================================
// Command-line entry point
// =========================================================================

#elif defined(ENTRY_TYPE_MAIN)

int main(int argc, char** argv) {
  cmdline cmdl = cmdline_make((sz)argc, (c8**)argv);
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

#elif defined(ENTRY_TYPE_MODULE)

global_var b32 module_entry_initialized = false;
global_var b32 module_callbacks_started = false;

func dll_export b32 module_init(void) {
  cmdline empty_cmdline = cmdline_make(0, NULL);

  module_entry_initialized = false;
  module_callbacks_started = false;

  if (!entry_init(empty_cmdline)) {
    return false;
  }
  module_entry_initialized = true;

  b32 result = entry_module_init();
  module_callbacks_started = result;
  if (!result) {
    entry_quit();
    module_entry_initialized = false;
  }

  return result;
}

func dll_export void module_quit(void) {
  if (module_callbacks_started) {
    entry_module_quit();
    module_callbacks_started = false;
  }

  if (module_entry_initialized) {
    entry_quit();
    module_entry_initialized = false;
  }
}

#endif
