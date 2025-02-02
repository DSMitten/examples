# Custom toolchain file that is used in place of the standard vcpkg toolchain
# Allows vcpkg packages to build with the same compiler that the repo builds with
# The official vcpkg toolchain script is included at the bottom of this file

# Use the same files to find compiler for vcpkg packages as we use for this repo
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/Platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/Processor.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/CompilerEnvironment.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/FindCompiler.cmake)

if (PLATFORM_WIN)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/windows.cmake")
elseif (PLATFORM_MAC)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/osx.cmake")
elseif (PLATFORM_LINUX)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/linux.cmake")
endif()
