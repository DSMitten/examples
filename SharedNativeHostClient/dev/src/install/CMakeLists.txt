# Application
set(CLIENT_APP_NAME ClientApp)

if (PLATFORM_WIN)
    # Executable
    install(TARGETS ${CLIENT_APP_NAME} RUNTIME DESTINATION bin)

    # Shared native host 
    install(FILES ${SHARED_NATIVE_HOST_INSTALL_BIN_FILES} DESTINATION bin)

    # WebView2 loader
    install(FILES ${WEBVIEW2_INSTALL_FILES} DESTINATION bin)

    # zlib & minizip
    install(FILES ${VCPKG_BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}zlib${D_SUFFIX}1${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION bin)
    install(FILES ${VCPKG_BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}minizip${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION bin)
elseif (PLATFORM_MAC)
    # Apple bundle contents placement documentation:
    # https://developer.apple.com/documentation/bundleresources/placing-content-in-a-bundle
    # Executable
    install(TARGETS ${CLIENT_APP_NAME} BUNDLE DESTINATION bin)

     set(CLIENT_APP_CONTENTS_DIR ${CLIENT_APP_NAME}.app/Contents)
    set(CLIENT_APP_HELPERS_DIR ${CLIENT_APP_CONTENTS_DIR}/Helpers)

    # WebView2 framework runtime files

    install(SCRIPT ${CMAKE_CURRENT_LIST_DIR}/FixupBundle.cmake)

    # WebView2 browser executable app
    install(DIRECTORY ${WEBVIEW2_APP_PATH} DESTINATION bin/${CLIENT_APP_HELPERS_DIR} USE_SOURCE_PERMISSIONS)
endif()
