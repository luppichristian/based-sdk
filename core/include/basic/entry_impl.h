// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

#include "entry.h"

// This is an ugly internal header where we implement the C main entry point...

// Implement the actual entry point.
#if defined(ENTRY_FUNCTION_NAME) && !defined(ENTRY_HANDLED) && !defined(ENTRY_NOIMPL)

#  ifndef ENTRY_IMPL
#    define ENTRY_IMPL

// Runtime entry implementation this wrapper dispatches to.
#    ifdef __cplusplus
extern "C" int ENTRY_FUNCTION_NAME(int argc, char** argv, allocator alloc, ENTRY_CALLBACKS_TYPE callbacks);
#    else
int ENTRY_FUNCTION_NAME(int argc, char** argv, allocator alloc, ENTRY_CALLBACKS_TYPE callbacks);
#    endif

// Single bootstrap macro so all platform entry variants follow the same path:
// - resolve allocator override
// - materialize compile-time callback bundle
// - call runtime entry implementation
#    define ENTRY_BOOT(argc, argv) ENTRY_FUNCTION_NAME((argc), (argv), ENTRY_GET_GLOBAL_ALLOCATOR(), ENTRY_CALLBACKS_INIT())

#    if defined(PLATFORM_WINDOWS) && !defined(ENTRY_WINDOWS_CONSOLE)

#      ifndef WINAPI
#        define WINAPI __stdcall
#      endif

typedef struct HINSTANCE__* HINSTANCE;
typedef char* LPSTR;
typedef wchar_t* PWSTR;

#      if defined(_MSC_VER)
#        if defined(UNICODE) && UNICODE
// MSVC Unicode console entry.
int wmain(int argc, wchar_t** wargv, wchar_t** wenvp) {
  (void)argc;
  (void)wargv;
  (void)wenvp;
  return ENTRY_BOOT(0, NULL);
}

#        else
// MSVC narrow console entry.
int main(int argc, char** argv) {
  return ENTRY_BOOT(argc, argv);
}

#        endif
#      endif

#      ifdef __cplusplus
extern "C" {
#      endif

#      if defined(UNICODE) && UNICODE
// Windows subsystem Unicode entry.
int WINAPI wWinMain(HINSTANCE hinst, HINSTANCE hprev, PWSTR cmdline, int show_cmd) {
#      else
// Windows subsystem narrow entry.
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR cmdline, int show_cmd) {
#      endif
  (void)hinst;
  (void)hprev;
  (void)cmdline;
  (void)show_cmd;
  return ENTRY_BOOT(0, NULL);
}

#      ifdef __cplusplus
}
#      endif

#    else

// Non-Windows (or console-forced) entry.
int main(int argc, char** argv) {
  return ENTRY_BOOT(argc, argv);
}

#    endif

#    undef ENTRY_BOOT
#  endif
#endif
