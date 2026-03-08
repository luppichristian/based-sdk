# based core Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY**

This file is the authoritative guide for the `core` module.

For project-wide policies and module organization rules, see `AGENTS.md` at repo root.

## Core Rules
- Use `#pragma once` in headers.
- Use project primitive types (`i32`, `u64`, `sz`, `b32`, `c8`, etc.) instead of raw C types.
- Prefer project qualifiers and keywords (`func`, `global_var`, `local_persist`, `force_inline`, `no_inline`, `no_return`, `read_only`, `const_var`).
- Do not use the `static` keyword directly for functions; use `func`.
- Naming is snake_case for functions, variables, typedefs, structs, and enums; macros/constants are UPPER_CASE.
- Do not introduce project-defined `_t` types.
- Variable and parameter names should generally be at least 3 characters.
- Functions starting with `_` should be called through their convenience macro so callsite info is preserved (see `include/basic/codespace.h` and `CALLSITE_HERE`).
- Keep `include/based_core.h` up to date when creating/changing public core headers.
- Keep `api_index.txt` up to date whenever public API signatures/macros/headers change.
- Every implemented function in `src/` must integrate Tracy profiling following existing patterns and must end every started zone exactly once on all paths.

## Core Layout

```text
CMakeLists.txt               # Core module build targets
api_index.txt                # Core public API index
include/                     # Core public headers
  based_core.h               # Core umbrella header
  basic/                     # Primitive types, keywords, assert/log, profiler, intrinsics
  context/                   # Global/thread/shared context APIs
  containers/                # Intrusive containers and sorting
  memory/                    # Allocators and memory utilities
  filesystem/                # Path/filesystem/file stream/module APIs
  input/                     # Input APIs and message/event abstractions
  windowing/                 # Monitor/window/dialog wrappers
  processes/                 # Process and pipe APIs
  system/                    # CPU/system/runtime information APIs
  strings/                   # Character/string/unicode APIs
  utils/                     # Utility helpers (cmdline, crc, timer, uuid, etc.)
  threads/                   # Threading/synchronization/atomics
src/                         # Core implementations (mirrors include/ module folders)
tests/                       # Core GoogleTest coverage (mirrors include/ module folders)
```

Keep this tree accurate as core evolves.

## Core Keywords Sheet

- `func`: function declaration alias; becomes `static` when `ALL_FUNCS_STATIC` is defined.
- `global_var`: marker for globals; becomes `static` when `ALL_GLOBAL_VARS_STATIC` is defined.
- `const_var`: marker for header-safe constants using `read_only` internally.
- `local_persist`: alias for local `static` storage.
- `read_only`: readonly storage qualifier (`const` + readonly section placement when supported).
- `thread_local`: thread-local storage qualifier.
- `dll_export` / `dll_import`: visibility markers for shared-library symbols.
- `static_assert(expr)`: compile-time assertion wrapper for compatible modes.
- `no_return`, `force_inline`, `no_inline`, `align_as(x)`, `align_of(x)`, `size_of(x)`, `likely(x)`, `unlikely(x)`.

Common macros used across core:
- `CALLSITE_HERE`.
- `assert(condition)` forwarding through `_assert(..., CALLSITE_HERE)`.
- `log_state_*` and `global_log_*` logging wrappers forwarding callsite info.
- `profiler_zone_begin`, `profiler_zone_end`, `profiler_frame_mark`, `profiler_frame_mark_named`.
- Utility macros from `include/basic/utility_defines.h` (`strfy`, `cat`, `count_of`, `bit`, `align_up`, `swap`, etc.).
