// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/entry.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "../internal.h"
#include "input/msg.h"
#include "input/msg_core.h"
#include "memory/vmem.h"
#include "../sdl3_include.h"
#include "basic/profiler.h"
#include "based_core.h"

func void msg_notify_internal_listeners(const msg* src);

// =========================================================================
// Entry runtime state
// =========================================================================

global_var cmdline entry_cmdline_current = {0};
global_var ctx_setup entry_start_setup = {0};

func void* entry_pipeline_malloc(sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* ptr = allocator_alloc(alloc, size);
  if (ptr == NULL) {
    global_log_warn("Entry pipeline allocation failed size=%zu", (size_t)size);
  }
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
  allocator_dealloc(alloc, ptr);
  profile_func_end;
}

func void* entry_pipeline_calloc(sz count, sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* ptr = allocator_calloc(alloc, count, size);
  if (ptr == NULL) {
    global_log_warn("Entry pipeline zero-allocation failed count=%zu size=%zu",
                    (size_t)count,
                    (size_t)size);
  }
  profile_func_end;
  return ptr;
}

func void* entry_pipeline_realloc(void* ptr, sz size) {
  profile_func_begin;
  allocator alloc = thread_get_allocator();
  void* new_ptr = allocator_realloc(alloc, ptr, size);
  if (new_ptr == NULL && size > 0) {
    global_log_warn("Entry pipeline reallocation failed size=%zu", (size_t)size);
  }
  profile_func_end;
  return new_ptr;
}

func cmdline entry_get_cmdline(void) {
  return entry_cmdline_current;
}

func allocator entry_get_allocator(void) {
  return entry_start_setup.main_allocator;
}

func ctx_setup entry_get_ctx_setup(void) {
  return entry_start_setup;
}

// =========================================================================
// Common lifecycle hooks
// =========================================================================

