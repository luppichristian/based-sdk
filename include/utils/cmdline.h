// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "../basic/primitive_types.h"

// Wraps a UTF-8 argc/argv pair from a standard main-style entry point.
typedef struct cmdline {
  sz count;
  c8** args;
} cmdline;

// Normalizes argc/argv into a command-line view. Negative counts or NULL argv yield an empty view.
func cmdline cmdline_make(sz count, c8** args);

// Returns the number of entries in the command line.
func sz cmdline_count(cmdline cmdl);

// Returns 1 if the command line contains no entries, 0 otherwise.
func b32 cmdline_is_empty(cmdline cmdl);

// Returns argv[index], or NULL if index is out of range.
func const c8* cmdline_arg(cmdline cmdl, sz index);

// Returns argv[0], or NULL if no entries are present.
func const c8* cmdline_program(cmdline cmdl);

// Searches for an exact argument match. Writes the index on success when out_index is non-NULL.
func b32 cmdline_find(cmdline cmdl, const c8* arg, sz* out_index);

// Returns 1 if any argument exactly matches arg, 0 otherwise.
func b32 cmdline_has(cmdline cmdl, const c8* arg);

// Looks up an option by name, supporting both "--name value" and "--name=value".
// Returns the option value, or NULL if the option is missing or has no value.
func const c8* cmdline_get_option(cmdline cmdl, const c8* name);

// Parses an option value as a base-10 signed integer. Returns 1 on success.
func b32 cmdline_get_option_i64(cmdline cmdl, const c8* name, i64* out);

// Parses an option value as a floating-point number. Returns 1 on success.
func b32 cmdline_get_option_f64(cmdline cmdl, const c8* name, f64* out);
