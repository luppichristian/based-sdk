function(fetch_libmath2)
    get_property(based_fetch_libmath2_done GLOBAL PROPERTY BASED_FETCH_LIBMATH2_DONE)
    if(based_fetch_libmath2_done)
        return()
    endif()

    message(STATUS "Fetching libmath2...")

    find_package(libmath2 QUIET)
    if(libmath2_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_LIBMATH2_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "libmath2 package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    set(LM2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(LM2_BUILD_SHARED OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        libmath2
        URL https://github.com/luppichristian/libmath2/archive/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(libmath2)

    message(STATUS "libmath2 package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_LIBMATH2_DONE TRUE)
endfunction()
