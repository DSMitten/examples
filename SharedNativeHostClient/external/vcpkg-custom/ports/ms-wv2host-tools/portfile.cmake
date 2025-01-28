include(${CMAKE_CURRENT_LIST_DIR}/getSources.cmake)

set(VCPKG_POLICY_EMPTY_INCLUDE_FOLDER enabled)
set(VCPKG_POLICY_DLLS_WITHOUT_LIBS enabled)
set(VCPKG_POLICY_MISMATCHED_NUMBER_OF_BINARIES enabled)

# Only need release build of tools
set(VCPKG_BUILD_TYPE "release")

get_sources(REPO_ROOT_PATH)
cmake_path(NORMAL_PATH REPO_ROOT_PATH)
set(SOURCE_PATH ${REPO_ROOT_PATH}/dev/tools/BuildTools/native)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    DISABLE_PARALLEL_CONFIGURE
    NO_CHARSET_FLAG
    GENERATOR Ninja
    OPTIONS
        --log-level=VERBOSE
        -DDONT_CONFIGURE_COMPILER_ENVIRONMENT=TRUE
    OPTIONS_RELEASE
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
)

vcpkg_cmake_install()

set(BIN_DIR ${CURRENT_PACKAGES_DIR}/bin)
set(LIB_DIR ${CURRENT_PACKAGES_DIR}/lib)
set(TOOLS_DIR ${CURRENT_PACKAGES_DIR}/tools/${PORT})
set(SHARE_DIR ${CURRENT_PACKAGES_DIR}/share/${PORT})

# Tools binaries
set(INTEROPGEN_EXE_BIN ${BIN_DIR}/interopgen${VCPKG_HOST_EXECUTABLE_SUFFIX})
set(LOGEVENTSGEN_EXE_BIN ${BIN_DIR}/logeventsgen${VCPKG_HOST_EXECUTABLE_SUFFIX})
set(BUILDTOOLS_DLL_BIN ${BIN_DIR}/${VCPKG_TARGET_SHARED_LIBRARY_PREFIX}buildtools${VCPKG_TARGET_SHARED_LIBRARY_SUFFIX})
set(YAMLCPP_DLL_BIN ${CURRENT_INSTALLED_DIR}/bin/${VCPKG_TARGET_SHARED_LIBRARY_PREFIX}yaml-cpp${VCPKG_TARGET_SHARED_LIBRARY_SUFFIX})


# Executable goes into tools directory
file(COPY ${INTEROPGEN_EXE_BIN} DESTINATION ${TOOLS_DIR})
file(COPY ${LOGEVENTSGEN_EXE_BIN} DESTINATION ${TOOLS_DIR})

# Build tools library goes into tools directory on Windows & Linux, but lib directory on Mac
# vcpkg stamps the Mac executable's RPATH variable to go to the lib directory
if (VCPKG_HOST_IS_WINDOWS OR VCPKG_HOST_IS_LINUX)
    # On Windows and Linux, buildtools library goes with the executable
    file(COPY ${BUILDTOOLS_DLL_BIN} DESTINATION ${TOOLS_DIR})
elseif(VCPKG_HOST_IS_OSX)
    # On Mac, vcpkg stamps RPATH to look in lib directory
    file(COPY ${BUILDTOOLS_DLL_BIN} DESTINATION ${LIB_DIR})
endif()

# On Windows, yaml-cpp is a dll, so copy to tools directory. On Mac & Linux, it is a static library.
if (VCPKG_HOST_IS_WINDOWS)
    file(COPY ${YAMLCPP_DLL_BIN} DESTINATION ${TOOLS_DIR})
endif()

# Don't need the remaining dlls or libs in the bin directory
# They were just there to support build tool exes and were copied into the tools dir
file(REMOVE_RECURSE "${BIN_DIR}")

# cmake helper files
file(COPY ${SOURCE_PATH}/share/generateInterop.cmake DESTINATION ${SHARE_DIR})
file(COPY ${SOURCE_PATH}/share/generateLogEvents.cmake DESTINATION ${SHARE_DIR})

# base logevents json files
file(COPY ${REPO_ROOT_PATH}/dev/src/nh/runtime/logging/api/nh-logging-api-logevents.json DESTINATION ${SHARE_DIR})
file(COPY ${REPO_ROOT_PATH}/dev/src/nh/runtime/telemetry/api/nh-telemetry-api-logevents.json DESTINATION ${SHARE_DIR})

# Schema files
file(COPY ${REPO_ROOT_PATH}/dev/tools/schemas/nhapi/nhapi.schema.json DESTINATION ${SHARE_DIR})
file(COPY ${REPO_ROOT_PATH}/dev/tools/schemas/logevents/LogEvents.schema.json DESTINATION ${SHARE_DIR})

file(WRITE "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" "Refer to LICENSE file")
