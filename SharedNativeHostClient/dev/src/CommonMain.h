#pragma once

// vcpkg includes
#include <nativehost/misc-strings-api/Strings.h>

int CommonMain();

inline constexpr Microsoft::NativeHost::zxstring_view c_AppName{ XSTR("SharedNativeHostClient") };
