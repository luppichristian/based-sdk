// MIT License
// Copyright (c) 2026 Christian Luppi

#include "../utils/cmdline.h"
#include "primitive_types.h"

#if defined(ENTRY_TYPE_APP)

// This follows's SDL3 conventions for the entry point, which is cross-platform and supports Unicode command lines.

// TODO: Will implement this later...

#elif defined(ENTRY_TYPE_MAIN)

// Standard C/C++ entry point.
// The main function is called with the command line arguments.

// Returns 1 on success, 0 on failure.
func b32 main(cmdline cmdl);

#elif defined(ENTRY_TYPE_MODULE)

// A module is essentially a DLL with a standardized entry point.
// The module_init function is called when the module is loaded,
// and module_quit is called when the module is unloaded.
// This is because there's a custom module system.

// Initializes the module. Returns 1 on success, 0 on failure.
func dll_export b32 module_init(void);

// Quits the module, its fine to call even if init failed.
func dll_export void module_quit(void);

#endif

// The following function must be implemented if you use a custom
// entry point (none of the above).
// All above entry points call this internally.

// Initializes the program. Returns 1 on success, 0 on failure.
func b32 entry_init(cmdline cmdline);

// Quits the program, its fine to call even if init failed.
func void entry_quit(void);