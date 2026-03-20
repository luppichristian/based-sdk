# How to use the based SDK

This guide explains how to consume prebuilt `based` SDK releases in your own project.

## 1) Download the SDK

From the repository releases, download:

- `based-sdk-<version>-full.zip`

The full archive contains:

- `variants/` with platform+arch+configuration payloads named `<os>-<arch>-<build_type>`
- `cmake/based-sdk-targets.cmake` for CMake integration
- `premake5.lua` for Premake integration

## 2) Pick the matching variant

Your build must match the SDK variant on:

- OS: `windows`, `linux`, or `macos`
- Architecture: `x86_64` or `arm64`
- Build configuration: `Debug`, `Release`, `RelWithDebInfo`, or `MinSizeRel`

If your project builds with a configuration that does not exist in the SDK archive, configuration will fail.

## 3) CMake integration

In your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.19)
project(my_app C)

add_executable(my_app src/main.c)

include("/absolute/path/to/based-sdk-<version>-full/cmake/based-sdk-targets.cmake")

target_link_libraries(my_app PRIVATE based-core based-gfx)
```

What this gives you:

- imported targets `based-core` and `based-gfx`
- aliases `based::core` and `based::gfx`
- include directories for `core`, `gfx`, and required bundled third-party headers (`olib`, `libmath2`, `tracy`)
- automatic library selection per active build configuration

## 4) Premake integration

In your `premake5.lua`:

```lua
local based_sdk = dofile("/absolute/path/to/based-sdk-<version>-full/premake5.lua")

workspace "my_workspace"
    configurations { "Debug", "Release", "RelWithDebInfo", "MinSizeRel" }
    architecture "x86_64" -- or ARM64

project "my_app"
    kind "ConsoleApp"
    language "C"
    files { "src/**.c" }

    based_sdk.use("/absolute/path/to/based-sdk-<version>-full")
```

The helper adds include directories, links `based-core`/`based-gfx`, and sets configuration-specific library directories.

## 5) Include headers in your code

- Core umbrella header: `#include <based_core.h>`
- Gfx umbrella header: `#include <based_gfx.h>`

## Troubleshooting

- Missing variant directory: confirm OS/architecture/configuration matches a folder in `variants/`
- Missing library errors: ensure you are using a build type present in the SDK archive
- Include not found: ensure you included the SDK helper file before linking `based-core`/`based-gfx`
