# Custom toolchain file that is used in place of the standard vcpkg toolchain
# Allows vcpkg packages to build with the same compiler that the repo builds with
# The official vcpkg toolchain script is included at the bottom of this file

# Use the same files to find compiler for vcpkg packages as we use for this repo
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/Platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/CompilerEnvironment.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/FindCompiler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/mac/MacLlvm.cmake)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    # Make sure the C++ standard library files we link against are from the llvm we installed,
    # not the default ones installed with XCode
    get_macos_llvm_lib_dir(llvm_lib_dir)
    get_macos_llvm_libcpp_dir(llvm_libcpp_dir)
    link_directories(BEFORE ${llvm_lib_dir} ${llvm_libcpp_dir})

    # Link against unwind library, which is necessary for stack traces
    add_link_options(LINKER:-lunwind)

    # cmake can add libraries multiple times in link commands. It adds direct dependencies and then appends transitive dependencies.
    # linking on mac produces 'ignoring duplicate library' warnings, which we don't, so suppress it
    add_link_options(LINKER:-no_warn_duplicate_libraries)
endif()

if (PLATFORM_WIN)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/windows.cmake")
elseif (PLATFORM_MAC)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/osx.cmake")
elseif (PLATFORM_LINUX)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/linux.cmake")
endif()
