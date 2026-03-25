# Gfx Module Quick Guide

`gfx` is the rendering-facing module built on top of `core`. It exposes module-level graphics entry points and serves as the integration surface for graphics backends.

The canonical API reference remains `./api_index.txt`. This guide is organized by API area and then by header so you can quickly locate public types, functions, and macros.

## API Areas

- `gfx`

## gfx

Graphics module-facing API for availability and module identity.

### `gfx/module.h`

- Types: **0**
- Functions: **2**
- Macros: **0**

#### Types

| Type | Declaration |
| --- | --- |
| None | - |

#### Functions

| Function | Declaration |
| --- | --- |
| `gfx_module_is_available` | `func b32 gfx_module_is_available(void);` |
| `gfx_module_get_name` | `func cstr8 gfx_module_get_name(void);` |

#### Macros

| Macro | Form |
| --- | --- |
| None | - |
