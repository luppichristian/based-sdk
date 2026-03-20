
# =========================================================================
# Library (always static for now)
# =========================================================================

# Source code
file(GLOB_RECURSE BASED_GFX_SOURCES "src/**.c")

function(setup_based_gfx_target target core_dep)
    target_include_directories(${target} PUBLIC include src)
    target_link_libraries(${target} PUBLIC ${core_dep})
    foreach(source_file IN LISTS BASED_GFX_SOURCES)
        get_filename_component(source_dir "${source_file}" DIRECTORY)
        file(RELATIVE_PATH relative_dir "${CMAKE_CURRENT_SOURCE_DIR}/src" "${source_dir}")

        if(relative_dir STREQUAL "")
            set(unity_group "root")
        else()
            string(REPLACE "\\" "/" normd_dir "${relative_dir}")
            string(REGEX MATCH "^[^/]+" unity_group "${normd_dir}")
        endif()

        set_source_files_properties(${source_file} PROPERTIES UNITY_GROUP "${unity_group}")
    endforeach()
    set_target_properties(${target} PROPERTIES UNITY_BUILD ON UNITY_BUILD_MODE GROUP)
endfunction()

add_library(based-gfx STATIC ${BASED_GFX_SOURCES})
setup_based_gfx_target(based-gfx based-core)
add_custom_command(
    TARGET based-gfx
    POST_BUILD
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/generate_api_indexes.py" --root "${CMAKE_SOURCE_DIR}" --modules gfx
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Regenerating gfx api_index.txt if stale"
    VERBATIM
)

# =========================================================================
# Tests
# =========================================================================

if(BASED_BUILD_TESTS)
    set(BASED_GFX_TEST_DEP "based-gfx")
    if(BASED_PROFILER_ENABLED)
        add_library(based-gfx-no-profile STATIC ${BASED_GFX_SOURCES})
        setup_based_gfx_target(based-gfx-no-profile based-core-no-profile)
        set(BASED_GFX_TEST_DEP "based-gfx-no-profile")
    endif()

    file(GLOB_RECURSE TEST_SOURCES "tests/**.cpp")
    list(LENGTH TEST_SOURCES TEST_SOURCE_COUNT)
    if(TEST_SOURCE_COUNT GREATER 0)
        add_executable(based-gfx-tests ${TEST_SOURCES})
        set_target_properties(
            based-gfx-tests
            PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE ${TEST_SOURCE_COUNT}
        )
        target_link_libraries(based-gfx-tests PRIVATE ${BASED_GFX_TEST_DEP} GTest::gtest)
        target_include_directories(based-gfx-tests PRIVATE tests)
        target_compile_definitions(based-gfx-tests PRIVATE ENTRY_WINDOWS_CONSOLE)
        gtest_discover_tests(based-gfx-tests DISCOVERY_MODE POST_BUILD)
    endif()
endif()
