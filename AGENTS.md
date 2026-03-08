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

## Project Layout

```text
cmake/                       # CMake helper modules (dependency fetch/setup helpers)
core/                        # Core module
  AGENTS.md                  # Core-module specific coding and layout guide
  CMakeLists.txt             # Core-module build targets
  include/                   # Core public headers
  src/                       # Core implementations
  tests/                     # Core tests
  api_index.txt              # Core public API index
CMakeLists.txt               # Top-level project configuration and module wiring
```

Keep this tree accurate as project-level structure evolves.
