// Matching header
#include "StartupShutdown.h"

// vcpkg headers
#include <nativehost/misc-eventLoop-api/EventLoop.h>
#include <nativehost/nh-startup-api/Startup.h>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

void RunDoShutdown(int exitCode)
{
    DoShutdown()
        .Then(
            [exitCode](StartupStage resultStage)
            {
                if (resultStage == StartupStage::StopFailed)
                {
                    // Shutdown failed. Cause the application to exit with an exit code of -1.
                    CauseShutdown(-1);
                }
                else
                {
                    // Shutdown succeeded. Cause the application to exit with the exit code we were provided with.
                    CauseShutdown(exitCode);
                }
            })
        .Catch(
            [](std::exception_ptr&& exptr)
            {
                // Something in the startup coroutine threw an error.
                // Run shutdown with an exit code of -1
                LogExceptionPtr(exptr);
                CauseShutdown(-1);
            });
}

void RunDoStartup()
{
    DoStartup()
        .Then(
            [](StartupStage resultStage)
            {
                if (resultStage == StartupStage::StopRequested)
                {
                    // Everything works, but a startup step requested that we exit early
                    // Run shutdown with an exit code of -1
                    RunDoShutdown(0);
                }
                else if (resultStage == StartupStage::Failed)
                {
                    // A startup step failed
                    // run shutdown with an exit code of -1
                    RunDoShutdown(-1);
                }
            })
        .Catch(
            [](std::exception_ptr&& exptr)
            {
                // Something in the startup coroutine threw an error.
                // Run shutdown with an exit code of -1
                LogExceptionPtr(exptr);
                RunDoShutdown(-1);
            });
}
