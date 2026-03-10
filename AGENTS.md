# based Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY**

`based` is a cross-platform C23 utility library organized as modules.

This file is the authoritative **project-wide** guide. Keep module-specific guidance in each module's own `AGENTS.md`.

Current module layout:
- `core/` - the core module (see `core/AGENTS.md` for all core-specific rules, API/layout notes, and maintenance requirements). Always read this, since all modules depend on the core module.

If modules are added, removed, or reorganized, update this file in the same change and ensure each affected module has its own up-to-date `AGENTS.md`.

## Project-Wide Rules
- Every `.h`, `.c`, and test file should start with:
  `// MIT License`
  `// Copyright (c) 2026 Christian Luppi`
- Respect `.clang-format` and `.clang-tidy`; warnings are treated as errors.
- Use modular naming in CMake targets to avoid collisions: `based-<module>` (for example `based-core`).
- Keep cross-module build and dependency wiring in top-level CMake files coherent when module structure changes.
- Never use functions from C/C++ standard directly. Always check for functions implemented in based modules. If you don't find the function you need, ask the user.
- Core foundational headers under `core/include/basic/` define shared project language and helpers and should be treated as canonical:
  - `keyword_defines.h` for project keywords/qualifiers. Keep usage aligned with the canonical set:
    `thread_local`, `dll_export`, `dll_import`, `static_assert`, `no_return`, `force_inline`, `no_inline`,
    `align_as(x)`, `align_of(x)`, `size_of(x)`, `likely(x)`, `unlikely(x)`, `read_only`, `c_begin`, `c_end`,
    `local_persist`, `const_var`, `global_var`, `func`.
  - `codespace.h` for source-location plumbing. Keep wrappers aligned with:
    `callsite`, `src_loc`, `CALLSITE_HERE`, `SRC_LOC_HERE`.
  - `utility_defines.h` for common utility macros. Keep helpers aligned with:
    `strfy`, `strfy_exp`, `cat`, `cat_exp`, `expr`, `stmt`, `count_of`, `size_of_each`, `multiline_literal`,
    `bit`, `bit64`, `bit_is_set`, `bit_set`, `bit_unset`, `bit_toggle`, `field_of`, `size_of_field`,
    `offset_of`, `container_of`, `in_range`, `in_bounds`, `swap`, `refswap`, `is_pow2`, `align_up`,
    `align_down`, `kb`, `mb`, `gb`, `tb`, `th`, `mil`, `bil`, `tril`.

## Profiling Policy

- When implementing new functions, always add Tracy function scopes with `profile_func_begin` and `profile_func_end`.
- You may also use other Tracy functions/macros where appropriate, following existing project patterns.
- For simple/straight getter functions, profiling scopes can be skipped.

## Logging Policy

- When writing new code, add logging where it helps surface failures, important state changes, unusual paths, and useful execution flow.
- Prefer the logging wrappers/macros that preserve callsite information instead of calling `_log` directly.
- Prefer `thread_log_*` over `global_log_*` unless the log must explicitly target process-global state.
- Be generous with `thread_log_verbose` and `thread_log_trace`; detailed step-by-step logging is encouraged, and there should often be a lot of it in multi-step flows and tricky paths.
- Use `log_state_*` when working with an explicit `log_state*`.
- Use `thread_log_*` for the current thread's effective log state.
- Use `global_log_*` for the process-global log state.
- Format log messages cleanly: start with a capital letter and do not end with a full stop.
- Severity guidance:
  - `thread_log_fatal` / `global_log_fatal`: unrecoverable invariant violations or conditions that should immediately assert/abort.
  - `thread_log_error` / `global_log_error`: real failures that should be fixed, where the operation cannot complete correctly.
  - `thread_log_warn` / `global_log_warn`: unexpected or degraded situations that are recoverable but likely need attention.
  - `thread_log_info` / `global_log_info`: important lifecycle/state transitions and user-relevant milestones.
  - `thread_log_debug` / `global_log_debug`: developer-focused state, branch decisions, sizes/counts, and diagnostics useful during debugging.
  - `thread_log_verbose` / `global_log_verbose`: more detailed step-by-step diagnostics that are noisier than normal debug logs.
  - `thread_log_trace` / `global_log_trace`: very fine-grained tracing for hot paths, frequent operations, or detailed flow tracking.
- Keep usage aligned with `core/include/utils/log_state.h`, `core/include/context/thread_ctx.h`, and `core/include/context/global_ctx.h`.

## Global Vs Thread Context

- `ctx` is the shared payload type; `global_ctx` wraps one process-global shared `ctx`, while `thread_ctx` exposes one `ctx` per thread.
- Prefer `thread_ctx`/`thread_*` helpers for thread-affine work, temporary allocations, and per-thread user data.
- Prefer `global_ctx`/`global_*` helpers for process-wide shared state and cross-thread shared resources.
- When doing coordinated multi-step access to the shared global context, use `global_ctx_lock()` / `global_ctx_unlock()`.
- Remember that `thread_get_log_state()` falls back to the global log state when the current thread context is not initialized.

## Project Layout

```text
cmake/                       # CMake helper modules (dependency fetch/setup helpers)
core/                        # Core module
  AGENTS.md                  # Core-module specific coding and layout guide
  CMakeLists.txt             # Core-module build targets
  include/                   # Core public headers
    basic/                   # Foundational keywords, callsite, utility macros
  src/                       # Core implementations
  tests/                     # Core tests
  utils/                     # Core utility work areas and in-progress submodules
    digits/                  # Digits utility module workspace
    random_series/           # Random-series utility module workspace
  api_index.txt              # Core public API index
CMakeLists.txt               # Top-level project configuration and module wiring
```

Keep this tree accurate as project-level structure evolves.
