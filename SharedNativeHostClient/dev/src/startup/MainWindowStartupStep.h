#pragma once

// vcpkg header files
#include <nativehost/nh-startup-api/Startup.h>
#include <nativehost/misc-async-api/Async.h>

// STL headers
#include <string_view>

// Sets up early platform initialization steps
class MainWindowStartupStep : public Microsoft::NativeHost::Startup::StartupStep
{
public:
    constexpr static std::string_view c_Name{ "MainWindow" };

private:
    [[nodiscard]] std::string_view GetName() const override;
    [[nodiscard]] Microsoft::NativeHost::Startup::StartupThread GetStartupThread() const override;
    [[nodiscard]] std::span<const std::string_view> GetServiceDependencies() const override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Start() override;
};
