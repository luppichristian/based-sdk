// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "process.h"

// =========================================================================
c_begin;
// =========================================================================

// =========================================================================
// Pipe
// =========================================================================

// Opaque handle to a process-owned stdio pipe.
typedef void* pipe;

// Returns the stdin pipe for a spawned process when pipe_stdin was enabled.
// Returns NULL when stdin is not available.
func pipe pipe_stdin(process prc);

// Returns the stdout pipe for a spawned process when pipe_stdout was enabled.
// Returns NULL when stdout is not available.
func pipe pipe_stdout(process prc);

// Returns the stderr pipe for a spawned process when pipe_stderr was enabled.
// Returns NULL when stderr is not available.
func pipe pipe_stderr(process prc);

// Returns true if pip is a valid pipe handle.
func b32 pipe_is_valid(pipe pip);

// Reads up to size bytes from a pipe.
// Returns the number of bytes read.
func sz pipe_read(pipe pip, void* ptr, sz size);

// Writes up to size bytes to a pipe.
// Returns the number of bytes written.
func sz pipe_write(pipe pip, const void* ptr, sz size);
// Non-blocking I/O and poll helpers.
func sz pipe_read_nonblocking(pipe pip, void* ptr, sz size);
func sz pipe_write_nonblocking(pipe pip, const void* ptr, sz size);
func b32 pipe_poll_readable(pipe pip, i32 timeout_ms);

// Flushes buffered data for a writable pipe.
func b32 pipe_flush(pipe pip);

// Closes a pipe.
// Closing stdin is the portable way to signal EOF to the child process.
func void pipe_close(pipe pip);

// =========================================================================
c_end;
// =========================================================================
