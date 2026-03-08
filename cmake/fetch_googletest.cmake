function(fetch_googletest)
    get_property(based_fetch_gtest_done GLOBAL PROPERTY BASED_FETCH_GTEST_DONE)
    if(based_fetch_gtest_done)
        return()
    endif()

    message(STATUS "Fetching GTest...")

    find_package(GTest QUIET)
    if(GTest_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_GTEST_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "GTest package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    if(TARGET gtest)
        target_compile_options(gtest PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
    endif()
    if(TARGET gtest_main)
        target_compile_options(gtest_main PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wno-undef>)
    endif()

    message(STATUS "GTest package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_GTEST_DONE TRUE)
endfunction()
