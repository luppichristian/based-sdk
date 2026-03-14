# based core Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY**

This file is the authoritative guide for the `core` module.

For project-wide policies and module organization rules, see `AGENTS.md` at repo root.

## Core Rules
- Use `#pragma once` in headers.
- Use project primitive types (`i32`, `u64`, `u64x`, `sz`, `b32`, `c8`, `b32x`,  etc.) instead of raw C types.
- Prefer project qualifiers and keywords (`func`, `global_var`, `local_persist`, `force_inline`, `no_inline`, `no_return`).
- Do not use the `static` keyword directly for functions; use `func`.
- Naming is snake_case for functions, variables, typedefs, structs, and enums; macros/constants are UPPER_CASE.
- Do not introduce project-defined `_t` types.
- Variable and parameter names should generally be at least 3 characters.
- Functions starting with `_` should be called through their convenience macro so callsite info is preserved (see `include/basic/codespace.h` and `CALLSITE_HERE`).
- Keep `include/based_core.h` up to date when creating/changing public core headers.
- Keep `api_index.txt` up to date whenever public API signatures/macros/headers change.
- `include/basic/safe.h` provides `safe_for` and `safe_while`; use them for fixed or regular loops, and do not convert game loops.

## Core Profiling Policy

- When implementing new functions in `src/`, add Tracy function scopes with `profile_func_begin` and `profile_func_end`.
- You may also use other Tracy functions/macros where appropriate, following existing project patterns.
- For simple/straight getter functions, profiling scopes can be skipped.
- Every started Tracy zone must end exactly once on all paths.

## Core Logging Policy

- When writing new code, add logging where it helps surface failures, important state changes, unusual paths, and useful execution flow.
- Prefer the logging wrappers/macros that preserve callsite information instead of calling `_log` directly.
- Prefer `thread_log_*` over `global_log_*` unless the log must explicitly target process-global state.
- Be generous with `thread_log_verbose` and `thread_log_trace`; detailed step-by-step logging is encouraged, and there should often be a lot of it in multi-step flows and tricky paths.
- Use `log_state_*` when operating on an explicit `log_state*`.
- Use `thread_log_*` when logging against the current thread's effective log state.
- Use `global_log_*` when logging against the process-global log state.
- Format log messages cleanly: start with a capital letter and do not end with a full stop.
- Severity guidance:
  - `thread_log_fatal` / `global_log_fatal`: unrecoverable invariant violations or conditions that should immediately assert/abort.
  - `thread_log_error` / `global_log_error`: real failures that should be fixed, where the operation cannot complete correctly.
  - `thread_log_warn` / `global_log_warn`: unexpected or degraded situations that are recoverable but likely need attention.
  - `thread_log_info` / `global_log_info`: important lifecycle/state transitions and user-relevant milestones.
  - `thread_log_debug` / `global_log_debug`: developer-focused state, branch decisions, sizes/counts, and diagnostics useful during debugging.
  - `thread_log_verbose` / `global_log_verbose`: more detailed step-by-step diagnostics that are noisier than normal debug logs.
  - `thread_log_trace` / `global_log_trace`: very fine-grained tracing for hot paths, frequent operations, or detailed flow tracking.
- When you need structured log collection, use `log_state_begin_frame` / `log_state_end_frame` or the matching thread/global wrappers.

## Core Global Vs Thread Context

- `ctx` is the shared payload type used by both wrappers.
- `thread_ctx` is thread-local: each thread must initialize its own context with `thread_ctx_init(...)` before relying on thread-owned allocators, user data, or thread-local state.
- `global_ctx` is process-global shared state and exposes convenience wrappers through `global_*` helpers.
- Prefer `thread_*` helpers for thread-affine allocators, temporary memory, and per-thread user data.
- Prefer `global_*` helpers for shared resources that must outlive or span threads.
- Use `global_ctx_lock()` / `global_ctx_unlock()` for coordinated multi-step access to `global_ctx_get_shared()`.
- Remember that `thread_get_log_state()` falls back to the global log state when the current thread has no initialized context.

## Core Layout

```text
CMakeLists.txt               # Core module build targets
api_index.txt                # Core public API index
include/                     # Core public headers
  based_core.h               # Core umbrella header
  basic/                     # Primitive types, keywords, assert, profiler, intrinsics
  context/                   # Global/thread/shared context APIs
  containers/                # Intrusive containers and sorting
  memory/                    # Allocators and memory utilities
  filesystem/                # Path/filesystem/file stream/module APIs
  input/                     # Input APIs and message/event abstractions
  interface/                 # Monitor/window/dialog wrappers
  processes/                 # Process and pipe APIs
  system/                    # CPU/system/runtime information APIs
  strings/                   # Character/string/unicode APIs
  utils/                     # Utility helpers (cmdline, crc, log state, timer, uuid, etc.)
  threads/                   # Threading/synchronization/atomics
src/                         # Core implementations (mirrors include/ module folders)
tests/                       # Core GoogleTest coverage (mirrors include/ module folders)
utils/                       # Core utility work areas for new submodules and scaffolding
  digits/                    # Digits utility module workspace
  random_series/             # Random-series utility module workspace
```

Keep this tree accurate as core evolves.

## Related Modules

- `../gfx/` is the sibling gfx module that builds on top of core.
- Keep shared foundational facilities in `core`; higher-level rendering and backend-facing APIs should live in `gfx` unless they are broadly reusable across all modules.

## Core Keywords Sheet

- `func`: function declaration alias; becomes `static` when `ALL_FUNCS_STATIC` is defined.
- `global_var`: marker for globals; becomes `static` when `ALL_GLOBAL_VARS_STATIC` is defined.
- `local_persist`: alias for local `static` storage.
- `thread_local`: thread-local storage qualifier.
- `dll_export` / `dll_import`: visibility markers for shared-library symbols.
- `static_assert(expr)`: compile-time assertion wrapper for compatible modes.
- `no_return`, `force_inline`, `no_inline`, `align_as(x)`, `align_of(x)`, `size_of(x)`, `type_of(x)`, `likely(x)`, `unlikely(x)`.

Common macros used across core:
- `CALLSITE_HERE`.
- `assert(condition)` forwarding through `_assert(..., CALLSITE_HERE)`.
- `log_state_*` and `global_log_*` logging wrappers forwarding callsite info.
- `profile_func_begin`, `profile_func_end`.
- Utility macros from `include/basic/utility_defines.h` (`strfy`, `cat`, `count_of`, `bit`, `align_up`, `swap`, etc.).
