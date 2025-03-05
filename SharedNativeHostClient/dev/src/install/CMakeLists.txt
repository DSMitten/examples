# Application
set(CLIENT_APP_NAME ClientApp)

if (PLATFORM_WIN)
    include (${CMAKE_CURRENT_LIST_DIR}/win/win.cmake)
elseif (PLATFORM_MAC)
    include (${CMAKE_CURRENT_LIST_DIR}/mac/mac.cmake)
endif()

# Site built by npm. SITE_INSTALL_DIR is set by <plat>/<plat.cmake>
install(DIRECTORY ${CMAKE_SOURCE_DIR}/packages/clientsite/site DESTINATION ${SITE_INSTALL_DIR})
