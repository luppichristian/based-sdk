# MIT License
# Copyright (c) 2026 Christian Luppi

# =========================================================================
# Project Setup
# =========================================================================

cmake_minimum_required(VERSION 3.19)
project(based C CXX)
include(FetchContent)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_tracy.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_sdl3.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_efsw.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_miniz.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_olib.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_libmath2.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch_googletest.cmake")

# Ensure third-party subprojects default to honoring pre-set option variables.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Language options
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dont care about this
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# Configuration options
option(BASED_BUILD_TESTS         "Build tests"                  ON)
option(BASED_ENABLE_FETCHCONTENT "Enable FetchContent for deps" ON)
option(BASED_PROFILER_ENABLED    "Enable Tracy profiler"        ON)

# =========================================================================
# Modules
# =========================================================================

add_subdirectory(core)