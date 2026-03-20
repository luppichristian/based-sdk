# based gfx Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY**

This file is the authoritative guide for the `gfx` module.

For project-wide policies and module organization rules, see `AGENTS.md` at repo root.

## Gfx Rules
- Use `#pragma once` in headers.
- Use project primitive types (`i32`, `u64`, `u64x`, `sz`, `b32`, `c8`, `b32x`, etc.) instead of raw C types.
- Prefer project qualifiers and keywords (`func`, `global_var`, `local_persist`, `force_inline`, `no_inline`, `no_return`).
- Do not use the `static` keyword directly for functions; use `func`.
- Naming is snake_case for functions, variables, typedefs, structs, and enums; macros/constants are UPPER_CASE.
- Do not introduce project-defined `_t` types.
- Variable and parameter names should generally be at least 3 characters.
- Keep `include/based_gfx.h` up to date when creating/changing public gfx headers.
- Keep `api_index.txt` up to date whenever public API signatures/macros/headers change.
- Keep `DOC.md` up to date whenever public API surface changes. `DOC.md` must include a brief module description and a categorized table of all public gfx types/functions/macros, and it must reference `./api_index.txt` as the canonical API index.
- Build gfx features on top of `core`; prefer using `based-core` types, helpers, allocators, logging, and profiling wrappers rather than reintroducing foundational utilities locally.

## Gfx Profiling Policy

- When implementing new functions in `src/`, add Tracy function scopes with `profile_func_begin` and `profile_func_end`.
- You may also use other Tracy functions/macros where appropriate, following existing project patterns.
- For simple/straight getter functions, profiling scopes can be skipped.
- Every started Tracy zone must end exactly once on all paths.

## Gfx Logging Policy

- When writing new code, add logging where it helps surface failures, important state changes, unusual paths, and useful execution flow.
- Prefer the logging wrappers/macros that preserve callsite information instead of calling `_log` directly.
- Prefer `thread_log_*` over `global_log_*` unless the log must explicitly target process-global state.
- Be generous with `thread_log_verbose` and `thread_log_trace`; detailed step-by-step logging is encouraged, especially for backend setup, device discovery, resource lifetime, and synchronization paths.
- Format log messages cleanly: start with a capital letter and do not end with a full stop.

## Gfx Layout

```text
CMakeLists.txt               # Gfx module build targets
api_index.txt                # Gfx public API index
DOC.md                       # Gfx module/API docs (references ./api_index.txt)
include/                     # Gfx public headers
  based_gfx.h                # Gfx umbrella header
  gfx/                       # Gfx public API groups
src/                         # Gfx implementations (mirrors include/ groups)
tests/                       # Gfx GoogleTest coverage (mirrors include/ groups)
utils/                       # Gfx utility work areas for backends and scaffolding
```

Keep this tree accurate as gfx evolves.
