include(BundleUtilities)

set(BUNDLE_PATH ${CMAKE_INSTALL_PREFIX}/bin/ClientApp.app/Contents/MacOS/ClientApp${CMAKE_EXECUTABLE_SUFFIX})
message(STATUS "BUNDLE_PATH: ${BUNDLE_PATH}")

fixup_bundle("${BUNDLE_PATH}" "" "")