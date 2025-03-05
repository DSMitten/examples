// Matching header
#include "../OSVersion.impl.h"

// Vcpkg headers
#include <nativehost/misc-errors-api/Errors.h>
#include <nativehost/misc-strings-api/zstring_view.h>
#include <nativehost/misc-types/FormatHack.h>

// Platform headers
#include <Windows.h>
#include <comdef.h>
#include <winrt/Windows.System.Profile.h>

// STL headers
#include <functional>
#include <string>

using namespace ClientApp::Interop;
using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Interop;

namespace {
    constexpr zwstring_view c_CurrentVersionPath{ L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion" };
    constexpr zwstring_view c_ProductNameValueName{ L"ProductName" };
    constexpr zwstring_view c_DisplayVersionValueName{ L"DisplayVersion" };
    constexpr zwstring_view c_VersionNumberValueName{ L"LCUVer" };

    std::wstring GetRegistryStringValue(HKEY hKey, zwstring_view path, zwstring_view valueName)
    {
        DWORD requiredSize;
        ThrowIf_NotEqual(RegGetValueW(hKey, path.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &requiredSize), ERROR_SUCCESS);

        std::wstring buffer(requiredSize / sizeof(wchar_t), L'\0');
        ThrowIf_NotEqual(RegGetValueW(hKey, path.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, buffer.data(), &requiredSize), ERROR_SUCCESS);
        buffer.resize((requiredSize - 1) / sizeof(wchar_t));
        return buffer;
    }

    std::wstring GetOSVersionDescription()
    {
        const std::wstring productName = GetRegistryStringValue(HKEY_LOCAL_MACHINE, c_CurrentVersionPath, c_ProductNameValueName);
        const std::wstring displayVersion = GetRegistryStringValue(HKEY_LOCAL_MACHINE, c_CurrentVersionPath, c_DisplayVersionValueName);
        return std::format(L"{} {}", productName, displayVersion);
    }

    std::wstring GetOSVersionNumber()
    {
        return GetRegistryStringValue(HKEY_LOCAL_MACHINE, c_CurrentVersionPath, c_VersionNumberValueName);
    }

}
[[nodiscard]] Future<void> OSVersionImpl::GetOSVersionInformation(std::function<void(ValueOrConstRef<OSVersionInformation>)> writeReturnValue)
{
    xstring osVersionName = GetOSVersionDescription();
    xstring osVersion = GetOSVersionNumber();

    OSVersionInformation versionInfo
    {
        .description = osVersionName,
        .version = osVersion
    };
    writeReturnValue(versionInfo);
    return MakePresetFuture();
}
