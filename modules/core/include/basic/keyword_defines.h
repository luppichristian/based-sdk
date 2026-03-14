// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// Required for compiler detection and platform-specific definitions.
#include "env_defines.h"

// Include standard boolean type for 'bool', 'true', and 'false'.
#include <assert.h>
#include <stdbool.h>

/*
Define custom keywords used in the project.
I like custom keywords, they make the code more readable to me, you can 100% tell
what they do even if they are not standard C keywords.
Some compilers/platforms have different names for the same thing,
so I want to have a single place where I can define them and use them throughout the project.

Functional keywords:
- 'thread_local' for thread-local storage.
- 'dll_export' for exporting symbols from a shared library.
- 'dll_import' for importing symbols from a shared library.
- 'static_assert' for compile-time assertions.
- 'no_return' for functions that do not return to the caller.
- 'force_inline' for suggesting the compiler to inline a function.
- 'no_inline' for suggesting the compiler not to inline a function.
- 'align_as' for specifying the alignment of a variable or type.
- 'align_of' for querying the alignment requirement of a type.
- 'size_of' alias for sizeof, but as a keyword for consistency with align_of.
- 'type_of' alias for compiler-supported typeof.
- 'likely' and 'unlikely' for branch prediction hints to the compiler.
- 'c_begin' use to mark the start of a based header.
- 'c_end' use to mark the end of a based header.

Purely cosmetic keywords:
- 'func' as a shorthand for 'function' to improve readability.
- 'global_var' for global variables to make it clear they are global.
- 'local_persist' for static variables that should persist across function calls.

If ALL_FUNCS_STATIC is defined, all functions will be declared as static.
If ALL_GLOBAL_VARS_STATIC is defined, all global variables will be declared as static.
*/

// =========================================================================
// Functional Keywords
// =========================================================================

// thread_local — thread-local storage duration.
#if defined(__cplusplus) && __cplusplus >= 201103L
// C++11 keyword — already defined.
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define thread_local _Thread_local
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define thread_local __thread
#else
#  define thread_local
#endif

// dll_export / dll_import — symbol visibility for shared libraries.
#if defined(PLATFORM_WINDOWS)
#  define dll_export __declspec(dllexport)
#  define dll_import __declspec(dllimport)
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define dll_export __attribute__((visibility("default")))
#  define dll_import
#else
#  define dll_export
#  define dll_import
#endif

// static_assert — compile-time assertion with an auto-generated message.
// #undef first so any prior definition from <assert.h> doesn't block ours.
#if !(defined(__cplusplus) && __cplusplus >= 201103L)
#  ifdef static_assert
#    undef static_assert
#  endif
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#    define static_assert(expr) _Static_assert(expr, #expr)
#  else
#    define static_assert(expr) typedef char static_assert_##__COUNTER__[(expr) ? 1 : -1]
#  endif
#endif

// no_return — function does not return to the caller.
#if defined(__cplusplus) && __cplusplus >= 201103L
#  define no_return [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define no_return _Noreturn
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define no_return __attribute__((noreturn))
#else
#  define no_return
#endif

// force_inline — ask the compiler to always inline this function.
#if defined(COMPILER_INTEL)
#  define force_inline __forceinline
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)
#  define force_inline __attribute__((always_inline)) inline
#else
#  define force_inline inline
#endif

// no_inline — ask the compiler never to inline this function.
#if defined(COMPILER_INTEL)
#  define no_inline __declspec(noinline)
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG)
#  define no_inline __attribute__((noinline))
#else
#  define no_inline
#endif

// align_as — specify the alignment of a variable or type.
#if defined(__cplusplus) && __cplusplus >= 201103L
#  define align_as(x) alignas(x)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define align_as(x) _Alignas(x)
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define align_as(x) __attribute__((aligned(x)))
#else
#  define align_as(x)
#endif

// align_of — query the alignment requirement of a type.
#if defined(__cplusplus) && __cplusplus >= 201103L
#  define align_of(x) alignof(x)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define align_of(x) _Alignof(x)
#elif defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define align_of(x) __alignof__(x)
#else
#  define align_of(x) sizeof(x)
#endif

// size_of — alias for sizeof, but as a keyword for consistency with align_of.
#define size_of(x) sizeof(x)

// type_of — alias for compiler-supported typeof.
#if defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define type_of(x) typeof(x)
#else
#  error "keyword_defines.h: type_of requires compiler typeof support."
#endif

// likely / unlikely — branch prediction hints.
#if defined(COMPILER_CLANG) || defined(COMPILER_APPLE_CLANG) || defined(COMPILER_INTEL)
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

// c_begin & c_end - this can be used in headers to make them C++ compatible.
// The only exeption is that the actual C main entry point main(int, char**) must be called outside
// this block no matter what.
#ifdef __cplusplus
#  define c_begin extern "C" {
#  define c_end   }
#else
#  define c_begin
#  define c_end
#endif

// =========================================================================
// Cosmetic Keywords
// =========================================================================

// local_persist — static variable that retains its value across calls.
#define local_persist static

// global_var — global variable (purely cosmetic, improves readability).
#if defined(ALL_GLOBAL_VARS_STATIC)
#  define global_var static
#else
#  define global_var
#endif

// func — function declaration; static if ALL_FUNCS_STATIC is defined.
#if defined(ALL_FUNCS_STATIC)
#  define func static
#else
#  define func
#endif
