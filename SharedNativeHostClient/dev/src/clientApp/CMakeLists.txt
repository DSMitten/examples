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
        startupSteps/InteropObjectsStartupStep.cpp
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
    include(${CMAKE_CURRENT_LIST_DIR}/win/win.cmake)
elseif (PLATFORM_MAC)
    include(${CMAKE_CURRENT_LIST_DIR}/mac/mac.cmake)
endif()

add_generate_log_events(${TARGET_NAME})

add_generate_interop(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/OSVersion.nhapi.yaml
)