static const SDL_InitFlags SDL_INIT_FLAGS =
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

  if (!global_ctx_init(entry_start_setup)) {
    global_log_error("Global context initialization failed");
    profile_func_end;
    return false;
  }

  global_log_info("Global context initialized");
  global_log_verbose("Initializing thread context");
  ctx_setup thread_setup = entry_start_setup;
  thread_setup.main_allocator = global_get_allocator();
  if (!thread_ctx_init(thread_setup)) {
    global_log_error("Thread context initialization failed");
    profile_func_end;
    return false;
  }

  global_log_info("Initializing entry runtime");
  global_log_trace("Entry init state cmdline_count=%zu has_allocator=%u has_global_ctx=%u has_thread_ctx=%u",
                   (size_t)cmdline.count,
                   (u32)(entry_start_setup.main_allocator.alloc_fn != NULL),
                   (u32)global_ctx_is_init(),
                   (u32)thread_ctx_is_init());

  // Set memory hooks for olib.
  global_log_verbose("Configuring olib memory hooks");
  olib_set_memory_fns(
      entry_pipeline_malloc,
      entry_pipeline_free,
      entry_pipeline_calloc,
      entry_pipeline_realloc);

  // SDL memory hooks must be configured before SDL init.
  global_log_verbose("Configuring SDL memory hooks");
  if (!SDL_SetMemoryFunctions(
          entry_pipeline_malloc,
          entry_pipeline_calloc,
          entry_pipeline_realloc,
          entry_pipeline_free)) {
    global_log_error("SDL_SetMemoryFunctions failed error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }

  global_log_verbose("Initializing SDL flags=0x%08X", (unsigned int)SDL_INIT_FLAGS);
  if (!SDL_Init(SDL_INIT_FLAGS)) {
    global_log_error("SDL_Init failed error=%s", SDL_GetError());
    profile_func_end;
    return false;
  }
  global_log_info("SDL initialized");

  if (!thread_log_sync()) {
    thread_log_warn("Thread log sync failed");
  }

  thread_log_info("Entry runtime initialized");
  profile_func_end;
  return true;
}

func void entry_quit(void) {
  profile_func_begin;
  b32 has_thread_ctx = thread_ctx_is_init();
  b32 has_global_ctx = global_ctx_is_init();
  b32 has_sdl = SDL_WasInit(0) != 0;

  global_log_info("Shutting down entry runtime");
  global_log_trace("Entry quit state has_thread_ctx=%u has_global_ctx=%u has_sdl=%u",
                   (u32)has_thread_ctx,
                   (u32)has_global_ctx,
                   (u32)has_sdl);

  if (!has_thread_ctx && !has_global_ctx && !has_sdl) {
    global_log_verbose("Entry runtime already shut down");
    entry_cmdline_current = (cmdline) {0};
    entry_start_setup = (ctx_setup) {0};
    profile_func_end;
    return;
  }

  if (has_sdl) {
    global_log_verbose("Shutting down SDL");
    SDL_Quit();
  }

  if (has_thread_ctx) {
    thread_log_verbose("Shutting down thread context");
    if (!thread_ctx_quit()) {
      thread_log_error("Failed shutting thread context");
    }
  }

  if (has_global_ctx) {
    global_log_verbose("Shutting down global context");
    if (!global_ctx_quit()) {
      thread_log_error("Failed shutting global context");
    }
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
  global_log_info("Using app entry point");
  global_log_trace("SDL_AppInit argc=%d argv=%p", argc, (void*)argv);

  if (!entry_init(entry_cmdline_current)) {
    global_log_fatal("Entry initialization failed");
    entry_quit();
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  if (app_callbacks.init_fn == NULL || app_callbacks.update_fn == NULL || app_callbacks.quit_fn == NULL) {
    thread_log_fatal("Missing app callbacks init=%u update=%u quit=%u",
                     (u32)(app_callbacks.init_fn != NULL),
                     (u32)(app_callbacks.update_fn != NULL),
                     (u32)(app_callbacks.quit_fn != NULL));
    entry_quit();
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  app_result result = app_callbacks.init_fn(entry_cmdline_current, appstate);
  thread_log_info("App init returned result=%u", (u32)result);
  profile_func_end;
  return app_result_to_sdl_result(result);
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  profile_func_begin;
  app_result based_result = app_result_from_sdl_result(result);
  thread_log_info("App quit received result=%u", (u32)based_result);
  if (app_callbacks.quit_fn != NULL) {
    app_callbacks.quit_fn(appstate, based_result);
  }
  entry_quit();
  profile_func_end;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  profile_func_begin;
  if (app_callbacks.update_fn == NULL) {
    thread_log_error("App update callback is NULL");
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  app_result res = app_callbacks.update_fn(appstate);
  thread_log_verbose("App update returned result=%u", (u32)res);
  profile_func_end;
  return app_result_to_sdl_result(res);
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  profile_func_begin;
  (void)appstate;

  if (event == NULL) {
    thread_log_error("Received NULL SDL event");
    profile_func_end;
    return SDL_APP_FAILURE;
  }

  thread_log_trace("Processing SDL event type=%u", (u32)event->type);

  msg based_msg = {0};

  if (!msg_post_native(event, &based_msg)) {
    if (!msg_from_native(event, &based_msg)) {
      thread_log_verbose("Ignored unsupported SDL event type=%u", (u32)event->type);
    } else {
      thread_log_trace("SDL event message was canceled type=%u", (u32)based_msg.type);
      profile_func_end;
      return SDL_APP_CONTINUE;
    }
  } else {
    thread_log_trace("Translated SDL event to message type=%u", (u32)based_msg.type);
  }

  profile_func_end;
  return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

int main_app(int argc, char** argv, ctx_setup setup, entry_app_callbacks callbacks) {
  profile_func_begin;
  global_log_info("Starting app runtime argc=%d", argc);
  entry_start_setup = setup;
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

  if (!entry_init(entry_cmdline_current)) {
    global_log_fatal("Entry initialization failed");
    entry_quit();
    profile_func_end;
    return EXIT_FAILURE;
  }

  if (run_callbacks.run_fn == NULL) {
    thread_log_fatal("Run callback is NULL");
    entry_quit();
    profile_func_end;
    return EXIT_FAILURE;
  }

  thread_log_verbose("Calling run callback");
  b32 result = run_callbacks.run_fn(entry_cmdline_current);
  thread_log_info("Run callback returned result=%u", (u32)result);

  entry_quit();
  profile_func_end;
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main_run(int argc, char** argv, ctx_setup setup, entry_run_callbacks callbacks) {
  profile_func_begin;
  global_log_info("Starting run runtime argc=%d", argc);
  entry_start_setup = setup;
  run_callbacks = callbacks;
  entry_cmdline_current = cmdline_build((sz)argc, (c8**)argv);
  int res = SDL_RunApp(argc, argv, main_run_internal, NULL);
  profile_func_end;
  return res;
}
