include(${CMAKE_CURRENT_LIST_DIR}/ClangVersion.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/mac/MacLlvm.cmake)

# -----------------------------------------------------------------------------
# Configures cmake to build with msvc
# -----------------------------------------------------------------------------
function(configure_windows_compiler)
    find_program(CL cl.exe REQUIRED)
    set(CMAKE_C_COMPILER "${CL}" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER "${CL}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_macos_compiler)

    get_macos_llvm_path(llvm_path)

    # Specify the compilers to use on MacOS
    set(CMAKE_C_COMPILER "${llvm_path}/bin/clang" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER "${llvm_path}/bin/clang++" PARENT_SCOPE)
    set(CMAKE_OBJC_COMPILER "${llvm_path}/bin/clang" PARENT_SCOPE)
    set(CMAKE_OBJCXX_COMPILER "${llvm_path}/bin/clang++" PARENT_SCOPE)

endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_linux_compiler)
    find_program(CLANG "clang-${CLANG_VERSION}" REQUIRED)
    find_program(CLANGXX "clang++-${CLANG_VERSION}" REQUIRED)
    set(CMAKE_C_COMPILER "${CLANG}" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILER "${CLANGXX}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configures compiler if not already set
# -----------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER}" STREQUAL "")
    # compiler isn't already set up
    if (PLATFORM_WIN)
        configure_windows_compiler()
    elseif (PLATFORM_MAC)
        configure_macos_compiler()
    elseif (PLATFORM_LINUX)
        configure_linux_compiler()
    else()
        message(FATAL_ERROR "Unsupported system type")
    endif()
endif()
