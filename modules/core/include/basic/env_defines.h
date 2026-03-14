// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

// =========================================================================
// Platform Detection
// =========================================================================

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#  define PLATFORM_WINDOWS
#elif defined(__linux__)
#  define PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#  define PLATFORM_MACOS
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#  define PLATFORM_UNIX
#endif

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#  define PLATFORM_DESKTOP
#endif

// =========================================================================
// CPU Architecture Detection
// =========================================================================

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)
#  define ARCH_X86_64
#  define ARCH_64
#elif defined(_M_IX86) || defined(__i386__)
#  define ARCH_X86
#  define ARCH_32
#elif defined(_M_ARM64) || defined(__aarch64__)
#  define ARCH_ARM64
#  define ARCH_64
#elif defined(_M_ARM) || defined(__arm__)
#  define ARCH_ARM
#  define ARCH_32
#elif defined(__riscv)
#  define ARCH_RISCV
#  if defined(__riscv_xlen) && __riscv_xlen == 64
#    define ARCH_64
#  else
#    define ARCH_32
#  endif
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(_M_PPC) && defined(__64BIT__)
#  define ARCH_POWERPC
#  define ARCH_64
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
#  define ARCH_POWERPC
#  define ARCH_32
#elif defined(__mips64)
#  define ARCH_MIPS
#  define ARCH_64
#elif defined(__mips__) || defined(__mips)
#  define ARCH_MIPS
#  define ARCH_32
#elif defined(__sparc_v9__) || defined(__sparcv9)
#  define ARCH_SPARC
#  define ARCH_64
#elif defined(__sparc__) || defined(__sparc)
#  define ARCH_SPARC
#  define ARCH_32
#endif

// =========================================================================
// Build Configuration
// =========================================================================

#if !defined(BUILD_DEBUG) && !defined(BUILD_RELEASE) && \
    !defined(BUILD_PROFILE) && !defined(BUILD_TEST) &&  \
    !defined(BUILD_SANITIZE)
#  if defined(NDEBUG) && !defined(_DEBUG) && !defined(DEBUG)
#    define BUILD_RELEASE
#  elif defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG) || defined(__debug)
#    define BUILD_DEBUG
#  elif !defined(NDEBUG)
#    define BUILD_DEBUG
#  else
#    define BUILD_RELEASE
#  endif
#endif

// =========================================================================
// Sanity Checks
// =========================================================================

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX) && !defined(PLATFORM_MACOS)
#  error "env_defines.h: no supported target platform detected."
#endif

#if !defined(ARCH_X86) && !defined(ARCH_X86_64) &&    \
    !defined(ARCH_ARM) && !defined(ARCH_ARM64) &&     \
    !defined(ARCH_RISCV) && !defined(ARCH_POWERPC) && \
    !defined(ARCH_MIPS) && !defined(ARCH_SPARC)
#  error "env_defines.h: no target CPU architecture detected."
#endif

#if !defined(__clang__)
#  error "env_defines.h: based currently supports only Clang-based compilers."
#endif