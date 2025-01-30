// Matching header
#include "ServicesRegistrarStartupStep.h"

// Local headers
#include "PlatformStartupStep.h"

// vcpkg header files
#include <nativehost/nh-services-api/ServicesRegistrarAdmin.h>

// STL headers
#include <array>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

[[nodiscard]] std::string_view ServicesRegistrarStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread ServicesRegistrarStartupStep::GetStartupThread() const
{
    return StartupThread::MainSynchronous;
}

[[nodiscard]] std::span<const std::string_view> ServicesRegistrarStartupStep::GetDependencies() const
{
    static constexpr std::array c_Dependencies{ PlatformStartupStep::c_Name };
    return c_Dependencies;
}

[[nodiscard]] Future<void> ServicesRegistrarStartupStep::Start()
{
    m_spServicesRegistrar = MakeServicesRegistrar();
    SetRegistrarAccessor(m_spServicesRegistrar);
    return MakePresetFuture();
}

[[nodiscard]] Future<void> ServicesRegistrarStartupStep::Stop()
{
    m_spServicesRegistrar = nullptr;
    return MakePresetFuture();
}
