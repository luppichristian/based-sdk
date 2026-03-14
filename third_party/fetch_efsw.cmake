function(fetch_efsw)
    get_property(based_fetch_efsw_done GLOBAL PROPERTY BASED_FETCH_EFSW_DONE)
    if(based_fetch_efsw_done)
        return()
    endif()

    message(STATUS "Fetching efsw...")

    find_package(efsw QUIET)
    if(efsw_FOUND)
        set_property(GLOBAL PROPERTY BASED_FETCH_EFSW_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "efsw package not found and BASED_ENABLE_FETCHCONTENT is OFF")
    endif()

    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_STATIC_LIBS ON)
    set(BUILD_TEST_APP OFF)

    FetchContent_Declare(
        efsw
        GIT_REPOSITORY https://github.com/SpartanJ/efsw
        GIT_TAG master
    )
    FetchContent_MakeAvailable(efsw)

    message(STATUS "efsw package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_EFSW_DONE TRUE)
endfunction()
