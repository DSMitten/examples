// Matching header
#include "CommonMain.h"

// Local headers
#include "startup/LoggingStartup.h"
#include "startup/PlatformStartupStep.h"
#include "startup/ServicesRegistrarStartupStep.h"
#include "startup/StartupShutdown.h"
#include "startup/TasksSystemStartupStep.h"
#include "startup/WebHostSystemStartupStep.h"
#include "startup/MainWindowStartupStep.h"

// Vcpkg headers
#include <nativehost/misc-errors-api/Errors.h>
#include <nativehost/misc-eventLoop-api/EventLoop.h>
#include <nativehost/misc-raii/ScopeGuard.h>

// STL headers
#include <exception>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

// ----------------------------------------------------------------------------
// Main routine runs the various application life cycle steps.
// ----------------------------------------------------------------------------
int CommonMain()
{
    const ScopeGuard loggingSystemScopeGuard = ShutdownLogging;

    const int result = DefaultExceptionHandler(
        []() -> void
        {
            InitializeLogging();
            std::set_terminate(CustomTerminateHandler);

            AddStartupStep<PlatformStartupStep>();
            AddStartupStep<ServicesRegistrarStartupStep>();
            AddStartupStep<TasksSystemStartupStep>();
            AddStartupStep<WebHostSystemStartupStep>();
            AddStartupStep<MainWindowStartupStep>();
            RunDoStartup();

            DoEventLoop();
        });

    if (WasExceptionCaught(result))
    {
        return result;
    }

    return GetShutdownExitCode();
}
