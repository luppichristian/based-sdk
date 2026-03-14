// MIT License
// Copyright (c) 2026 Christian Luppi

#define ENTRY_TYPE_MOD
#include <based_core.h>

func dll_export b32 mod_init(void) {
  return true;
}

func dll_export void mod_quit(void) {
}
