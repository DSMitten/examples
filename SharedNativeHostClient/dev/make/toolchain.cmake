# The toolchain file is used by cmake to configure compiler paths and other settings

include(${CMAKE_CURRENT_LIST_DIR}/platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/compiler_environment.cmake)

set(CLANG_VERSION 18)

# -----------------------------------------------------------------------------
# Configures cmake to build with msvc
# -----------------------------------------------------------------------------
function(configure_windows_compiler)
    find_program(CL cl.exe REQUIRED)
    set(CMAKE_CXX_COMPILER "${CL}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_macos_compiler)
    set(llvm_formula_name llvm@18)

    execute_process(
        COMMAND brew --prefix ${llvm_formula_name}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE llvm_path
    )

    if (NOT(${result} EQUAL 0))
        message(FATAL_ERROR "Failed to get the ${llvm_formula_name} formula path. brew exit code: ${result}. brew output: ${llvm_path}")
    endif()

    string(STRIP "${llvm_path}" llvm_path)

    set(CMAKE_CXX_COMPILER "${llvm_path}/bin/clang++" PARENT_SCOPE)

endfunction()

# -----------------------------------------------------------------------------
# Configures cmake to build with LLVM clang
# -----------------------------------------------------------------------------
function(configure_linux_compiler)
    read_clang_version(llvm_version)
    find_program(CLANGXX clang++-18 REQUIRED)
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

# Find the paths to vcpkg directories
set(VCPKG_DIR ${CMAKE_CURRENT_LIST_DIR}/../../external/vcpkg)
cmake_path(NORMAL_PATH VCPKG_DIR)
set(VCPKG_TOOLCHAIN_PATH ${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_INSTALLED_DIR ${VCPKG_DIR}/installed-${VCPKG_TARGET_TRIPLET})

if (NOT EXISTS "${VCPKG_TOOLCHAIN_PATH}")
    message(FATAL_ERROR "VCPKG_TOOLCHAIN_PATH not found: ${VCPKG_TOOLCHAIN_PATH}. run ensureVcpkg.ps1 if it is not cloned yet")
endif()

set(VCPKG_CUSTOM_DIR ${CMAKE_CURRENT_LIST_DIR}/../../external/vcpkg-custom)
cmake_path(NORMAL_PATH VCPKG_CUSTOM_DIR)
set(VCPKG_OVERLAY_PORTS ${VCPKG_CUSTOM_DIR}/ports)

include(${VCPKG_TOOLCHAIN_PATH})
