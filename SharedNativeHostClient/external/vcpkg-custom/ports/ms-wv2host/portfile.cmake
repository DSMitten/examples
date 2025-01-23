include(${CMAKE_CURRENT_LIST_DIR}/../ms-wv2host-tools/getSources.cmake)

get_sources(SOURCE_PATH)

# -----------------------------------------------------------------------------
# Generate, build & install
# -----------------------------------------------------------------------------
if(${TARGET_TRIPLET} STREQUAL "x64-windows")
    set(CONFIG_BASE "win-x64")
elseif(${TARGET_TRIPLET} STREQUAL "x64-windows-vulcanbuild")
    set(CONFIG_BASE "win-x64")
elseif(${TARGET_TRIPLET} STREQUAL "x86-windows")
    set(CONFIG_BASE "win-x86")
elseif(${TARGET_TRIPLET} STREQUAL "arm64-windows")
    set(CONFIG_BASE "win-arm64")
elseif(${TARGET_TRIPLET} STREQUAL "x64-osx")
    set(CONFIG_BASE "mac-x64")
elseif(${TARGET_TRIPLET} STREQUAL "arm64-osx")
    set(CONFIG_BASE "mac-arm64")
elseif(${TARGET_TRIPLET} STREQUAL "x64-linux")
    set(CONFIG_BASE "lnx-x64")
else()
    message(FATAL_ERROR "Unable to resolve triplet ${TARGET_TRIPLET}")
endif()

# Add dotnet to the path for the build tools, as the path is sanitized by vcpkg and dotnet path gets removed
# Vcpkg doesn't yet support `vcpkg_find_acquire_program` use to 
# find or acquire dotnet, so the DOTNET_PATH must be set in the port file
if(VCPKG_TARGET_IS_WINDOWS)
    set(DOTNET_PATH C:\\Program\ Files\\dotnet)
    vcpkg_add_to_path(${DOTNET_PATH})

    vcpkg_find_acquire_program(NUGET)
    cmake_path(GET NUGET PARENT_PATH NUGET_DIR)
    vcpkg_add_to_path(${NUGET_DIR})
endif()

# WV2 is not public for mac, and must be manually installed on mac
if(VCPKG_TARGET_IS_OSX)
    set(CUSTOM_WV2_FRAMEWORK_DIR "${CMAKE_CURRENT_LIST_FILE}/../../../../nuget/MSWebView2")
    cmake_path(NORMAL_PATH CUSTOM_WV2_FRAMEWORK_DIR)
    set(CUSTOM_WV2_FRAMEWORK_DIR_OPTION "-DCUSTOM_WV2_FRAMEWORK_DIR=${CUSTOM_WV2_FRAMEWORK_DIR}")
endif()

# Make sure that the ms-wv2host port uses the client-native-host VCPKG_INSTALLED_DIR
cmake_path(GET CURRENT_INSTALLED_DIR PARENT_PATH VCPKG_INSTALLED_DIR)

# Configure the project
vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    DISABLE_PARALLEL_CONFIGURE
    NO_CHARSET_FLAG
    GENERATOR Ninja
    OPTIONS
        --log-level=VERBOSE
        -DNO_CLANG_TIDY=TRUE
        -DENSURE_BUILD_TOOLS=TRUE
        -DVCPKG_INSTALLED_DIR=${VCPKG_INSTALLED_DIR}
        ${CUSTOM_WV2_FRAMEWORK_DIR_OPTION}
    OPTIONS_DEBUG
        -DCONFIG=${CONFIG_BASE}-debug
        -DONLY_INSTALL_BINARIES=TRUE
    OPTIONS_RELEASE
        -DCONFIG=${CONFIG_BASE}-release
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

file(WRITE "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" "Refer to LICENSE file")
