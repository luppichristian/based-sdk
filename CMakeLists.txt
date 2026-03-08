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
# Tracy
# =========================================================================

option(BASED_TRACY_FETCH "Fetch Tracy if not found" ON)
find_package(Tracy CONFIG QUIET)

if(NOT Tracy_FOUND AND BASED_TRACY_FETCH)
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.13.1
    )
    FetchContent_MakeAvailable(tracy)
endif()

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

# Binary-kind compile definitions by target type.
function(based_apply_binary_type_defines target_name)
    get_target_property(target_type ${target_name} TYPE)
    if(NOT target_type)
        message(FATAL_ERROR "based_apply_binary_type_defines: target '${target_name}' has no TYPE")
    endif()

    if(target_type STREQUAL "EXECUTABLE")
        target_compile_definitions(${target_name} PRIVATE BIN_RUNNABLE)
    elseif(target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "MODULE_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_DYNAMIC_LIB)
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_STATIC_LIB)
    elseif(target_type STREQUAL "OBJECT_LIBRARY")
        target_compile_definitions(${target_name} PRIVATE BIN_OBJ_LIB)
    else()
        message(FATAL_ERROR "based_apply_binary_type_defines: unsupported target type '${target_type}' for target '${target_name}'")
    endif()
endfunction()

# =========================================================================
# Library (always static for now)
# =========================================================================

file(GLOB_RECURSE BASED_SOURCES "src/**.c")
add_library(based STATIC ${BASED_SOURCES})
target_include_directories(based PUBLIC include)
target_link_libraries(based PRIVATE SDL3::SDL3 efsw miniz PUBLIC olib::static libmath2 Tracy::TracyClient)
target_compile_definitions(based PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(based PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
based_apply_binary_type_defines(based)
target_compile_definitions(
    based
    INTERFACE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:BIN_RUNNABLE>
        $<$<OR:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>,$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>>:BIN_DYNAMIC_LIB>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:BIN_STATIC_LIB>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,OBJECT_LIBRARY>:BIN_OBJ_LIB>
)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    enable_testing()

    # Ignore a placeholder test registered by libmath2 when its test binary
    # is intentionally disabled in this workspace configuration.
    file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake" "set(CTEST_CUSTOM_TESTS_IGNORE libmath2-tests_NOT_BUILT)\n")

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

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/**.cpp")
    add_executable(based-tests ${TEST_SOURCES})
    target_link_libraries(based-tests PRIVATE based GTest::gtest)
    target_include_directories(based-tests PRIVATE tests)
    target_compile_options(based-tests PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
    based_apply_binary_type_defines(based-tests)

    include(GoogleTest)
    gtest_discover_tests(based-tests PROPERTIES RUN_SERIAL TRUE)

    # libmath2 may register a gtest_discover placeholder when its test binary
    # is not built in this workspace configuration; ignore that non-project test.
    if(TEST libmath2-tests_NOT_BUILT)
        set_tests_properties(libmath2-tests_NOT_BUILT PROPERTIES DISABLED TRUE)
    endif()
endif()
