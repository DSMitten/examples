# The toolchain file is used by cmake to configure compiler paths and other settings

include(${CMAKE_CURRENT_LIST_DIR}/Platform.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/CompilerEnvironment.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/FindCompiler.cmake)

# Find the paths to vcpkg directories
set(VCPKG_DIR ${CMAKE_CURRENT_LIST_DIR}/../../external/vcpkg)
cmake_path(NORMAL_PATH VCPKG_DIR)
set(VCPKG_TOOLCHAIN_PATH ${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_INSTALLED_DIR ${VCPKG_DIR}/installed-${VCPKG_TARGET_TRIPLET})

if (NOT EXISTS "${VCPKG_TOOLCHAIN_PATH}")
    message(FATAL_ERROR "VCPKG_TOOLCHAIN_PATH not found: ${VCPKG_TOOLCHAIN_PATH}. run ensureVcpkg.ps1 if it is not cloned yet")
endif()

# Configure vcpkg overlay ports & triplets
set(VCPKG_CUSTOM_DIR ${CMAKE_CURRENT_LIST_DIR}/../../external/vcpkg-custom)
cmake_path(NORMAL_PATH VCPKG_CUSTOM_DIR)
set(VCPKG_OVERLAY_PORTS ${VCPKG_CUSTOM_DIR}/ports)
set(VCPKG_OVERLAY_TRIPLETS ${VCPKG_CUSTOM_DIR}/triplets)

# Supply vcpkg toolchain
include(${VCPKG_TOOLCHAIN_PATH})
