# -----------------------------------------------------------------------------
# From default vcpkg triplet
# -----------------------------------------------------------------------------
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES x86_64)

# -----------------------------------------------------------------------------
# Our custom settings
# -----------------------------------------------------------------------------

# Default to building vcpkg ports with hidden visibility so they work better with project targets
# which are also built with hidden visibility. Mixing visibilities can result in linker warnings
# such as 'ld: warning: direct access in function' ... means the weak symbol cannot be overridden at runtime.

# Some vcpkg ports can't build successfully with hidden visibility, so those will remain default
if (NOT PORT MATCHES "openssl")
    # Set default visibility to hidden for vcpkg target
    set(VCPKG_C_FLAGS "${VCPKG_C_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    set(VCPKG_CXX_FLAGS "${VCPKG_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

    # The following variables don't do anything but hopefully will in the future
    # vpkcg issue https://github.com/microsoft/vcpkg/issues/27099 has been opened to track adding support for setting objective-c compile flags
    set(VCPKG_OBJC_FLAGS "${VCPKG_OBJC_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    set(VCPKG_OBJCXX_FLAGS "${VCPKG_OBJCXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
endif()

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/ChainloadToolchainFile.cmake)
