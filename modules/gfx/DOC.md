# Gfx Module Quick Guide

`gfx` is the rendering-facing module built on top of `core`. It exposes module-level graphics entry points and serves as the integration surface for graphics backends.

This guide is organized by API area and then by header so you can quickly jump to relevant types and functions.

## API Areas

- `gfx`

## gfx

Graphics module-facing API for availability and module identity.

### `gfx/module.h`

- Types: **0**
- Functions: **2**

#### Types

- None

#### Functions

- `gfx_module_is_available() -> b32`
  - What it does: Performs gfx module is available
  - Parameters: none
- `gfx_module_get_name() -> cstr8`
  - What it does: Gets gfx module get name
  - Parameters: none
