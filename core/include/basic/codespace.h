// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "primitive_types.h"

// =========================================================================
// Callsite
// =========================================================================

// Represent a callsite in the source code (filename, function name, and line number).
typedef struct callsite {
  cstr8 filename;
  cstr8 function;
  u32 line;
} callsite;

// Macro to capture the current source location as a callsite struct.
#define CALLSITE_HERE ((callsite) {__FILE__, __FUNCTION__, __LINE__})

// =========================================================================
// Source Location
// =========================================================================

// Represent a source code location (filename and line number).
typedef struct src_loc {
  cstr8 filename;
  u32 line;
} src_loc;