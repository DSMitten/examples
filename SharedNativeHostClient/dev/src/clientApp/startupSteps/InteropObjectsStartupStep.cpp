// Matching header
#include "InteropObjectsStartupStep.h"

// Local headers
#include "CommonMain.h"
#include "WebHostSystemStartupStep.h"
#include "StartupShutdown.h"
#include "../OSVersion.impl.h"

// vcpkg headers
#include <nativehost/misc-uri-api/Uri.h>
#include <nativehost/nh-webHost-api/WebHostManager.h>
#include <nativehost/nh-interop-api/Interop.h>

// STL headers
#include <array>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Interop;
using namespace Microsoft::NativeHost::Startup;

[[nodiscard]] std::string_view InteropObjectsStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread InteropObjectsStartupStep::GetStartupThread() const
{
    return StartupThread::UI;
}

[[nodiscard]] std::span<const std::string_view> InteropObjectsStartupStep::GetServiceDependencies() const
{
    static constexpr std::array c_Dependencies{ WellKnownServices::WebHostManager };
    return c_Dependencies;
}

[[nodiscard]] Future<void> InteropObjectsStartupStep::Start()
{
    std::shared_ptr<InteropObject> spOSVersion = MakeOSVersionInteropObject();
    m_interopObjects.push_back(std::move(spOSVersion));
    co_await GetInteropService()->GetInteropObjects().ProvideInteropObjects(m_interopObjects);
}
