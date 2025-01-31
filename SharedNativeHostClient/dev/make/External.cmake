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
find_package(WebView2 REQUIRED)
find_package(SharedNativeHost REQUIRED)

# Add logevents generation functions
function (find_generateLogEvents_cmake output_var)
    set(${output_var}  PARENT_SCOPE)
endfunction()

message(STATUS "WV2HOST_LOG_EVENTS_OUTPUT_DIR: ${CMAKE_BINARY_DIR}/gen/logEvents")
set(WV2HOST_LOG_EVENTS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/gen/logEvents)

include("${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/share/ms-wv2host-tools/generateLogEvents.cmake")
include("${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/share/ms-wv2host-tools/generateInterop.cmake")

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
