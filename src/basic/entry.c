// MIT License
// Copyright (c) 2026 Christian Luppi

#include "basic/entry.h"

func b32 entry_init(cmdline cmdline) {
  return true;  // TODO: Properly implement
}

func void entry_quit(void) {
  // TODO: Properly implement
}

#if defined(ENTRY_TYPE_APP)

// TODO: Will implement this later...

#elif defined(ENTRY_TYPE_MAIN)

int main(int argc, char** argv) {
  cmdline cmdl = cmdline_make(argc, argv);
  if (!entry_init(cmdl)) {
    return EXIT_FAILURE;
  }

  atexit(entry_quit);
  return EXIT_SUCCESS;
}

#endif