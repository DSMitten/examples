// Matching header
#include "PlatformStartupStep.h"

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

// Forward declare functions that are implemented in platform subdirectories (e.g. win/, mac/)
void PlatformInitialize();
void PlatformShutdown();

[[nodiscard]] std::string_view PlatformStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread PlatformStartupStep::GetStartupThread() const
{
    return StartupThread::MainSynchronous;
}

[[nodiscard]] Future<void> PlatformStartupStep::Start()
{
    PlatformInitialize();
    return MakePresetFuture();
}

[[nodiscard]] Future<void> PlatformStartupStep::Stop()
{
    PlatformShutdown();
    return MakePresetFuture();
}
