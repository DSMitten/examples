#include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/Platform.cmake)
#include(${CMAKE_CURRENT_LIST_DIR}/../../../dev/make/FindCompiler.cmake)

# Template file for the compiler paths for VCPKG to use
# Build tools 'EnableCompiler' step reads this template and replace the placeholders with the compiler paths before invoking vcpkg install
#set(CMAKE_C_COMPILER "/usr/local/opt/llvm@18/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

#if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
#    set(CMAKE_OBJC_COMPILER "/usr/local/opt/llvm@18/bin/clang")
#    set(CMAKE_OBJCXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

    # Make sure the C++ standard library files we link against are from the llvm we installed, not
    # the default ones installed with XCode
#    link_directories(BEFORE "/usr/local/opt/llvm@18/lib/c++")
#endif()

# Custom toolchain file that is used in place of the standard vcpkg toolchain

function (set_compile_settings)

    # This runs before the project command, so the CMAKE_<LANG>_COMPILER_ID variables have not been set
    # So instead we'll look at the compiler executable name to find out what compiler is being used
    cmake_path(GET CMAKE_C_COMPILER STEM compiler)

    if (("${compiler}" STREQUAL "clang-cl") OR ("${compiler}" STREQUAL "clang"))
        set(compiler_clang TRUE)
    endif()

endfunction()

if (TRUE)
    set(CMAKE_C_COMPILER "/usr/local/opt/llvm@18/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(CMAKE_OBJC_COMPILER "/usr/local/opt/llvm@18/bin/clang")
        set(CMAKE_OBJCXX_COMPILER "/usr/local/opt/llvm@18/bin/clang++")

        function(set_llvm_lib_path)
            cmake_path(GET CMAKE_OBJCXX_COMPILER PARENT_PATH llvm_bin_path)
            cmake_path(GET llvm_bin_path PARENT_PATH llvm_path)
            set(llvm_lib_path ${llvm_path}/lib)
            link_directories(BEFORE ${llvm_lib_path})
        endfunction()

        # Make sure the C++ standard library files we link against are from the llvm we installed, not
        # the default ones installed with XCode
        set_llvm_lib_path()
        add_link_options(LINKER:-lunwind)

        # cmake can add libraries multiple times in link commands. It adds direct dependencies and then appends transitive dependencies.
        # linking on mac produces 'ignoring duplicate library' warnings, which we don't, so suppress it
        add_link_options(LINKER:-no_warn_duplicate_libraries)

    endif()

    set_compile_settings()
endif()


#if (PLATFORM_WIN)
#    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/windows.cmake")
#elseif (PLATFORM_MAC)
    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/osx.cmake")
#elseif (PLATFORM_LINUX)
#    include("${CMAKE_CURRENT_LIST_DIR}/../../vcpkg/scripts/toolchains/linux.cmake")
#endif()
