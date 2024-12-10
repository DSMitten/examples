set(TARGET_NAME SharedNativeHostClient)

# Platform-specific cmake instructions
if (PLATFORM_WIN)

    add_executable(${TARGET_NAME} WIN32
        win/WinMain.cpp
        win/WinPlatformInit.cpp
    )

    # Support for delay-loading dlls
    target_link_libraries(${TARGET_NAME} PRIVATE delayimp)

elseif (PLATFORM_MAC)

    add_executable(${TARGET_NAME} MACOSX_BUNDLE)

    # RPATH for a bundle should look in the bundle's Frameworks directory
    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY INSTALL_RPATH "@executable_path/../Frameworks")

else()

    add_executable(${TARGET_NAME})

endif()

# Platform-independent cmake instructions
target_sources(${TARGET_NAME}
    PRIVATE
        CommonMain.cpp
        startup/LoggingStartup.cpp
        startup/MainWindowStartupStep.cpp
        startup/PlatformStartupStep.cpp
        startup/ServicesRegistrarStartupStep.cpp
        startup/StartupShutdown.cpp
        startup/TasksSystemStartupStep.cpp
        startup/WebHostSystemStartupStep.cpp
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        NativeHost::SharedLibrary
)
