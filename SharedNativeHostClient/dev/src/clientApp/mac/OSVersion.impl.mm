// Matching header
#include "../OSVersion.impl.h"

// Vcpkg headers
#include <nativehost/misc-errors-api/Errors.h>
#include <nativehost/misc-objc/mac/ObjCConversions.h>

// Platform headers
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

// STL headers
#include <functional>
#include <string>

using namespace ClientApp::Interop;
using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Interop;

namespace {

    std::string GetOSVersionDescription()
    {
        NSProcessInfo *processInfo = [NSProcessInfo processInfo];
        return ToString(processInfo.operatingSystemVersionString);
    }

    std::string GetOSVersionNumber()
    {
        NSDictionary *systemVersion = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
        return ToString(systemVersion[@"ProductBuildVersion"]);
    }

}
[[nodiscard]] Future<void> OSVersionImpl::GetOSVersionInformation(std::function<void(ValueOrConstRef<OSVersionInformation>)> writeReturnValue)
{
    xstring windowsVersionName = GetOSVersionDescription();
    xstring windowsVersion = GetOSVersionNumber();

    OSVersionInformation versionInfo
    {
        .description = windowsVersionName,
        .version = windowsVersion
    };
    writeReturnValue(versionInfo);
    return MakePresetFuture();
}
