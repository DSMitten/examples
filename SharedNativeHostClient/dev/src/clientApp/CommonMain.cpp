// Matching header
#include "CommonMain.h"

// Local headers
#include "startupSteps/InteropObjectsStartupStep.h"
#include "startupSteps/LoggingStartup.h"
#include "startupSteps/PlatformStartupStep.h"
#include "startupSteps/ServicesRegistrarStartupStep.h"
#include "startupSteps/StartupShutdown.h"
#include "startupSteps/TasksSystemStartupStep.h"
#include "startupSteps/WebHostSystemStartupStep.h"
#include "startupSteps/MainWindowStartupStep.h"

// Repo headers
#include <nativehost/logEvents/ClientApp-logevents.h>
#include <nativehost/nh-logging-api/Logging.h>

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
            Log(ApplicationStart());
            std::set_terminate(CustomTerminateHandler);

            AddStartupStep<PlatformStartupStep>();
            AddStartupStep<ServicesRegistrarStartupStep>();
            AddStartupStep<TasksSystemStartupStep>();
            AddStartupStep<WebHostSystemStartupStep>();
            AddStartupStep<InteropObjectsStartupStep>();
            AddStartupStep<MainWindowStartupStep>();
            RunDoStartup();

            DoEventLoop();
        });

    if (WasExceptionCaught(result))
    {
        return result;
    }

    int exitCode = GetShutdownExitCode();
    Log(ApplicationExit(exitCode));
    return exitCode;
}
