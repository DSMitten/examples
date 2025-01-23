#include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/Platform.cmake)
#include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/FindCompiler.cmake)

# Template file for the compiler paths for VCPKG to use
# Build tools 'EnableCompiler' step reads this template and replace the placeholders with the compiler paths before invoking vcpkg install
set(CMAKE_C_COMPILER "/usr/local/opt/llvm@18/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(CMAKE_OBJC_COMPILER "/usr/local/opt/llvm@18/bin/clang")
    set(CMAKE_OBJCXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

    # Make sure the C++ standard library files we link against are from the llvm we installed, not
    # the default ones installed with XCode
    link_directories(BEFORE "/usr/local/opt/llvm@18/lib/c++")
endif()


#if (PLATFORM_WIN)
#    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/windows.cmake")
#elseif (PLATFORM_MAC)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/osx.cmake")
#elseif (PLATFORM_LINUX)
#    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/linux.cmake")
#endif()
