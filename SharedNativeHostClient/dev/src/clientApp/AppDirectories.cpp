// Matching header
#include "AppDirectories.h"

// Local headers
#include "CommonMain.h"

// Vcpkg headers
#include <nativehost/misc-filesystem-api/PlatformPaths.h>

using namespace Microsoft::NativeHost;

std::filesystem::path GetAppUserDataDirectoryPath()
{
    return GetUserDataBaseDirectoryPath() / c_AppName.c_str();
}

std::filesystem::path GetLogFilesPath()
{
    return GetAppUserDataDirectoryPath() / "logs";
}
