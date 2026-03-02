# based Agent Guide

## Hello

**KEEP THIS ALWAYS UP TO DATE, THIS IS YOUR MEMORY*

`based` is a cross-platform C23 utility library with public headers under `include/`, implementations under `src/`, and GoogleTest coverage under `tests/`.

Use this file as the authoritative repo guide for future coding tasks. If files are added or removed, update this document in the same change.

Keeping this file up to date is mandatory. Any change that adds, removes, or materially repurposes files or directories covered here must update `AGENTS.md` in the same change.

## Code Rules
- Never build, configure cmake yourself. Always let me do it.
- Every `.h`, `.c`, and test file should start with:
  `// MIT License`
  `// Copyright (c) 2026 Christian Luppi`
- Use `#pragma once` in headers.
- Respect `.clang-format` and `.clang-tidy`; warnings are treated as errors.
- Use project primitive types (`i32`, `u64`, `sz`, `b32`, `c8`, etc.) instead of raw C types **always**.
- Prefer project qualifiers and keywords (`func`, `global_var`, `local_persist`, `force_inline`, `no_inline`, `no_return`, `read_only`).
- You should never use the `static` keyword directly, use `func` even if functions are internal.
- Naming is snake_case for functions, variables, typedefs, structs, and enums; macros and constants are UPPER_CASE.
- Do not introduce project-defined `_t` types.
- Variable and parameter names should generally be at least 3 characters to satisfy the active tidy rules.
- Functions that have a name that starts with `_` means that they should be called by users with the implemented convenience macro so that we can pass it callsite info. Look at `codespace.h` `CALLSITE_HERE` define.
- `include/based.h` is the umbrella header, you should always keep it up to date when creating public header files.

## Project Layout

```text
include/                      # public API headers
  based.h                     # umbrella include for most public modules
  basic/                      # core types, macros, asserts, logging, environment helpers
    assert.h                  # runtime assertions and assert callbacks
    codespace.h               # callsite/source location helpers
    env_defines.h             # platform, arch, compiler, build macros
    intrinsics.h              # popcount, bit scans, byte swap, rotates
    keyword_defines.h         # project keyword and attribute macros
    log.h                     # logging levels, macros, and callbacks
    primitive_types.h         # primitive type aliases and min/max constants
    utility_defines.h         # general-purpose utility macros
  containers/                 # intrusive containers and container utilities
    binary_tree.h             # intrusive binary tree macros
    bitset.h                  # bitset macros over u64 arrays
    doubly_list.h             # intrusive doubly-linked list macros
    hash_map.h                # Robin Hood hash map API
    ring_list.h               # intrusive circular doubly-linked list macros
    singly_list.h             # intrusive singly-linked list macros
    stack_list.h              # intrusive LIFO stack macros
    tree.h                    # intrusive N-ary tree macros
  memory/                     # allocators and memory-management utilities
    allocator.h               # generic allocator callback interface
    arena.h                   # bump allocator
    buffer.h                  # non-owning buffer views and helpers
    heap.h                    # general-purpose heap allocator
    pool.h                    # fixed-size object pool allocator
    ring.h                    # byte-oriented ring buffer
    scratch.h                 # arena checkpoint helper
    vmem.h                    # low-level virtual-memory primitives
  strings/                    # UTF-aware string and Unicode helpers
    cstrings.h                # null-terminated string utilities
    strings.h                 # bounded mutable string wrappers
    unicode.h                 # UTF encoding, decoding, and conversion
  threads/                    # threading, sync, and atomics
    atomics.h                 # atomic integer/pointer APIs and fences
    condvar.h                 # condition variable API
    mutex.h                   # mutex API
    rwlock.h                  # reader-writer lock API
    semaphore.h               # counting semaphore API
    spinlock.h                # spinlock API
    thread.h                  # thread creation and lifecycle
    thread_current.h          # current-thread helpers
    thread_group.h            # fixed-size thread group helpers
src/                          # module implementations
  basic/                      # implementations for basic/*
  containers/                 # implementations for container modules with source files
  memory/                     # implementations for memory/*
  strings/                    # implementations for strings/*
  threads/                    # implementations for threads/*
tests/                        # GoogleTest coverage
```

Names should remain descriptive enough that this section can stay compact; 
Update this tree when files are added, removed, or materially repurposed. 

## Keywords Sheet

- `func`: function declaration alias; becomes `static` when `ALL_FUNCS_STATIC` is defined. Use this instead of writing `static` directly for functions.
- `global_var`: cosmetic marker for global variables; becomes `static` when `ALL_GLOBAL_VARS_STATIC` is defined.
- `local_persist`: alias for `static` storage on local variables.
- `read_only`: readonly global/file-scope storage qualifier; maps to `const` plus platform-specific readonly section placement when supported.
- `thread_local`: thread-local storage qualifier; maps to the platform/compiler-specific thread-local keyword.
- `dll_export`: export visibility marker for shared-library symbols.
- `dll_import`: import visibility marker for shared-library symbols.
- `static_assert(expr)`: single-argument compile-time assertion wrapper when the language mode does not already provide one.
- `no_return`: non-returning function attribute.
- `force_inline`: force-inline function attribute/qualifier.
- `no_inline`: no-inline function attribute/qualifier.
- `align_as(x)`: alignment specifier wrapper.
- `align_of(x)`: alignment query wrapper.
- `size_of(x)`: project alias for `sizeof(x)`.
- `likely(x)`: branch prediction hint for likely code paths.
- `unlikely(x)`: branch prediction hint for unlikely code paths.

Related core helper macros from `include/basic/` that are used pervasively:

- `CALLSITE_HERE`: expands to a `callsite` with the current file, function, and line.
- `assert(condition)`: project assert wrapper that forwards through `_assert(..., CALLSITE_HERE)`.
- `log_fatal(...)`, `log_error(...)`, `log_warn(...)`, `log_info(...)`, `log_debug(...)`, `log_verbose(...)`, `log_trace(...)`: logging wrappers that forward through `_log(..., CALLSITE_HERE)`.

Common utility macros from `include/basic/utility_defines.h`:

- Token/string helpers: `stringify`, `stringify_exp`, `concat`, `concat_exp`, `multiline_literal`.
- Expression/array helpers: `expression`, `count_of`, `size_of_each`.
- Bit helpers: `bit`, `bit_is_set`, `bit_set`, `bit_unset`, `bit_toggle`.
- Layout helpers: `field_sizeof`, `offset_of`, `container_of`.
- Range/alignment helpers: `in_range`, `in_bounds`, `is_pow2`, `align_up`, `align_down`.
- Numeric helpers: `kb`, `mb`, `gb`, `tb`, `th`, `mil`, `bil`, `tril`.
- Value helper: `swap`.
