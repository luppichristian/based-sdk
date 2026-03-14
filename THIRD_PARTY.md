# Third-Party Libraries

This file lists the third-party libraries referenced by the CMake fetch helpers in `third_party/` and the license each dependency is distributed under.

## Libraries Used By `third_party/`

| Library | CMake Source | Version / Reference | Upstream | License |
| --- | --- | --- | --- | --- |
| Tracy | `third_party/fetch_tracy.cmake` | `v0.13.1` | `https://github.com/wolfpld/tracy` | BSD 3-Clause |
| SDL3 | `third_party/fetch_sdl3.cmake` | `release-3.2.0` / `SDL3-3.2.0.tar.gz` | `https://github.com/libsdl-org/SDL` | zlib |
| efsw | `third_party/fetch_efsw.cmake` | `master` | `https://github.com/SpartanJ/efsw` | MIT |
| miniz | `third_party/fetch_miniz.cmake` | repository HEAD (no `GIT_TAG` pinned) | `https://github.com/richgel999/miniz` | MIT |
| olib | `third_party/fetch_olib.cmake` | `v1.0.0` | `https://github.com/luppichristian/olib` | MIT |
| libmath2 | `third_party/fetch_libmath2.cmake` | `main` / `main.zip` | `https://github.com/luppichristian/libmath2` | MIT |
| GoogleTest | `third_party/fetch_googletest.cmake` | `v1.14.0` | `https://github.com/google/googletest` | BSD 3-Clause |

## License Notes

- Tracy is distributed under the BSD 3-Clause license.
- SDL3 is distributed under the zlib license.
- efsw is distributed under the MIT license.
- miniz is distributed under the MIT license.
- olib is distributed under the MIT license.
- libmath2 is distributed under the MIT license.
- GoogleTest is distributed under the BSD 3-Clause license.

## Verification Sources

The license information above was checked against each upstream project's published license file or repository license metadata for the exact reference used by the current CMake fetch scripts where available.

## Maintenance

If a dependency is added, removed, renamed, or its fetched version changes in `third_party/`, update this file in the same change.
