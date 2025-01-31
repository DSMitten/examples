set(TARGET_NAME ClientApp)

if (PLATFORM_WIN)
    set(EXE_TYPE WIN32)
elseif (PLATFORM_MAC)
    set(EXE_TYPE MACOSX_BUNDLE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/mac/info_plist.cmake)
endif()

add_executable(${TARGET_NAME} ${EXE_TYPE})

target_compile_definitions(${TARGET_NAME} PRIVATE "APP_NAME=\"${TARGET_NAME}\"")

target_sources(${TARGET_NAME}
    PRIVATE
        AppDirectories.cpp
        CommonMain.cpp
        startupSteps/LoggingStartup.cpp
        startupSteps/MainWindowStartupStep.cpp
        startupSteps/PlatformStartupStep.cpp
        startupSteps/ServicesRegistrarStartupStep.cpp
        startupSteps/StartupShutdown.cpp
        startupSteps/TasksSystemStartupStep.cpp
        startupSteps/WebHostSystemStartupStep.cpp
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        NativeHost::SharedLibrary
)

if (PLATFORM_WIN)
    target_sources(${TARGET_NAME}
        PRIVATE
            win/WinMain.cpp
            win/WinPlatformInit.cpp
    )

    # Support for delay-loading dlls
    target_link_libraries(${TARGET_NAME} PRIVATE delayimp)
elseif (PLATFORM_MAC)
    target_sources(${TARGET_NAME}
        PRIVATE
            mac/AppDelegate.mm
            mac/MacPlatformInit.mm
            mac/main.cpp
    )

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            MacOS::AppKit
            MacOS::Foundation
    )

    target_path_compile_definition(${TARGET_NAME} "${WEBVIEW2_APP_PATH}" WEBVIEW2_APP_PATH)

    # RPATH for a bundle should look in the bundle's Frameworks directory
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY INSTALL_RPATH "@executable_path/../Frameworks")
endif()

add_generate_log_events(${TARGET_NAME})
