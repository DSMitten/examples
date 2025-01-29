# -----------------------------------------------------------------------------
# Shared vcpkg settings for all triplets
# -----------------------------------------------------------------------------

# Note - we can't use Platform.cmake to create PLATFORM_ macros because the
# VCPKG_TARGET_TRIPLET variable is not set for the chainload toolchain file.
# VCPKG_CMAKE_SYSTEM_NAME and WIN32 variables can tell us the platform

if ("${VCPKG_CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    # The repo builds with -fvisibility=hidden and -fvisibility-inlines-hidden.
    # Vcpkg packages need to build with the same settings, to prevent runtime errors due to inconsisent visibility
    set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DCMAKE_CXX_VISIBILITY_PRESET=hidden -DCMAKE_OBJCXX_VISIBILITY_PRESET=hidden -DCMAKE_VISIBILITY_INLINES_HIDDEN=TRUE)
elseif("${VCPKG_CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    # Allow shared libraries to be relocated by adding $ORIGIN to the RPATH
    set(VCPKG_FIXUP_ELF_RPATH TRUE)
endif()

# Tell vcpkg to use our custom toolchain file; it will set compiler paths and then include the vcpkg-provided one
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/ChainloadToolchainFile.cmake)

if (WIN32)
    # Setting VCPKG_CHAINLOAD_TOOLCHAIN_FILE deactivates automatic vcvars setup so reenable it
    set(VCPKG_LOAD_VCVARS_ENV ON)
endif()