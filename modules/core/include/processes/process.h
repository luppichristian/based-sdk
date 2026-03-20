// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/codespace.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Process
// =========================================================================

// Opaque handle to a spawned process.
typedef void* process;

#define PROCESS_NAME_CAP 260

// Snapshot entry for an operating-system process.
typedef struct process_snapshot_entry {
  u64 id;
  c8 name[PROCESS_NAME_CAP];
} process_snapshot_entry;

// Spawn-time process I/O and lifecycle options.
typedef struct process_options {
  b32 pipe_stdin;
  b32 pipe_stdout;
  b32 pipe_stderr;
  b32 stderr_to_stdout;
  b32 background;
  cstr8 cwd;
  cstr8 const* envp;
  i32 timeout_ms;
} process_options;

// Returns the default spawn options:
// no pipes, no stderr redirection, foreground process.
func process_options process_options_default(void);

// Returns capture-friendly spawn options:
// stdout piped and stderr merged into stdout.
func process_options process_options_captured(void);

// Creates a new process with default options.
// args[0] must be the executable path and the array must be NULL-terminated.
func process _process_create(cstr8 const* args, callsite site);

// Creates a new process with explicit options.
// args[0] must be the executable path and the array must be NULL-terminated.
func process _process_create_with(cstr8 const* args, process_options options, callsite site);

#define process_create(args) \
  _process_create(args, CALLSITE_HERE)
#define process_create_with(args, options) \
  _process_create_with(args, options, CALLSITE_HERE)

// Returns true if prc is a valid process handle.
func b32 process_is_valid(process prc);

// Returns the OS-level identifier of a spawned process.
func u64 process_get_id(process prc);

// Returns a newly allocated snapshot of currently running OS processes.
// The returned array must be released with process_snapshot_free.
func process_snapshot_entry* process_snapshot_get(sz* out_count);

// Releases a process snapshot returned by process_snapshot_get.
func void process_snapshot_free(process_snapshot_entry* ptr);

// Reads the entire piped stdout stream, blocking until the process exits.
// The returned pointer is allocated by SDL and must be released with process_read_free.
// To capture stderr as well, create the process with process_options_captured().
func void* process_read(process prc, sz* out_size, i32* out_exit_code);

// Releases a buffer returned by process_read.
func void process_read_free(void* ptr);

// Waits for the process to exit.
// If block is false, this polls and returns false while the process is still running.
func b32 process_wait(process prc, b32 block, i32* out_exit_code);
// Waits up to timeout_ms for process exit.
func b32 process_wait_timeout(process prc, i32 timeout_ms, i32* out_exit_code);

// Requests termination of the process.
// If force is false, the OS is asked to terminate it gracefully when possible.
func b32 process_kill(process prc, b32 force);

// Destroys the tracking handle for a process.
// This does not terminate the process.
func void _process_destroy(process prc, callsite site);

#define process_destroy(prc) \
  _process_destroy(prc, CALLSITE_HERE)

// =========================================================================
c_end;
// =========================================================================
