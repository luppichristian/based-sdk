// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// Wraps a UTF-8 argc/argv pair from a standard main-style entry point.
typedef struct cmdline {
  sz count;
  c8** args;
} cmdline;

// Normalizes argc/argv into a command-line view. Negative counts or NULL argv yield an empty view.
func cmdline cmdline_build(sz count, c8** args);

// Returns the number of entries in the command line.
func sz cmdline_get_count(cmdline cmdl);

// Returns 1 if the command line contains no entries, 0 otherwise.
func b32 cmdline_is_empty(cmdline cmdl);

// Returns argv[idx], or NULL if idx is out of range.
func cstr8 cmdline_get_arg(cmdline cmdl, sz idx);

// Returns argv[0], or NULL if no entries are present.
func cstr8 cmdline_get_program(cmdline cmdl);

// Searches for an exact argument match. Writes the idx on success when out_idx is non-NULL.
func b32 cmdline_find(cmdline cmdl, cstr8 arg, sz* out_idx);

// Returns 1 if any argument exactly matches arg, 0 otherwise.
func b32 cmdline_has(cmdline cmdl, cstr8 arg);

// Looks up an option by name, supporting both "--name value" and "--name=value".
// Returns the option value, or NULL if the option is missing or has no value.
func cstr8 cmdline_get_option(cmdline cmdl, cstr8 name);

// Parses an option value as a base-10 signed integer. Returns 1 on success.
func b32 cmdline_get_option_i64(cmdline cmdl, cstr8 name, i64* out);

// Parses an option value as a floating-point number. Returns 1 on success.
func b32 cmdline_get_option_f64(cmdline cmdl, cstr8 name, f64* out);

// =========================================================================
c_end;
// =========================================================================
