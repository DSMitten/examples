# Application
install(TARGETS ClientApp DESTINATION bin)

if (PLATFORM_WIN)
    # Shared native host 
    install(FILES ${SHARED_NATIVE_HOST_INSTALL_BIN_FILES} DESTINATION bin)

    # WebView2 loader
    install(FILES ${WEBVIEW2_INSTALL_FILES} DESTINATION bin)

    # zlib & minizip
    install(FILES ${VCPKG_BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}zlib${D_SUFFIX}1.${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION bin)
    install(FILES ${VCPKG_BIN_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}minizip.${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION bin)
elseif (PLATFORM_MAC)
    # Shared native host 
    install(FILES ${SHARED_NATIVE_HOST_INSTALL_BIN_FILES} DESTINATION bin/ClientApp.app/Contents/Frameworks)
endif()
