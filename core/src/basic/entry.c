// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/entry.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "memory/vmem.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "based_core.h"

// =========================================================================
// Entry runtime state
// =========================================================================

global_var cmdline entry_cmdline_current = {0};
global_var allocator entry_start_allocator = {0};

func void* entry_pipeline_malloc(sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* ptr = _allocator_alloc(&alloc, size, CALLSITE_HERE);
  profile_func_end;
  return ptr;
}

func void entry_pipeline_free(void* ptr) {
  profile_func_begin;
  if (ptr == NULL) {
    profile_func_end;
    return;
  }
  allocator alloc = thread_get_allocator();
  _allocator_dealloc(&alloc, ptr, 0, CALLSITE_HERE);
  profile_func_end;
}

func void* entry_pipeline_calloc(sz count, sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* ptr = _allocator_calloc(&alloc, count, size, CALLSITE_HERE);
  profile_func_end;
  return ptr;
}

func void* entry_pipeline_realloc(void* ptr, sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* new_ptr = _allocator_realloc(&alloc, ptr, 0, size, CALLSITE_HERE);
  profile_func_end;
  return new_ptr;
}

func cmdline entry_get_cmdline(void) {
  return entry_cmdline_current;
}

func allocator entry_get_allocator(void) {
  return entry_start_allocator;
}

// =========================================================================
// Common lifecycle hooks
// =========================================================================

const_var SDL_InitFlags SDL_INIT_FLAGS =
    SDL_INIT_AUDIO |
    SDL_INIT_VIDEO |
    SDL_INIT_JOYSTICK |
    SDL_INIT_HAPTIC |
    SDL_INIT_GAMEPAD |
    SDL_INIT_EVENTS |
    SDL_INIT_SENSOR |
    SDL_INIT_CAMERA;

func b32 entry_init(cmdline cmdline) {
  profile_func_begin;
  global_log_trace("begin cmdline_count=%zu", (size_t)cmdline.count);
  if (entry_cmdline_current.args == NULL && cmdline.args != NULL && cmdline.count > 0) {
    entry_cmdline_current = cmdline;
    global_log_trace("captured cmdline count=%zu", (size_t)cmdline.count);
  }

  // Set memory hooks for olib.
  olib_set_memory_fns(
      entry_pipeline_malloc,
      entry_pipeline_free,
      entry_pipeline_calloc,
      entry_pipeline_realloc);

  // SDL memory hooks must be configured before SDL init.
  if (!SDL_SetMemoryFunctions(
          entry_pipeline_malloc,
          entry_pipeline_calloc,
          entry_pipeline_realloc,
          entry_pipeline_free)) {
    global_log_error("SDL_SetMemoryFunctions failed (%s)", SDL_GetError());
    return false;
  }

  if (!SDL_Init(SDL_INIT_FLAGS)) {
    global_log_error("SDL_Init failed (%s)", SDL_GetError());
    return false;
  }

  global_log_trace("SDL initialized");

  if (!global_ctx_init(entry_start_allocator)) {
    global_log_error("global_ctx_init failed");
    return false;
  }

  global_log_trace("global_ctx initialized");
  if (!thread_ctx_init(global_get_allocator())) {
    global_log_error("thread_ctx_init failed");
    return false;
  }

  (void)thread_log_sync();
  profile_func_end;
  return true;
}

func void entry_quit(void) {
  profile_func_begin;
  b32 has_thread_ctx = thread_ctx_is_init();
  b32 has_global_ctx = global_ctx_is_init();
  b32 has_sdl = SDL_WasInit(0) != 0;

  if (has_sdl) {
    global_log_trace("quitting SDL");
    SDL_Quit();
  }

  if (has_thread_ctx) {
    global_log_trace("quitting thread_ctx");
    thread_ctx_quit();
  }

  if (has_global_ctx) {
    global_log_trace("quitting global_ctx");
    global_ctx_quit();
  }

  if (!thread_ctx_is_init() && !global_ctx_is_init() && SDL_WasInit(0) == 0) {
    entry_cmdline_current = (cmdline) {0};
    entry_start_allocator = (allocator) {0};
  }
  profile_func_end;
}

// =========================================================================
// App entry point
// =========================================================================

