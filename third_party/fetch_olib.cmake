function(fetch_olib)
    get_property(based_fetch_olib_done GLOBAL PROPERTY BASED_FETCH_OLIB_DONE)
    if(based_fetch_olib_done)
        return()
    endif()

    message(STATUS "Fetching olib...")

    find_package(olib QUIET)
    if(olib_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_OLIB_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "olib package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    set(OLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(OLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(OLIB_BUILD_CLI OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        olib
        URL https://github.com/luppichristian/olib/archive/refs/tags/v1.0.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(olib)

    message(STATUS "olib package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_OLIB_DONE TRUE)
endfunction()
