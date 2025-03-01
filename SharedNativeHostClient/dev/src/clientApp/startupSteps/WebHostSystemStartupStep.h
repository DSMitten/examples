#pragma once

// vcpkg header files
#include <nativehost/nh-startup-api/Startup.h>
#include <nativehost/misc-async-api/Async.h>
#include <nativehost/misc-events-api/Events.h>

// STL headers
#include <string_view>

// Sets up early platform initialization steps
class WebHostSystemStartupStep : public Microsoft::NativeHost::Startup::StartupStep, public Microsoft::NativeHost::EventSubscriber
{
public:
    constexpr static std::string_view c_Name{ "WebHostSystem" };

private:
    [[nodiscard]] std::string_view GetName() const override;
    [[nodiscard]] Microsoft::NativeHost::Startup::StartupThread GetStartupThread() const override;
    [[nodiscard]] std::span<const std::string_view> GetServicesProvided() const override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Start() override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Stop() override;
};
