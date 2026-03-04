// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../utils/cmdline.h"
#include "primitive_types.h"

// =========================================================================
// Application Entry Points
// =========================================================================

#if defined(ENTRY_TYPE_APP)

// SDL-style application lifecycle used for cross-platform entry handling.
// The runtime owns the outer platform entry point and forwards the parsed
// command line plus caller-managed state through this interface.

typedef enum app_result {
  APP_RESULT_CONTINUE,  // Continue calling update().
  APP_RESULT_SUCCESS,   // Exit successfully.
  APP_RESULT_FAIL,      // Exit with failure.
  APP_RESULT_MAX,
} app_result;

// Called once before the main loop starts.
// Implementations can allocate and store user state through state.
func app_result init(cmdline cmdl, void** state);

// Called repeatedly until it returns APP_RESULT_SUCCESS or APP_RESULT_FAIL.
func app_result update(void* state);

// Called once during shutdown, even after a failed init().
func void quit(void* state);

#elif defined(ENTRY_TYPE_MAIN)

// Simple command-line entry point wrapper.
// Returns true on success, false on failure.
func b32 entry_main(cmdline cmdl);

#elif defined(ENTRY_TYPE_MODULE)

// Module lifecycle used by the custom dynamic-module loader.
// module_init() runs when the module is loaded and module_quit() runs when it
// is unloaded.

// Exported wrapper used by the module loader.
func dll_export b32 module_init(void);

// Exported wrapper used by the module loader.
func dll_export void module_quit(void);

// User callback invoked after entry_init() succeeds.
// Returns true on success, false on failure.
func b32 entry_module_init(void);

// User callback invoked before entry_quit().
// Safe to call even if initialization failed.
func void entry_module_quit(void);

#endif

// =========================================================================
// Shared Program Lifecycle
// =========================================================================

// Common initialization hook used by custom entry points.
// The built-in entry paths above also route through this function internally.
// Returns true on success, false on failure.
func b32 entry_init(cmdline cmdline);

// Common shutdown hook paired with entry_init().
// Safe to call even if initialization failed.
func void entry_quit(void);
