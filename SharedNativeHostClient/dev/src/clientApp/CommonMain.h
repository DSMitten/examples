#pragma once

// vcpkg includes
#include <nativehost/misc-strings-api/Strings.h>

// STL headers
#include <filesystem>

// App-wide constants and settings
inline constexpr Microsoft::NativeHost::zxstring_view c_AppName{ XSTR(APP_NAME) };

// Shared main function
int CommonMain();
