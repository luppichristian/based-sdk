# based Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY**

`based` is a cross-platform C23 utility library organized as modules.

This file is the authoritative **project-wide** guide. Keep module-specific guidance in each module's own `AGENTS.md`.

Current module layout:
- `modules/core/` - the core module (see `modules/core/AGENTS.md` for core-specific rules and `modules/core/DOC.md` for user-facing module/API docs). Always read this, since all modules depend on the core module.
- `modules/gfx/` - the gfx module (see `modules/gfx/AGENTS.md` for gfx-specific rules and `modules/gfx/DOC.md` for user-facing module/API docs). This module builds on top of `core` and is the home for rendering, graphics device, and backend-facing graphics work.

If modules are added, removed, or reorganized, update this file in the same change and ensure each affected module has its own up-to-date `AGENTS.md`.

## Project-Wide Rules
- Every `.h`, `.c`, and test file should start with:
  `// MIT License`
  `// Copyright (c) 2026 Christian Luppi`
- Respect `.clang-format` and `.clang-tidy`; warnings are treated as errors.
- Use modular naming in CMake targets to avoid collisions: `based-<module>` (for example `based-core`).
- Keep cross-module build and dependency wiring in top-level CMake files coherent when module structure changes.
- Keep `THIRD_PARTY.md` up to date whenever dependencies in `third_party/` are added, removed, renamed, re-versioned, or have license metadata changes.
- Keep root user docs `README.md`, `HOW_TO_BUILD.md`, and `HOW_TO_USE_SDK.md` up to date whenever build flow, module layout, SDK packaging/integration, or onboarding steps change.
- Keep each main module `DOC.md` up to date (`modules/core/DOC.md`, `modules/gfx/DOC.md`). These docs must include a brief module description and a categorized table of all public types/functions/macros, with `./api_index.txt` as the canonical relative API reference.
- Never use functions from C/C++ standard directly. Always check for functions implemented in based modules. If you don't find the function you need, ask the user.
- Core foundational headers under `modules/core/include/basic/` define shared project language and helpers and should be treated as canonical:
  - `keyword_defines.h` for project keywords/qualifiers. Keep usage aligned with the canonical set:
    `thread_local`, `dll_export`, `dll_import`, `static_assert`, `no_return`, `force_inline`, `no_inline`,
    `align_as(x)`, `align_of(x)`, `size_of(x)`, `type_of(x)`, `likely(x)`, `unlikely(x)`, `c_begin`, `c_end`,
    `local_persist`, `global_var`, `func`.
  - `codespace.h` for source-location plumbing. Keep wrappers aligned with:
    `callsite`, `src_loc`, `CALLSITE_HERE`, `SRC_LOC_HERE`.
  - `utility_defines.h` for common utility macros. Keep helpers aligned with:
    `strfy`, `strfy_exp`, `cat`, `cat_exp`, `expr`, `stmt`, `count_of`, `size_of_each`, `multiline_literal`,
    `bit`, `bit64`, `bit_is_set`, `bit_set`, `bit_unset`, `bit_toggle`, `field_of`, `size_of_field`,
    `offset_of`, `container_of`, `in_range`, `in_bounds`, `swap_value`, `swap_ptr`, `is_pow2`, `align_up`,
    `align_down`, `kb`, `mb`, `gb`, `tb`, `th`, `mil`, `bil`, `tril`.
  - `safe.h` for loop safety helpers. Use `safe_for` and `safe_while` for fixed or regular loops, and do not convert game loops to the safe loop macros.

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
  - Keep usage aligned with `modules/core/include/utils/log_state.h`, `modules/core/include/context/thread_ctx.h`, and `modules/core/include/context/global_ctx.h`.

## Global Vs Thread Context

- `ctx` is the shared payload type used by both global and thread context helpers.
- Prefer `thread_ctx`/`thread_*` helpers for thread-affine work, temporary allocations, and per-thread user data.
- Prefer `global_ctx_get()`/`global_*` helpers for process-wide shared state and cross-thread shared resources.
- When doing coordinated multi-step access to the shared global context, use `global_ctx_lock()` / `global_ctx_unlock()`.
- Remember that `thread_get_log_state()` falls back to the global log state when the current thread context is not initialized.

## SDK Release Maintenance

- SDK automation files are:
  - `.github/workflows/ci.yml` (one build+test per OS)
  - `.github/workflows/release-sdk.yml` (compile matrix and per-variant SDK payload artifacts)
  - `.github/workflows/publish-sdk.yml` (packages releases, full bundle, checksums, uploads assets)
  - `.github/sdk/cmake/based-sdk-targets.cmake` (CMake integration shipped in full bundle)
  - `.github/sdk/premake5.lua` (Premake integration shipped in full bundle)
  - `.github/sdk/README.md` (full bundle quick usage)
- SDK release operation is manual: create/push the version tag (for example `v1.2.3`), let `release-sdk.yml` run, then run `publish-sdk.yml` manually with matching `release_tag` and `run_id` when needed.
- Target names in SDK integration must match real CMake targets from module CMake files (`based-<module>`). Keep names exact (current: `based-core`, `based-gfx`).
- For SDK packaging, include headers for every dependency linked as `PUBLIC` by exported based targets (current `based-core` set includes `olib::static`, `libmath2`, and optional `Tracy::TracyClient`, so their headers must be staged under SDK `include/third_party/...`).
- When adding/removing/renaming modules, update all of the following in the same change:
  - module list in this `AGENTS.md`
  - root `CMakeLists.txt` module wiring
  - `.github/workflows/release-sdk.yml` staging logic so headers/libs for the module are included in each SDK variant
  - `.github/sdk/cmake/based-sdk-targets.cmake` so imported targets and config-specific library resolution include the module
  - `.github/sdk/premake5.lua` so include directories, links, and per-config library directories include the module
  - `.github/sdk/README.md` examples and notes if integration surface changes
- When `PUBLIC` dependencies change on SDK-exported targets, update both `.github/workflows/release-sdk.yml` (header staging) and SDK integration files (`.github/sdk/cmake/based-sdk-targets.cmake`, `.github/sdk/premake5.lua`) in the same change.
- The full SDK archive (`based-sdk-<version>-full.zip`) must always include every variant plus current CMake/Premake integration files.
- SDK payloads should continue copying all root `*.md` files so licensing and distribution docs stay bundled without hardcoding filenames.
