// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/entry.h"
#include "basic/crash.h"
#include "context/global_ctx.h"
#include "context/thread_ctx.h"
#include "../internal.h"
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
global_var ctx_setup entry_start_setup = {0};
global_var u32 entry_init_depth = 0;
global_var u64 entry_internal_msg_handler_ids[6] = {0};

func void entry_register_internal_msg_handlers(void) {
  profile_func_begin;
  if (entry_internal_msg_handler_ids[0] != 0) {
    profile_func_end;
    return;
  }

  entry_internal_msg_handler_ids[0] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = keyboard_internal_on_msg,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  entry_internal_msg_handler_ids[1] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = mouse_internal_on_msg,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  entry_internal_msg_handler_ids[2] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = gamepad_internal_on_msg,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  entry_internal_msg_handler_ids[3] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = joystick_internal_on_msg,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  entry_internal_msg_handler_ids[4] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = tablet_internal_on_msg,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  entry_internal_msg_handler_ids[5] = msg_add_handler(&(msg_handler_desc) {
      .handler_fn = msg_handle_runtime_internal,
      .priority = I32_MAX,
      .category = MSG_CATEGORY_CORE,
  });
  profile_func_end;
}

func void entry_unregister_internal_msg_handlers(void) {
  profile_func_begin;
  safe_for (sz item_idx = 0; item_idx < count_of(entry_internal_msg_handler_ids); item_idx += 1) {
    if (entry_internal_msg_handler_ids[item_idx] != 0) {
      (void)msg_remove_handler(entry_internal_msg_handler_ids[item_idx]);
      entry_internal_msg_handler_ids[item_idx] = 0;
    }
  }
  profile_func_end;
}

func void* entry_pipeline_malloc(sz size) {
  profile_func_begin;
  allocator alloc = vmem_get_allocator();
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
  allocator alloc = vmem_get_allocator();
  allocator_dealloc(alloc, ptr);
  profile_func_end;
}

func void* entry_pipeline_calloc(sz count, sz size) {
  profile_func_begin;
  allocator alloc = vmem_get_allocator();
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
  allocator alloc = vmem_get_allocator();
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

static const SDL_InitFlags SDL_INIT_REQUIRED_FLAGS = SDL_INIT_EVENTS;
static const u32 ENTRY_THREAD_IDLE_WAIT_TIMEOUT_MS = 2000;

typedef struct entry_optional_sdl_subsystem {
  SDL_InitFlags flags;
  cstr8 name;
} entry_optional_sdl_subsystem;

static const entry_optional_sdl_subsystem ENTRY_OPTIONAL_SDL_SUBSYSTEMS[] = {
    {   SDL_INIT_AUDIO,    "audio"},
    {   SDL_INIT_VIDEO,    "video"},
    {SDL_INIT_JOYSTICK, "joystick"},
    {  SDL_INIT_HAPTIC,   "haptic"},
    { SDL_INIT_GAMEPAD,  "gamepad"},
    {  SDL_INIT_SENSOR,   "sensor"},
    {  SDL_INIT_CAMERA,   "camera"},
};

func b32 entry_try_init_sdl_dummy_driver(SDL_InitFlags flags, cstr8 subsystem_name, cstr8 env_name) {
  profile_func_begin;

  if (SDL_getenv(env_name) != NULL) {
    global_log_warn("SDL %s subsystem unavailable and %s is already set error=%s",
                    subsystem_name,
                    env_name,
                    SDL_GetError());
    profile_func_end;
    return false;
  }

  if (SDL_setenv_unsafe(env_name, "dummy", 1) != 0) {
    global_log_warn("Failed setting %s=dummy for SDL %s subsystem error=%s",
                    env_name,
                    subsystem_name,
                    SDL_GetError());
    profile_func_end;
    return false;
  }

  global_log_warn("Retrying SDL %s subsystem with %s=dummy", subsystem_name, env_name);
  if (SDL_InitSubSystem(flags)) {
    global_log_info("SDL %s subsystem initialized with %s=dummy", subsystem_name, env_name);
    profile_func_end;
    return true;
  }

  global_log_warn("SDL %s subsystem still unavailable after %s=dummy error=%s",
                  subsystem_name,
                  env_name,
                  SDL_GetError());
  profile_func_end;
  return false;
}

func void entry_init_optional_sdl_subsystem(SDL_InitFlags flags, cstr8 subsystem_name) {
  profile_func_begin;

  if (SDL_InitSubSystem(flags)) {
    global_log_verbose("SDL %s subsystem initialized", subsystem_name);
    profile_func_end;
    return;
  }

  global_log_warn("SDL %s subsystem unavailable error=%s", subsystem_name, SDL_GetError());

  if (flags == SDL_INIT_VIDEO) {
    (void)entry_try_init_sdl_dummy_driver(flags, subsystem_name, "SDL_VIDEODRIVER");
  } else if (flags == SDL_INIT_AUDIO) {
    (void)entry_try_init_sdl_dummy_driver(flags, subsystem_name, "SDL_AUDIODRIVER");
  }

  profile_func_end;
}

func b32 entry_init(cmdline cmdline) {
  profile_func_begin;

  if (entry_init_depth > 0) {
    entry_init_depth += 1;
    global_log_trace("Entry runtime already initialized depth=%u", entry_init_depth);
    profile_func_end;
    return true;
  }

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
    global_log_verbose("Rolling back global context after thread init failure");
    global_ctx_quit();
    profile_func_end;
    return false;
  }

  global_log_info("Initializing entry runtime");
  global_log_trace("Entry init state cmdline_count=%zu has_allocator=%u has_global_ctx=%u has_thread_ctx=%u",
                   (size_t)cmdline.count,
                   (u32)(entry_start_setup.main_allocator.alloc_fn != NULL),
                   (u32)global_ctx_is_init(),
                   (u32)thread_ctx_is_init());

  global_log_verbose("Installing crash handler");
  if (!crash_install()) {
    global_log_error("Crash handler installation failed");
    thread_log_verbose("Rolling back thread context after crash handler install failure");
    if (!thread_ctx_quit()) {
      global_log_error("Failed rolling back thread context after crash handler install failure");
    }
    global_log_verbose("Rolling back global context after crash handler install failure");
    global_ctx_quit();
    profile_func_end;
    return false;
  }

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
    global_log_verbose("Uninstalling crash handler after SDL memory hook failure");
    crash_uninstall();
    thread_log_verbose("Rolling back thread context after SDL memory hook failure");
    if (!thread_ctx_quit()) {
      global_log_error("Failed rolling back thread context after SDL memory hook failure");
    }
    global_log_verbose("Rolling back global context after SDL memory hook failure");
    global_ctx_quit();
    profile_func_end;
    return false;
  }

  global_log_verbose("Initializing SDL required flags=0x%08X", (unsigned int)SDL_INIT_REQUIRED_FLAGS);
  if (!SDL_Init(SDL_INIT_REQUIRED_FLAGS)) {
    global_log_error("SDL_Init failed error=%s", SDL_GetError());
    global_log_verbose("Uninstalling crash handler after SDL init failure");
    crash_uninstall();
    thread_log_verbose("Rolling back thread context after SDL init failure");
    if (!thread_ctx_quit()) {
      global_log_error("Failed rolling back thread context after SDL init failure");
    }
    global_log_verbose("Rolling back global context after SDL init failure");
    global_ctx_quit();
    profile_func_end;
    return false;
  }
  safe_for (sz idx = 0; idx < count_of(ENTRY_OPTIONAL_SDL_SUBSYSTEMS); idx++) {
    entry_optional_sdl_subsystem subsystem = ENTRY_OPTIONAL_SDL_SUBSYSTEMS[idx];
    entry_init_optional_sdl_subsystem(subsystem.flags, subsystem.name);
  }

  global_log_info("SDL initialized");

  entry_register_internal_msg_handlers();

  if (cmdline_is_empty(entry_cmdline_current)) {
    entry_cmdline_current = cmdline;
    global_log_trace("Captured entry command line count=%zu", (size_t)entry_cmdline_current.count);
  }

  entry_init_depth = 1;
  global_log_trace("Entry runtime initialized depth=%u", entry_init_depth);

  if (!thread_log_sync()) {
    thread_log_warn("Thread log sync failed");
  }

  thread_log_info("Entry runtime initialized");
  profile_func_end;
  return true;
}

