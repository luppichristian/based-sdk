// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"
#include "../strings/cstrings.h"

// =========================================================================
c_begin;
// =========================================================================

// Maximum number of frames captured per call.
#ifndef STACKTRACE_CAPTURE_CAP
#  define STACKTRACE_CAPTURE_CAP 64
#endif

typedef struct stacktrace_frame {
  up address;
  str8_medium symbol;
} stacktrace_frame;

// Captures stack frames for the current thread.
// - out_frames: destination frame array.
// - out_capacity: number of entries in out_frames.
// - skip_frames: additional user frames to skip.
// Returns the number of captured frames written in out_frames.
func sz stacktrace_capture(stacktrace_frame* out_frames, sz out_capacity, sz skip_frames);

// =========================================================================
c_end;
// =========================================================================
