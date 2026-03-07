// MIT License
// Copyright (c) 2026 Christian Luppi

#pragma once

/* Define build and runtime environment variables for the project.

Plaform detection variables:
- PLATFORM_WINDOWS: Defined if the target platform is Windows.
- PLATFORM_LINUX: Defined if the target platform is Linux.
- PLATFORM_MACOS: Defined if the target platform is macOS.
- PLATFORM_UNIX: Defined if the target platform is a Unix-like system (Linux or macOS).
- PLATFORM_ANDROID: Defined if the target platform is Android.
- PLATFORM_IOS: Defined if the target platform is iOS.
- PLATFORM_WEB: Defined if the target platform is WebAssembly (WASM).
- PLATFORM_PLAYSTATION: Defined if the target platform is PlayStation.
- PLATFORM_XBOX: Defined if the target platform is Xbox.
- PLATFORM_SWITCH: Defined if the target platform is Nintendo Switch.
- PLATFORM_DESKTOP: Defined if the target platform is a desktop operating system (Windows, Linux, or macOS).
- PLATFORM_MOBILE: Defined if the target platform is a mobile operating system (Android or iOS).
- PLATFORM_CONSOLE: Defined if the target platform is a gaming console (e.g., PlayStation, Xbox, Nintendo Switch).

CPU architecture detection variables:
- ARCH_X86: Defined if the target CPU architecture is x86 (32-bit).
- ARCH_X86_64: Defined if the target CPU architecture is x86-64 (64-bit).
- ARCH_ARM: Defined if the target CPU architecture is ARM (32-bit).
- ARCH_ARM64: Defined if the target CPU architecture is ARM64 (64-bit).
- ARCH_RISCV: Defined if the target CPU architecture is RISC-V.
- ARCH_POWERPC: Defined if the target CPU architecture is PowerPC.
- ARCH_MIPS: Defined if the target CPU architecture is MIPS.
- ARCH_SPARC: Defined if the target CPU architecture is SPARC.
- ARCH_WASM: Defined if the target CPU architecture is WebAssembly (WASM).
- ARCH_64: Defined if the target CPU architecture is 64-bit (x86-64, ARM64, RISC-V 64-bit, etc.).
- ARCH_32: Defined if the target CPU architecture is 32-bit (x86, ARM, etc.).

Compiler detection variables:
- COMPILER_MSVC: Defined if the compiler is Microsoft Visual C++ (MSVC).
- COMPILER_GCC: Defined if the compiler is GNU Compiler Collection (GCC).
- COMPILER_CLANG: Defined if the compiler is Clang.
- COMPILER_INTEL: Defined if the compiler is Intel C++ Compiler.
- COMPILER_APPLE_CLANG: Defined if the compiler is Apple Clang (a variant of Clang used on macOS).
- COMPILER_EMSCRIPTEN: Defined if the compiler is Emscripten (used for WebAssembly).
- COMPILER_NVCC: Defined if the compiler is NVIDIA CUDA Compiler (NVCC).

User-defined build configuration variables:
- BUILD_DEBUG: Defined if the build configuration is Debug.
- BUILD_RELEASE: Defined if the build configuration is Release.
- BUILD_PROFILE: Defined if the build configuration is Profile.
- BUILD_SANITIZE: Defined if the build configuration includes sanitizers (e.g., AddressSanitizer, ThreadSanitizer).
- BUILD_TEST: Defined if the build configuration is for testing (e.g., unit tests, integration tests).

Expected binary-kind definitions:
- BIN_RUNNABLE:    executable target
- BIN_DYNAMIC_LIB: shared/module library target
- BIN_STATIC_LIB:  static library target
- BIN_OBJ_LIB:     object library target

Below you can find the actual definitions: */

// =========================================================================
// Platform Detection
// =========================================================================

#if defined(__EMSCRIPTEN__)
#  define PLATFORM_WEB

#elif defined(__ANDROID__)
#  define PLATFORM_ANDROID

#elif defined(__APPLE__) && defined(__MACH__)
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#    define PLATFORM_IOS
#  else
#    define PLATFORM_MACOS
#  endif

#elif defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#  define PLATFORM_WINDOWS

#elif defined(__linux__)
#  define PLATFORM_LINUX

#elif defined(__ORBIS__) || defined(__PROSPERO__)
#  define PLATFORM_PLAYSTATION

#elif defined(_DURANGO) || defined(__XBOXSERIES__)
#  define PLATFORM_XBOX

#elif defined(__NX__)
#  define PLATFORM_SWITCH
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#  define PLATFORM_UNIX
#endif

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#  define PLATFORM_DESKTOP
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
#  define PLATFORM_MOBILE
#endif

