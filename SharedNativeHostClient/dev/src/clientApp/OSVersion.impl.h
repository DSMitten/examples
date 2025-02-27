#pragma once

// Local headers
#include <nativehost/interop/OSVersion.gen.h>

// STL headers
#include <functional>
#include <memory>

class OSVersionImpl : public ClientApp::Interop::OSVersion
{
    [[nodiscard]] Future<void> GetOSVersionInformation(
        std::function<void(::Microsoft::NativeHost::Interop::ValueOrConstRef<::ClientApp::Interop::OSVersionInformation>)> writeReturnValue
    ) override;
};

inline std::shared_ptr<Microsoft::NativeHost::Interop::InteropObject> MakeOSVersionInteropObject()
{
    return std::static_pointer_cast<Microsoft::NativeHost::Interop::InteropObject>(std::make_shared<OSVersionImpl>());
}
