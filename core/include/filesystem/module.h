// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "path.h"

// Signature expected from module entry points defined by basic/entry.h.
typedef b32 mod_init_func(void);
typedef void mod_quit_func(void);

typedef struct mod {
  void* native_handle;
  mod_init_func* init_func;
  mod_quit_func* quit_func;
  path source_path;
  b32 initialized;
} mod;

// Loads src as a dynamic module and calls mod_init(). Returns an empty
// module on failure.
func mod mod_open(const path* src);

// Returns 1 when mod currently references a loaded module.
func b32 mod_is_open(const mod* mod_ptr);

// Returns the named symbol as a generic function pointer, or NULL on failure.
func void* mod_get_func(const mod* mod_ptr, cstr8 name);

// Returns the platform-specific dynamic-library extension.
func cstr8 mod_get_extension(void);

// Calls mod_quit() when needed, unloads the module, and resets mod.
func void mod_close(mod* mod_ptr);