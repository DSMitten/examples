#pragma once

// vcpkg header files
#include <nativehost/misc-async-api/Async.h>
#include <nativehost/nh-services-api/Services.h>
#include <nativehost/nh-startup-api/Startup.h>

// STL headers
#include <string_view>

// Sets up early platform initialization steps
class TasksSystemStartupStep : public Microsoft::NativeHost::Startup::StartupStep
{
public:
    constexpr static std::string_view c_Name{ "TasksSystem" };

private:
    [[nodiscard]] std::string_view GetName() const override;
    [[nodiscard]] Microsoft::NativeHost::Startup::StartupThread GetStartupThread() const override;
    [[nodiscard]] std::span<const std::string_view> GetDependencies() const override;
    [[nodiscard]] std::span<const std::string_view> GetServicesProvided() const override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Start() override;
    [[nodiscard]] Microsoft::NativeHost::Future<void> Stop() override;
};