global_var entry_app_callbacks app_callbacks = {0};

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

func app_result app_result_from_sdl_result(SDL_AppResult result) {
  switch (result) {
    case SDL_APP_CONTINUE:
      return APP_RESULT_CONTINUE;
    case SDL_APP_SUCCESS:
      return APP_RESULT_SUCCESS;
    case SDL_APP_FAILURE:
      return APP_RESULT_FAIL;
  }

  return APP_RESULT_MAX;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  profile_func_begin;
  (void)argc;
  (void)argv;
  global_log_info("Using entry point of type ENTRY_TYPE_APP");

  if (!entry_init(entry_cmdline_current)) {
    global_log_fatal("entry initialization failed");
    entry_quit();
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  if (app_callbacks.init_fn == NULL || app_callbacks.update_fn == NULL || app_callbacks.quit_fn == NULL) {
    global_log_fatal("missing app callbacks init=%u update=%u quit=%u",
                     (u32)(app_callbacks.init_fn != NULL),
                     (u32)(app_callbacks.update_fn != NULL),
                     (u32)(app_callbacks.quit_fn != NULL));
    entry_quit();
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  app_result result = app_callbacks.init_fn(entry_cmdline_current, appstate);
  global_log_info("app_init result=%u", (u32)result);
  profile_func_end;
  return app_result_to_sdl_result(result);
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  profile_func_begin;
  global_log_info("app quit result=%u", (u32)result);
  if (app_callbacks.quit_fn != NULL) {
    app_callbacks.quit_fn(appstate);
  }
  entry_quit();
  profile_func_end;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  profile_func_begin;
  if (app_callbacks.update_fn == NULL) {
    global_log_error("update callback is NULL");
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  app_result res = app_callbacks.update_fn(appstate);
  global_log_verbose("app_update result=%u", (u32)res);
  profile_func_end;
  return app_result_to_sdl_result(res);
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  profile_func_begin;
  (void)appstate;
  msg based_msg = {0};

  if (msg_from_native(event, &based_msg)) {
    if (!msg_post(&based_msg)) {
      cstr8 category = "";
      cstr8 type = "";
      // msg_get_strings(&based_msg, &category, &type); // TODO: Implement

      global_log_warn("Event %s:%s is not cancellable", category, type);
    }
  }

  profile_func_end;
  return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

int main_app(int argc, char** argv, allocator alloc, entry_app_callbacks callbacks) {
  profile_func_begin;
  global_log_trace("app start argc=%d", argc);
  entry_start_allocator = alloc;
  app_callbacks = callbacks;
  entry_cmdline_current = cmdline_build((sz)argc, (c8**)argv);
  int res = SDL_EnterAppMainCallbacks(argc, argv, SDL_AppInit, SDL_AppIterate, SDL_AppEvent, SDL_AppQuit);
  profile_func_end;
  return res;
}

// =========================================================================
// Run entry point
// =========================================================================

global_var entry_run_callbacks run_callbacks = {0};

int main_run_internal(int argc, char** argv) {
  profile_func_begin;
  (void)argc;
  (void)argv;
  global_log_info("Using entry point of type ENTRY_TYPE_RUN");

  if (!entry_init(entry_cmdline_current)) {
    global_log_fatal("Failed to initialize based_core");
    entry_quit();
    profile_func_end;
    return EXIT_FAILURE;
  }

  if (run_callbacks.run_fn == NULL) {
    global_log_fatal("Callback for run entry point is NULL, quitting immediately");
    entry_quit();
    profile_func_end;
    return EXIT_FAILURE;
  }

  global_log_verbose("Running user defined run() function ....");
  b32 result = run_callbacks.run_fn(entry_cmdline_current);
  global_log_info("run result=%u", (u32)result);

  entry_quit();
  profile_func_end;
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main_run(int argc, char** argv, allocator alloc, entry_run_callbacks callbacks) {
  profile_func_begin;
  global_log_trace("run start argc=%d", argc);
  entry_start_allocator = alloc;
  run_callbacks = callbacks;
  entry_cmdline_current = cmdline_build((sz)argc, (c8**)argv);
  int res = SDL_RunApp(argc, argv, main_run_internal, NULL);
  profile_func_end;
  return res;
}
