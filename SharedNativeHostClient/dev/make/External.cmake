# Find Mac SDK frameworks if building on MacOS
if (PLATFORM_MAC)
    include (${CMAKE_CURRENT_LIST_DIR}/mac/MacFrameworks.cmake)
endif()

# Set paths to vcpkg directories
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBUG_DIR "debug/")
    set(D_SUFFIX "d")
endif()

set(VCPKG_INC_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
set(VCPKG_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${DEBUG_DIR}lib)
set(VCPKG_BIN_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${DEBUG_DIR}bin)

# Add the path to our modules to the cmake module path, so find_package can locate packages we've added
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules;${CMAKE_MODULE_PATH})

# Find packages used by the project
if (PLATFORM_WIN OR PLATFORM_MAC)
    find_package(WebView2 REQUIRED)
endif()

find_package(SharedNativeHost REQUIRED)

# Add logevents & interop generation functions
message(STATUS "WV2HOST_LOG_EVENTS_OUTPUT_DIR: ${CMAKE_BINARY_DIR}/gen/logEvents")
set(WV2HOST_LOG_EVENTS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/gen/logEvents)
set(INTEROP_INCLUDE_DIR           ${CMAKE_BINARY_DIR}/gen/interop/include)
set(INTEROP_HEADERS_DIR           ${INTEROP_INCLUDE_DIR}/nativehost/interop)

include("${MS_WV2HOST_TOOLS_SHARE_DIR}/generateLogEvents.cmake")
include("${MS_WV2HOST_TOOLS_SHARE_DIR}/generateInterop.cmake")

# -----------------------------------------------------------------------------
# Wrapper for target_generate_interop_files, providing settings used by
# the webview2-host-framework repo
# -----------------------------------------------------------------------------
function(add_generate_log_events target_name)

    target_add_generate_log_events(${target_name}
        NH_LOGGING_DEPS
            NativeHost::IncludeDirs
    )

endfunction()

# -----------------------------------------------------------------------------
# Wrapper for target_generate_interop_files, providing settings used by
# the webview2-host-framework repo
# -----------------------------------------------------------------------------
function(add_generate_interop target_name input_file_paths)

    set(input_file_paths ${input_file_paths} ${ARGN}) # append any overflow arguments into input_file_paths

    target_generate_interop_files(${target_name}
        OUTPUT_DIR ${INTEROP_HEADERS_DIR}
        INCLUDE_DIR ${INTEROP_INCLUDE_DIR}
        FILES ${input_file_paths}
    )

    get_target_property(targetType ${target_name} TYPE)
        if (${targetType} STREQUAL "INTERFACE_LIBRARY")
            set(export_type INTERFACE)
        else()
            set(export_type PUBLIC)
        endif()

    # Connect the target to dependencies that are required by the generated header file
    target_link_libraries(${target_name}
        ${export_type}
            NativeHost::IncludeDirs
    )

endfunction()