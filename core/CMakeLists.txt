
# =========================================================================
# Dependencies
# =========================================================================

fetch_tracy()
fetch_sdl3()
fetch_efsw()
fetch_miniz()
fetch_olib()
fetch_libmath2()

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

# Source code
file(GLOB_RECURSE BASED_CORE_SOURCES "src/**.c")

function(configure_based_core_target tgt source_root)
    target_include_directories(${tgt} PUBLIC include)
    target_link_libraries(${tgt}
        PRIVATE SDL3::SDL3 efsw miniz
        PUBLIC olib::static libmath2 Tracy::TracyClient
    )
    target_compile_options(${tgt} PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})
    foreach(source_file IN LISTS ARGN)
        get_filename_component(source_dir "${source_file}" DIRECTORY)
        file(RELATIVE_PATH relative_dir "${source_root}" "${source_dir}")

        if(relative_dir STREQUAL "")
            set(unity_group "root")
        else()
            string(REPLACE "\\" "/" normalized_dir "${relative_dir}")
            string(REGEX MATCH "^[^/]+" unity_group "${normalized_dir}")
        endif()

        set_source_files_properties(${source_file} PROPERTIES UNITY_GROUP "${unity_group}")
    endforeach()
    set_target_properties(${tgt} PROPERTIES UNITY_BUILD ON UNITY_BUILD_MODE GROUP)
endfunction()

# Default target (entry point is not handled)
add_library(based-core STATIC ${BASED_CORE_SOURCES})
configure_based_core_target(based-core "${CMAKE_CURRENT_SOURCE_DIR}/src" ${BASED_CORE_SOURCES})

# Module entry target (ENTRY_TYPE_MOD handled)
add_library(based-core-mod STATIC ${BASED_CORE_SOURCES})
configure_based_core_target(based-core-mod "${CMAKE_CURRENT_SOURCE_DIR}/src" ${BASED_CORE_SOURCES})
target_compile_definitions(based-core-mod PUBLIC ENTRY_TYPE_MOD )

# Main entry target (ENTRY_TYPE_MAIN handled)
add_library(based-core-main STATIC ${BASED_CORE_SOURCES})
configure_based_core_target(based-core-main "${CMAKE_CURRENT_SOURCE_DIR}/src" ${BASED_CORE_SOURCES})
target_compile_definitions(based-core-main PUBLIC ENTRY_TYPE_MAIN)

# App entry target (ENTRY_TYPE_APP handled)
add_library(based-core-app STATIC ${BASED_CORE_SOURCES})
configure_based_core_target(based-core-app "${CMAKE_CURRENT_SOURCE_DIR}/src" ${BASED_CORE_SOURCES})
target_compile_definitions(based-core-app PUBLIC ENTRY_TYPE_APP)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    enable_testing()

    # Ignore a placeholder test registered by libmath2 when its test binary
    # is intentionally disabled in this workspace configuration.
    file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake" "set(CTEST_CUSTOM_TESTS_IGNORE libmath2-tests_NOT_BUILT)\n")

    fetch_googletest()

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/**.cpp")
    add_executable(based-core-tests ${TEST_SOURCES})
    list(LENGTH TEST_SOURCES TEST_SOURCE_COUNT)
    if(TEST_SOURCE_COUNT GREATER 0)
        set_target_properties(
            based-core-tests
            PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE ${TEST_SOURCE_COUNT}
        )
    endif()
    target_link_libraries(based-core-tests PRIVATE based-core GTest::gtest)
    target_include_directories(based-core-tests PRIVATE tests)
    target_compile_options(based-core-tests PRIVATE ${BASED_CLANG_FOLDING_CONSTANT_WARNING})

    include(GoogleTest)
    gtest_discover_tests(based-core-tests PROPERTIES RUN_SERIAL TRUE)

    # libmath2 may register a gtest_discover placeholder when its test binary
    # is not built in this workspace configuration; ignore that non-project test.
    if(TEST libmath2-tests_NOT_BUILT)
        set_tests_properties(libmath2-tests_NOT_BUILT PROPERTIES DISABLED TRUE)
    endif()
endif()
