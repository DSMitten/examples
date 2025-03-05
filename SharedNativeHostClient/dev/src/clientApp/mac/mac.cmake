target_sources(${TARGET_NAME}
    PRIVATE
        mac/AppDelegate.mm
        mac/OSVersion.impl.mm
        mac/MacPlatformInit.mm
        mac/main.cpp
)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        MacOS::AppKit
        MacOS::Foundation
)

target_compile_definitions(${TARGET_NAME} PRIVATE WEBVIEW2_APP_NAME=\"${WEBVIEW2_APP_NAME}\")
