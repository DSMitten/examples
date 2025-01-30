// Matching header
#include "TasksSystemStartupStep.h"

// Local headers
#include "ServicesRegistrarStartupStep.h"

// vcpkg headers
#include <nativehost/nh-tasks-api/TasksSystem.h>

// STL headers
#include <array>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

[[nodiscard]] std::string_view TasksSystemStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread TasksSystemStartupStep::GetStartupThread() const
{
    return StartupThread::MainSynchronous;
}

[[nodiscard]] std::span<const std::string_view> TasksSystemStartupStep::GetDependencies() const
{
    static constexpr std::array c_Dependencies{ ServicesRegistrarStartupStep::c_Name };
    return c_Dependencies;
}

[[nodiscard]] std::span<const std::string_view> TasksSystemStartupStep::GetServicesProvided() const
{
    static constexpr std::array c_ServicesProvided{ WellKnownServices::Tasks };
    return c_ServicesProvided;
}

[[nodiscard]] Future<void> TasksSystemStartupStep::Start()
{
    StartTasksSystem();
    RegisterTasksService();
    return MakePresetFuture();
}

[[nodiscard]] Future<void> TasksSystemStartupStep::Stop()
{
    UnregisterTasksService();
    ShutdownTasksSystem();
    return MakePresetFuture();
}
