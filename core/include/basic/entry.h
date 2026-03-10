// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../memory/vmem.h"
#include "../utils/cmdline.h"
#include "env_defines.h"
#include "primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

/*

This header defines the public entry API and the compile-time wiring used by
entry_impl.h to generate platform entry wrappers.

How it works:
1) Choose exactly one entry mode by defining one of ENTRY_TYPE_APP,
   ENTRY_TYPE_RUN, or ENTRY_TYPE_MOD before including this header.
2) Implement the callbacks required by that mode (app_* / run / mod_*).

User-definable macros:
- ENTRY_TYPE_APP
  Selects SDL app lifecycle mode. Requires app_init, app_update, app_quit.
- ENTRY_TYPE_RUN
  Selects simple command-line mode. Requires run.
- ENTRY_TYPE_MOD
  Selects module mode. Requires mod_init and mod_quit exports.
- ENTRY_GET_GLOBAL_ALLOCATOR
  Override allocator getter used by generated wrapper.
  Default is vmem_get_allocator.
- ENTRY_WINDOWS_CONSOLE
  On Windows, force console main path instead of WinMain/wWinMain path.

*/

// =========================================================================
// Shared Program Lifecycle
// =========================================================================

// You can define ENTRY_GET_GLOBAL_ALLOCATOR to override the global allocator
// passed into internal entry functions.
#ifndef ENTRY_GET_GLOBAL_ALLOCATOR
#  define ENTRY_GET_GLOBAL_ALLOCATOR vmem_get_allocator
#endif

// Common initialization hook used by custom entry points.
// The built-in entry paths above also route through this function internally.
// Returns true on success, false on failure.
func b32 entry_init(cmdline cmdline);

// Returns the command line captured during the first successful entry_init().
func cmdline entry_get_cmdline(void);

// Common shutdown hook paired with entry_init().
// Safe to call even if initialization failed.
func void entry_quit(void);

// Enum to describe app result
typedef enum app_result {
  APP_RESULT_CONTINUE,  // Continue calling update().
  APP_RESULT_SUCCESS,   // Exit successfully.
  APP_RESULT_FAIL,      // Exit with failure.
  APP_RESULT_MAX,
} app_result;

// Callback bundles passed from generated platform entry wrappers
// to the runtime entry implementation for app entry point.
typedef app_result entry_app_init_fn(cmdline cmdl, void** state);
typedef app_result entry_app_update_fn(void* state);
typedef void entry_app_quit_fn(void* state);
typedef struct entry_app_callbacks {
  entry_app_init_fn* init_fn;
  entry_app_update_fn* update_fn;
  entry_app_quit_fn* quit_fn;
} entry_app_callbacks;

// Callback bundles passed from generated platform entry wrappers
// to the runtime entry implementation for run entry point.
typedef b32 entry_run_fn(cmdline cmdl);
typedef struct entry_run_callbacks {
  entry_run_fn* run_fn;
} entry_run_callbacks;

// =========================================================================
// Application entry point
// =========================================================================

// SDL-style application lifecycle used for cross-platform entry handling.
// The runtime owns the outer platform entry point and forwards the parsed
// command line plus caller-managed state through this interface.
#if defined(ENTRY_TYPE_APP)

// Called once before the main loop starts.
// Implementations can allocate and store user state through state.
func app_result app_init(cmdline cmdl, void** state);

// Called repeatedly until it returns APP_RESULT_SUCCESS or APP_RESULT_FAIL.
func app_result app_update(void* state);

// Called once during shutdown, even after a failed init().
func void app_quit(void* state);

// entry_impl.h generates a platform entry wrapper that forwards
// this bundle into main_app(..., callbacks).
#  define ENTRY_FUNCTION_NAME    main_app
#  define ENTRY_CALLBACKS_TYPE   entry_app_callbacks
#  define ENTRY_CALLBACKS_INIT() ((entry_app_callbacks) {app_init, app_update, app_quit})

// =========================================================================
// Run entry point
// =========================================================================

// Simple command-line entry point wrapper.
// Returns true on success, false on failure.
#elif defined(ENTRY_TYPE_RUN)

func b32 run(cmdline cmdl);

// entry_impl.h generates a platform entry wrapper that forwards
// this bundle into main_run(..., callbacks).
#  define ENTRY_FUNCTION_NAME    main_run
#  define ENTRY_CALLBACKS_TYPE   entry_run_callbacks
#  define ENTRY_CALLBACKS_INIT() ((entry_run_callbacks) {run})

// =========================================================================
// Module entry point
// =========================================================================

// Module lifecycle used by the custom dynamic-module loader.
// mod_init() runs when the module is loaded and mod_quit() runs when it
// is unloaded.
#elif defined(ENTRY_TYPE_MOD)

// Exported wrapper used by the module loader.
func dll_export b32 mod_init(void);

// Exported wrapper used by the module loader.
func dll_export void mod_quit(void);

#endif

// =========================================================================
c_end;
// =========================================================================