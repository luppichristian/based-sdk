# based SDK full bundle

This bundle contains all per-platform SDK variants under `variants/`.

- CMake integration helper: `cmake/based-sdk-targets.cmake`
- Premake5 integration helper: `premake5.lua`

Each variant is named `<os>-<arch>-<build_type>`.

## CMake usage

```cmake
include("<path-to-full-sdk>/cmake/based-sdk-targets.cmake")

target_link_libraries(your-target PRIVATE based-core based-gfx)
```

The CMake helper resolves the active platform/architecture and binds the imported target locations based on `CMAKE_BUILD_TYPE` (or the active multi-config configuration).
It also exposes bundled third-party public include paths required by `based-core` public link dependencies (`olib`, `libmath2`, `Tracy`).

## Premake5 usage

```lua
local based_sdk = dofile("<path-to-full-sdk>/premake5.lua")

project "your-project"
    kind "ConsoleApp"
    language "C"
    architecture "x86_64" -- or ARM64
    configurations { "Debug", "Release", "RelWithDebInfo", "MinSizeRel" }

    based_sdk.use("<path-to-full-sdk>")
```

The Premake helper applies include paths and selects the per-config `libdirs` for the current architecture.
