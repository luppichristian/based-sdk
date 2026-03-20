if(TARGET based-core AND TARGET based-gfx)
    return()
endif()

set(_based_sdk_root "${CMAKE_CURRENT_LIST_DIR}/..")
set(_based_sdk_variants_dir "${_based_sdk_root}/variants")

if(NOT EXISTS "${_based_sdk_variants_dir}")
    message(FATAL_ERROR "based SDK variants directory not found: ${_based_sdk_variants_dir}")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_based_sdk_os "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_based_sdk_os "linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(_based_sdk_os "macos")
else()
    message(FATAL_ERROR "Unsupported platform '${CMAKE_SYSTEM_NAME}'")
endif()

set(_based_sdk_processor "${CMAKE_SYSTEM_PROCESSOR}")
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_OSX_ARCHITECTURES)
    list(GET CMAKE_OSX_ARCHITECTURES 0 _based_sdk_processor)
endif()

string(TOLOWER "${_based_sdk_processor}" _based_sdk_processor)
if(_based_sdk_processor STREQUAL "x86_64" OR _based_sdk_processor STREQUAL "amd64")
    set(_based_sdk_arch "x86_64")
elseif(_based_sdk_processor STREQUAL "arm64" OR _based_sdk_processor STREQUAL "aarch64")
    set(_based_sdk_arch "arm64")
else()
    message(FATAL_ERROR "Unsupported architecture '${CMAKE_SYSTEM_PROCESSOR}'")
endif()

function(_based_sdk_variant_dir out_dir config_name)
    set(_dir "${_based_sdk_variants_dir}/${_based_sdk_os}-${_based_sdk_arch}-${config_name}")
    if(NOT EXISTS "${_dir}")
        message(FATAL_ERROR "Missing SDK variant directory: ${_dir}")
    endif()
    set(${out_dir} "${_dir}" PARENT_SCOPE)
endfunction()

function(_based_sdk_find_library_file out_path variant_dir logical_name)
    set(_candidates
        "${variant_dir}/lib/${logical_name}.lib"
        "${variant_dir}/lib/lib${logical_name}.a"
        "${variant_dir}/lib/lib${logical_name}.so"
        "${variant_dir}/lib/lib${logical_name}.dylib"
        "${variant_dir}/lib/${logical_name}.dll"
    )
    foreach(_candidate IN LISTS _candidates)
        if(EXISTS "${_candidate}")
            set(${out_path} "${_candidate}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "Library '${logical_name}' was not found in '${variant_dir}/lib'")
endfunction()

function(_based_sdk_set_config_location target_name logical_name config_name)
    string(TOUPPER "${config_name}" _config_upper)
    _based_sdk_variant_dir(_variant_dir "${config_name}")
    _based_sdk_find_library_file(_library_path "${_variant_dir}" "${logical_name}")

    set_property(TARGET ${target_name} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${_config_upper})
    set_target_properties(${target_name} PROPERTIES "IMPORTED_LOCATION_${_config_upper}" "${_library_path}")
endfunction()

add_library(based-core UNKNOWN IMPORTED GLOBAL)
add_library(based-gfx UNKNOWN IMPORTED GLOBAL)
set_target_properties(based-gfx PROPERTIES INTERFACE_LINK_LIBRARIES based-core)

_based_sdk_variant_dir(_release_variant_dir "Release")
target_include_directories(based-core INTERFACE "${_release_variant_dir}/include/core")
target_include_directories(based-core INTERFACE "${_release_variant_dir}/include/third_party/olib")
target_include_directories(based-core INTERFACE "${_release_variant_dir}/include/third_party/libmath2")
target_include_directories(based-core INTERFACE "${_release_variant_dir}/include/third_party/tracy")
target_include_directories(based-gfx INTERFACE "${_release_variant_dir}/include/gfx")

foreach(_config_name IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    _based_sdk_set_config_location(based-core "based-core" "${_config_name}")
    _based_sdk_set_config_location(based-gfx "based-gfx" "${_config_name}")
endforeach()

if(NOT TARGET based::core)
    add_library(based::core ALIAS based-core)
endif()

if(NOT TARGET based::gfx)
    add_library(based::gfx ALIAS based-gfx)
endif()
