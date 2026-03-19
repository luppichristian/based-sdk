function(fetch_sdl3)
    get_property(based_fetch_sdl3_done GLOBAL PROPERTY BASED_FETCH_SDL3_DONE)
    if(based_fetch_sdl3_done)
        return()
    endif()

    message(STATUS "Fetching SDL3...")

    find_package(SDL3 QUIET)
    if(SDL3_FOUND)
        message(STATUS "SDL3 package found")
        set_property(GLOBAL PROPERTY BASED_FETCH_SDL3_DONE TRUE)
        return()
    endif()

    if(NOT BASED_ENABLE_FETCHCONTENT)
        message(FATAL_ERROR "SDL3 package was not found, cant continue since BASED_ENABLE_FETCHCONTENT is OFF...")
    endif()

    message(STATUS "SDL3 package was not found, proceeding with FetchContent...")

    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SDL3
        URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.0/SDL3-3.2.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(SDL3)

    message(STATUS "SDL3 package properly fetched")
    set_property(GLOBAL PROPERTY BASED_FETCH_SDL3_DONE TRUE)
endfunction()
