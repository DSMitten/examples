#pragma once

// vcpkg header files
#include <nativehost/nh-startup-api/Startup.h>
#include <nativehost/misc-async-api/Async.h>
#include <nativehost/nh-interop-api/InteropService.h>

// STL headers
#include <string_view>
#include <vector>

// Makes interop objects available to the web application
class InteropObjectsStartupStep : public Microsoft::NativeHost::Startup::StartupStep
{
public:
    constexpr static std::string_view c_Name{ "InteropObjects" };

private:
    std::vector<std::shared_ptr<Microsoft::NativeHost::Interop::InteropObject>> m_interopObjects;

    [[nodiscard]] std::string_view GetName() const override;
    [[nodiscard]] Microsoft::NativeHost::Startup::StartupThread GetStartupThread() const override;
    [[nodiscard]] std::span<const std::string_view> GetServiceDependencies() const override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Start() override;
};