func void entry_quit(void) {
  profile_func_begin;

  if (entry_init_depth > 1) {
    entry_init_depth -= 1;
    global_log_trace("Skipping shutdown while nested entry init is active depth=%u", entry_init_depth);
    profile_func_end;
    return;
  }

  b32 has_thread_ctx = thread_ctx_is_init();
  b32 has_global_ctx = global_ctx_is_init();
  b32 has_sdl = SDL_WasInit(0) != 0;

  global_log_info("Shutting down entry runtime");
  global_log_trace("Entry quit state has_thread_ctx=%u has_global_ctx=%u has_sdl=%u",
                   (u32)has_thread_ctx,
                   (u32)has_global_ctx,
                   (u32)has_sdl);

  if (core_thread_active_count() != 0) {
    global_log_verbose("Waiting for worker threads before context shutdown active=%u",
                       core_thread_active_count());
    if (!core_thread_wait_idle(ENTRY_THREAD_IDLE_WAIT_TIMEOUT_MS)) {
      global_log_warn("Worker threads are still running after timeout active=%u",
                      core_thread_active_count());
    } else {
      global_log_trace("All worker threads completed before shutdown");
    }
  }

  if (!has_thread_ctx && !has_global_ctx && !has_sdl) {
    global_log_verbose("Entry runtime already shut down");
    entry_init_depth = 0;
    entry_cmdline_current = (cmdline) {0};
    entry_start_setup = (ctx_setup) {0};
    profile_func_end;
    return;
  }

  if (crash_is_installed()) {
    global_log_verbose("Uninstalling crash handler");
    crash_uninstall();
  }

  if (has_thread_ctx) {
    thread_log_verbose("Shutting down thread context");
    if (!thread_ctx_quit()) {
      thread_log_error("Failed shutting thread context");
    }
  }

  if (has_global_ctx) {
    global_log_verbose("Shutting down global context");
    global_ctx_quit();
  }

  if (has_sdl) {
    entry_unregister_internal_msg_handlers();
    global_log_verbose("Shutting down SDL");
    SDL_Quit();
  }

  entry_init_depth = 0;
  entry_cmdline_current = (cmdline) {0};
  entry_start_setup = (ctx_setup) {0};

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
  entry_start_setup = setup;
  run_callbacks = callbacks;
  entry_cmdline_current = cmdline_build((sz)argc, (c8**)argv);
  int res = SDL_RunApp(argc, argv, main_run_internal, NULL);
  profile_func_end;
  return res;
}
