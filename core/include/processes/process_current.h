// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Current Process
// =========================================================================

// Coarse scheduling priority levels for the current process.
// Mappings are platform-specific and intentionally approximate.
typedef enum process_priority {
  PROCESS_PRIORITY_LOW,
  PROCESS_PRIORITY_NORMAL,
  PROCESS_PRIORITY_HIGH,
  PROCESS_PRIORITY_REALTIME,
} process_priority;

// Returns the OS-level identifier of the current process.
func u64 process_id(void);

// Returns the scheduling priority of the current process.
func process_priority process_get_priority(void);

// Sets the scheduling priority of the current process.
// Higher priorities may require elevated privileges on some platforms.
func b32 process_set_priority(process_priority priority);

// Returns 1 if this process currently appears to be the only live instance of its executable.
func b32 process_is_unique(void);

// Spawns a fresh instance of the current executable using the captured startup command line,
// then exits the current process with code 0 on success.
func b32 process_restart(void);

// Terminates the current process with the provided exit code.
// This does not return.
func no_return void process_exit(i32 exit_code);

// Abnormally terminates the current process.
// This does not return.
func no_return void process_abort(void);

// =========================================================================
c_end;
// =========================================================================
