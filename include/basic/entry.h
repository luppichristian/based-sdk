// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../memory/allocator.h"
#include "../utils/cmdline.h"
#include "env_defines.h"
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
func app_result app_init(cmdline cmdl, void** state);

// Called repeatedly until it returns APP_RESULT_SUCCESS or APP_RESULT_FAIL.
func app_result app_update(void* state);

// Called once during shutdown, even after a failed init().
func void app_quit(void* state);

#elif defined(ENTRY_TYPE_MAIN)

// Simple command-line entry point wrapper.
// Returns true on success, false on failure.
func b32 main(cmdline cmdl);

#elif defined(ENTRY_TYPE_MOD)

// Module lifecycle used by the custom dynamic-module loader.
// mod_init() runs when the module is loaded and mod_quit() runs when it
// is unloaded.

// Exported wrapper used by the module loader.
func dll_export b32 mod_init(void);

// Exported wrapper used by the module loader.
func dll_export void mod_quit(void);

#endif

// Entry mode and binary-kind consistency.
#if defined(ENTRY_TYPE_MOD) && !defined(BIN_DYNAMIC_LIB)
#  error "entry.h: ENTRY_TYPE_MOD requires BIN_DYNAMIC_LIB."
#endif

#if (defined(ENTRY_TYPE_APP) || defined(ENTRY_TYPE_MAIN)) && !defined(BIN_RUNNABLE)
#  error "entry.h: ENTRY_TYPE_APP and ENTRY_TYPE_MAIN require BIN_RUNNABLE."
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

// =========================================================================
// Override Global Default Allocator
// =========================================================================

// If you want, you can override the global ctx allocator.
// By default its set to vmem_get_allocator().
#ifdef OVERRIDE_GLOBAL_DEFAULT_ALLOCATOR
func allocator global_allocator_default();
#endif