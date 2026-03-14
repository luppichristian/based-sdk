function(fetch_tracy)
    get_property(based_fetch_tracy_done GLOBAL PROPERTY BASED_FETCH_TRACY_DONE)
    if(based_fetch_tracy_done)
        return()
    endif()

    message(STATUS "Fetching Tracy...")

    find_package(Tracy CONFIG QUIET)
    if(Tracy_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_TRACY_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "Tracy package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.13.1
    )
    FetchContent_MakeAvailable(tracy)

    message(STATUS "Tracy package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_TRACY_DONE TRUE)
endfunction()