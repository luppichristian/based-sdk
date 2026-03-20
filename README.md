# based

`based` is a cross-platform C utility library organized as modules.

Current modules:

- `core`: foundational systems, containers, memory, filesystem, input, threads, logging, and utilities
- `gfx`: graphics-facing functionality built on top of `core`

## Documentation

- Build from source: `HOW_TO_BUILD.md`
- Integrate prebuilt SDK releases: `HOW_TO_USE_SDK.md`
- Third-party dependencies and licenses: `THIRD_PARTY.md`

## Quick start (build from source)

```bash
cmake -S . -B build -G Ninja -D CMAKE_BUILD_TYPE=Release -D BASED_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure --build-config Release
```

If you are new to the project, read `HOW_TO_BUILD.md` first for platform prerequisites and all build options.

## Quick start (SDK consumer)

If you want to consume prebuilt libraries instead of building this repo, download a `based-sdk-<version>-full.zip` release and follow `HOW_TO_USE_SDK.md`.

## License

This project is licensed under the MIT License. See `LICENSE`.
