// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

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

// Terminates the current process with the provided exit code.
// This does not return.
func no_return void process_exit(i32 exit_code);

// Abnormally terminates the current process.
// This does not return.
func no_return void process_abort(void);