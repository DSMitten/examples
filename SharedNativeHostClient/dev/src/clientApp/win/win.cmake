target_sources(${TARGET_NAME}
    PRIVATE
        win/OSVersion.impl.cpp
        win/WinMain.cpp
        win/WinPlatformInit.cpp
)

# Support for delay-loading dlls
target_link_libraries(${TARGET_NAME} PRIVATE delayimp)
