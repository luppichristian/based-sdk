// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "primitive_types.h"

// =========================================================================
c_begin;
// =========================================================================

// Installs platform crash handlers that route unhandled faults/exceptions
// through the assert system before terminating the process.
// Safe to call multiple times; repeated calls keep the handler installed.
func b32 crash_install(void);

// Removes the currently installed platform crash handlers.
// Safe to call even when the handler is not installed.
func void crash_uninstall(void);

// Returns 1 when the crash handler is currently installed, 0 otherwise.
func b32 crash_is_installed(void);

// =========================================================================
c_end;
// =========================================================================
