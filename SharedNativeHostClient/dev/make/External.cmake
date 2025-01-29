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