#if defined(PLATFORM_PLAYSTATION) || defined(PLATFORM_XBOX) || defined(PLATFORM_SWITCH)
#  define PLATFORM_CONSOLE
#endif

// =========================================================================
// CPU Architecture Detection
// =========================================================================

#if defined(__wasm64__)
#  define ARCH_WASM
#  define ARCH_64

#elif defined(__wasm__) || defined(__wasm32__)
#  define ARCH_WASM
#  define ARCH_32

#elif defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)
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
// Compiler Detection
// =========================================================================

#if defined(__NVCC__)
#  define COMPILER_NVCC

#elif defined(__EMSCRIPTEN__)
#  define COMPILER_EMSCRIPTEN

#elif defined(__INTEL_COMPILER) || defined(__ICC)
#  define COMPILER_INTEL

#elif defined(__clang__) && defined(__apple_build_version__)
#  define COMPILER_APPLE_CLANG

#elif defined(__clang__)
#  define COMPILER_CLANG

#elif defined(_MSC_VER)
#  define COMPILER_MSVC

#elif defined(__GNUC__)
#  define COMPILER_GCC
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
// Binary Type
// =========================================================================

// These are expected to be provided by the build system.
#if (defined(BIN_RUNNABLE) + defined(BIN_DYNAMIC_LIB) + defined(BIN_STATIC_LIB) + defined(BIN_OBJ_LIB)) == 0
#  error "env_defines.h: no binary kind detected. Define one of BIN_RUNNABLE, BIN_DYNAMIC_LIB, BIN_STATIC_LIB, BIN_OBJ_LIB."
#elif (defined(BIN_RUNNABLE) + defined(BIN_DYNAMIC_LIB) + defined(BIN_STATIC_LIB) + defined(BIN_OBJ_LIB)) > 1
#  error "env_defines.h: multiple binary kinds detected. Define exactly one of BIN_RUNNABLE, BIN_DYNAMIC_LIB, BIN_STATIC_LIB, BIN_OBJ_LIB."
#endif

// =========================================================================
// Sanity Checks
// =========================================================================

// At least one platform must be detected
#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX) &&    \
    !defined(PLATFORM_MACOS) && !defined(PLATFORM_ANDROID) &&    \
    !defined(PLATFORM_IOS) && !defined(PLATFORM_WEB) &&          \
    !defined(PLATFORM_PLAYSTATION) && !defined(PLATFORM_XBOX) && \
    !defined(PLATFORM_SWITCH)
#  error "env_defines.h: no target platform detected."
#endif

// At least one architecture must be detected
#if !defined(ARCH_X86) && !defined(ARCH_X86_64) &&    \
    !defined(ARCH_ARM) && !defined(ARCH_ARM64) &&     \
    !defined(ARCH_RISCV) && !defined(ARCH_POWERPC) && \
    !defined(ARCH_MIPS) && !defined(ARCH_SPARC) &&    \
    !defined(ARCH_WASM)
#  error "env_defines.h: no target CPU architecture detected."
#endif

// At least one compiler must be detected
#if !defined(COMPILER_MSVC) && !defined(COMPILER_GCC) &&               \
    !defined(COMPILER_CLANG) && !defined(COMPILER_INTEL) &&            \
    !defined(COMPILER_APPLE_CLANG) && !defined(COMPILER_EMSCRIPTEN) && \
    !defined(COMPILER_NVCC)
#  error "env_defines.h: no compiler detected."
#endif

// Compiler / Platform consistency
#if defined(COMPILER_MSVC) && !defined(PLATFORM_WINDOWS)
#  error "env_defines.h: COMPILER_MSVC is only supported on PLATFORM_WINDOWS."
#endif
#if defined(COMPILER_APPLE_CLANG) && !defined(PLATFORM_MACOS) && !defined(PLATFORM_IOS)
#  error "env_defines.h: COMPILER_APPLE_CLANG is only supported on PLATFORM_MACOS or PLATFORM_IOS."
#endif
#if defined(COMPILER_EMSCRIPTEN) && !defined(PLATFORM_WEB)
#  error "env_defines.h: COMPILER_EMSCRIPTEN is only supported on PLATFORM_WEB."
#endif

// Architecture / Platform consistency
#if defined(ARCH_WASM) && !defined(PLATFORM_WEB)
#  error "env_defines.h: ARCH_WASM requires PLATFORM_WEB."
#endif
#if defined(PLATFORM_WEB) && !defined(ARCH_WASM)
#  error "env_defines.h: PLATFORM_WEB requires ARCH_WASM."
#endif
