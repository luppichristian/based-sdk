function(fetch_miniz)
    get_property(based_fetch_miniz_done GLOBAL PROPERTY BASED_FETCH_MINIZ_DONE)
    if(based_fetch_miniz_done)
        return()
    endif()

    message(STATUS "Fetching miniz...")

    find_package(miniz QUIET)
    if(miniz_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_MINIZ_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "miniz package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    set(BUILD_TESTS OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_EXAMPLES OFF)

    FetchContent_Declare(
        miniz
        GIT_REPOSITORY https://github.com/richgel999/miniz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(miniz)

    message(STATUS "miniz package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_MINIZ_DONE TRUE)
endfunction()
