# MIT License
# Copyright (c) 2026 Christian Luppi

# =========================================================================
# Setup
# =========================================================================

cmake_minimum_required(VERSION 3.19)
project(based C CXX)
include(FetchContent)

# Ensure third-party subprojects default to honoring pre-set option variables.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BASED_BUILD_TESTS  "Build tests"                ON)
option(BASED_GTEST_FETCH  "Fetch GoogleTest if not found" ON)

# =========================================================================
# SDL3
# =========================================================================

option(BASED_SDL3_FETCH "Fetch SDL3 if not found" ON)
find_package(SDL3 QUIET)

if(NOT SDL3_FOUND AND BASED_SDL3_FETCH)
    set(SDL_SHARED          OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC          ON  CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY    OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS           OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES        OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        SDL3
        URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/SDL3-3.2.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# =========================================================================
# efsw
# =========================================================================

option(BASED_EFSW_FETCH "Fetch efsw if not found" ON)
find_package(efsw QUIET)

if(NOT efsw_FOUND AND BASED_EFSW_FETCH)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_TEST_APP OFF)

    FetchContent_Declare(
        efsw 
        GIT_REPOSITORY https://github.com/SpartanJ/efsw 
        GIT_TAG master
    )
    FetchContent_MakeAvailable(efsw)
endif()

# =========================================================================
# miniz
# =========================================================================

option(BASED_MINIZ_FETCH "Fetch miniz if not found" ON)
find_package(miniz QUIET)

if(NOT miniz_FOUND AND BASED_MINIZ_FETCH)
    set(BUILD_TESTS Off)
    set(BUILD_SHARED_LIBS Off)
    set(BUILD_EXAMPLES Off)

    FetchContent_Declare(
        miniz 
        GIT_REPOSITORY https://github.com/richgel999/miniz 
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(miniz)
endif()

# =========================================================================
# Olib
# =========================================================================

option(BASED_OLIB_FETCH "Fetch Olib if not found" ON)
find_package(olib QUIET)

if(NOT olib_FOUND AND BASED_OLIB_FETCH)
    set(OLIB_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(OLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(OLIB_BUILD_CLI      OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        olib
        URL https://github.com/luppichristian/olib/archive/refs/tags/v1.0.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(olib)
endif()

# =========================================================================
# libmath2
# =========================================================================

option(BASED_LIBMATH2_FETCH "Fetch libmath2 if not found" ON)
find_package(libmath2 QUIET)
if(NOT libmath2_FOUND AND BASED_LIBMATH2_FETCH)
    set(LM2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(LM2_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        libmath2
        URL https://github.com/luppichristian/libmath2/archive/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(libmath2)
endif()

# Define LM2_CUSTOM_ASSERT to use custom assert in libmath2.
target_compile_definitions(libmath2 PUBLIC LM2_CUSTOM_ASSERT)

# Suppress Clang's GNU extension folding warning without affecting other compilers.
set(
    BASED_CLANG_FOLDING_CONSTANT_WARNING
    $<$<COMPILE_LANG_AND_ID:C,Clang,AppleClang>:-Wno-gnu-folding-constant>
    $<$<COMPILE_LANG_AND_ID:CXX,Clang,AppleClang>:-Wno-gnu-folding-constant>
)

# =========================================================================
# Library (always static for now)
# =========================================================================

file(GLOB_RECURSE BASED_SOURCES "src/**.c")
add_library(based STATIC ${BASED_SOURCES})
target_include_directories(based PUBLIC include)
target_link_libraries(based PRIVATE SDL3::SDL3 efsw miniz PUBLIC olib::static libmath2)
target_compile_definitions(based PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(based PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    find_package(GTest QUIET)

    if(NOT GTest_FOUND AND BASED_GTEST_FETCH)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        target_compile_options(gtest      PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
        target_compile_options(gtest_main PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
    endif()

    file(GLOB TEST_SOURCES "tests/*.cpp")
    add_executable(based-tests ${TEST_SOURCES})
    target_link_libraries(based-tests PRIVATE based GTest::gtest_main)
    target_compile_options(based-tests PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})

    include(GoogleTest)
    gtest_discover_tests(based-tests)
endif()
