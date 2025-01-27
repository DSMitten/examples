cmake_minimum_required(VERSION 3.29)

include(${CMAKE_CURRENT_LIST_DIR}/dev/make/Platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dev/make/CompilerEnvironment.cmake)

project(ExampleNativeHostClient LANGUAGES CXX)

# Use C++20 language standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

# Automatically add the current source and build directories to the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# On Linux and Mac, compile position independent code and build position independent executables.
if (PLATFORM_POSIX)
    # Note we have to call check_pie_supported for CMAKE_POSITION_INDEPENDENT_CODE to set compiler and linker flags correctly
    include(CheckPIESupported)
    check_pie_supported(LANGUAGES CXX)

    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

    if (PLATFORM_MAC)
        # Enable setting macos rpath
        # Useful reference: https://www.mikeash.com/pyblog/friday-qa-2009-11-06-linking-and-install-names.html
        set(CMAKE_MACOSX_RPATH TRUE)
    else()
        # Set shared library RUNPATH to $ORIGIN because our shared libraries will be located next to executables
        # Useful reference: https://nehckl0.medium.com/creating-relocatable-linux-executables-by-setting-rpath-with-origin-45de573a2e98
        list(APPEND CMAKE_INSTALL_RPATH "$ORIGIN")
    endif()
endif()

# Add the path to our modules to the cmake module path, so find_package can locate packages we've added
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/dev/make/modules;${CMAKE_MODULE_PATH})

# Set paths to dependencies
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBUG_DIR "debug/")
    set(D_SUFFIX "d")
endif()

set(VCPKG_INC_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
set(VCPKG_LIB_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${DEBUG_DIR}lib)
set(VCPKG_BIN_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/${DEBUG_DIR}bin)

# Find packages used by the project
find_package(WebView2 REQUIRED)
find_package(SharedNativeHost REQUIRED)

# Specify the install directory.
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install directory")

# wv2-host package headers look for PLATFORM_ macros for conditional platform-specific code
if (PLATFORM_WIN)
    add_compile_definitions(PLATFORM_WIN)
elseif (PLATFORM_MAC)
    add_compile_definitions(PLATFORM_MAC)
elseif (PLATFORM_LINUX)
    add_compile_definitions(PLATFORM_LINUX)
else()
    message(FATAL_ERROR "No supported platform was selected.")
endif()

if (PLATFORM_POSIX)
    add_compile_definitions(PLATFORM_POSIX)
endif()

if (PLATFORM_WIN)
    # Add definitions when compiling source files
    add_compile_definitions(
        UNICODE                         # Specify unicode for Windows header files
        NOMINMAX                        # When including <Windows.h>, don't define 'min' or 'max' macros
                                        # that could interfere with identifiers named min or max
    )
endif()

add_subdirectory(dev)


# Application
install(TARGETS SharedNativeHostClient DESTINATION bin)

# Shared native host 
install(FILES ${SHARED_NATIVE_HOST_INSTALL_BIN_FILES} DESTINATION bin)

# WebView2 loader
install(FILES ${WEBVIEW2_INSTALL_FILES} DESTINATION bin)

# zlib & minizip
install(FILES ${VCPKG_BIN_DIR}/zlib${D_SUFFIX}1.dll DESTINATION bin COMPONENT application)
install(FILES ${VCPKG_BIN_DIR}/minizip.dll DESTINATION bin COMPONENT application)
