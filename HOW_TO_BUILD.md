# How to build based from source

This guide explains how to configure, build, and test `based` from this repository.

## 1) Prerequisites

- CMake 3.19 or newer
- Python 3 (required by CMake configuration and API index generation scripts)
- A C/C++ toolchain
  - Linux/macOS: `clang`/`clang++` recommended
  - Windows: `clang-cl` recommended
- Ninja (recommended generator)

### Linux packages used in CI

If you are building on Linux and hit missing SDL/system dependency errors, install the package set used in CI:

```bash
sudo apt-get update
sudo apt-get install -y \
  clang \
  ninja-build \
  pkg-config \
  libasound2-dev \
  libpulse-dev \
  libaudio-dev \
  libfribidi-dev \
  libjack-dev \
  libsndio-dev \
  libx11-dev \
  libxext-dev \
  libxrandr-dev \
  libxcursor-dev \
  libxfixes-dev \
  libxi-dev \
  libxss-dev \
  libxtst-dev \
  libxkbcommon-dev \
  libdrm-dev \
  libgbm-dev \
  libgl1-mesa-dev \
  libgles2-mesa-dev \
  libegl1-mesa-dev \
  libdbus-1-dev \
  libibus-1.0-dev \
  libinput-dev \
  libudev-dev \
  libthai-dev \
  libpipewire-0.3-dev \
  libwayland-dev \
  libdecor-0-dev \
  liburing-dev
```

## 2) Configure

From repository root:

```bash
cmake -S . -B build -G Ninja \
  -D CMAKE_BUILD_TYPE=Release \
  -D BASED_BUILD_TESTS=ON \
  -D BASED_ENABLE_MODULE_GRAPHICS=ON \
  -D BASED_PROFILER_ENABLED=ON
```

## 3) Build

```bash
cmake --build build --parallel
```

## 4) Run tests

```bash
ctest --test-dir build --output-on-failure --build-config Release
```

For environments where `ctest` may not execute discovered binaries the way you want, you can also run the helper used in CI:

```bash
python .github/scripts/run_test_executables.py --build-dir build --build-config Release
```

## Important CMake options

- `BASED_BUILD_TESTS` (default `ON`): build module test targets
- `BASED_ENABLE_MODULE_GRAPHICS` (default `ON`): include `gfx` module
- `BASED_PROFILER_ENABLED` (default `ON`): enable Tracy profiling support
- `BASED_ENABLE_FETCHCONTENT` (default `ON`): allow dependency fetching through CMake FetchContent

## Notes

- Libraries are currently built as static libraries in module CMake files
- Root target names are module-scoped: `based-core` and `based-gfx`
- If you change build type with single-config generators (Ninja/Makefiles), re-run configure with the desired `CMAKE_BUILD_TYPE`
