// MIT License
// Copyright (c) 2026 Christian Luppi

#define ENTRY_TYPE_RUN
#define ENTRY_WINDOWS_CONSOLE
#include <based_core.h>

func b32 run(cmdline cmdl) {
  global_log_info("RUNNING!");
  return true;
}
