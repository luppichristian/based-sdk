// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "process.h"

// =========================================================================
c_begin;
// =========================================================================

// Opens the provided web URL in the default browser.
func b32 process_open_weblink(cstr8 url);

// Opens a file-manager window at location. If location is NULL, uses platform default.
func b32 process_open_file_window(cstr8 location);

// Opens a terminal shell rooted at location. If location is NULL, uses the current directory.
func b32 process_open_terminal(cstr8 location);

// =========================================================================
c_end;
// =========================================================================
