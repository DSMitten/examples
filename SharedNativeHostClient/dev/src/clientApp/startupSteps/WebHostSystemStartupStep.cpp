// Matching header
#include "WebHostSystemStartupStep.h"

// Local headers
#include "../AppDirectories.h"
#include "../CommonMain.h"
#include "ServicesRegistrarStartupStep.h"
#include "StartupShutdown.h"

// vcpkg headers
#include <nativehost/misc-filesystem-api/PlatformPaths.h>
#include <nativehost/misc-window-api/Windowing.h>
#include <nativehost/nh-webHost-api/WebHostManager.h>
#include <nativehost/nh-webHost-api/WebHostSystem.h>

// STL headers
#include <array>
#include <filesystem>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;
using namespace Microsoft::NativeHost::WindowManagement;

// Sets up early platform initialization steps
[[nodiscard]] std::string_view WebHostSystemStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread WebHostSystemStartupStep::GetStartupThread() const
{
    return StartupThread::UI;
}

[[nodiscard]] std::span<const std::string_view> WebHostSystemStartupStep::GetServiceDependencies() const
{
    static constexpr std::array c_Dependencies{ WellKnownServices::Tasks };
    return c_Dependencies;
}

[[nodiscard]] std::span<const std::string_view> WebHostSystemStartupStep::GetServicesProvided() const
{
    static constexpr std::array c_ServicesProvided{ WellKnownServices::WebHostManager };
    return c_ServicesProvided;
}

[[nodiscard]] Future<void> WebHostSystemStartupStep::Start()
{
    const std::filesystem::path userDataDir = GetAppUserDataDirectoryPath();
    #if defined(PLATFORM_WIN)
        const xstring userDataDirString = userDataDir.wstring();
        const std::optional<zxstring_view> browserExecutableFolder = std::nullopt;
    #elif defined(PLATFORM_MAC)
        const xstring userDataDirString = userDataDir.string();
        const std::optional<zxstring_view> browserExecutableFolder = WEBVIEW2_APP_PATH;
    #endif

    std::unique_ptr<WebHostSystemStartupOptions> spWebHostSystemStartupOptions = std::make_unique<WebHostSystemStartupOptions>(
        c_AppName,                                // appName
        userDataDirString,                        // userDataFolder
        browserExecutableFolder,                  // browserExecutableFolder
        std::nullopt,                             // iconResourceId
        Interop::GetTasksLibraryThreadingConfig() // spThreadingConfig
    );
    
    co_await StartWebHostSystem(std::move(spWebHostSystemStartupOptions));

    WindowManager windowManager = GetWebHostManager().GetWindowManager();

    // Don't auto-exit the application when the window manager is being destructed;
    // that would interfere with the rest of shutdown executing
    windowManager.SetExitAppOnDestruction(false);

    EventRegs += windowManager.AddAllWindowsDestroyedEventHandler(
        [](WindowManager&, IEmptyEventArgs&)
        {
            // When all windows go away, shut down the application
            RunDoShutdown(0);
        });
}

[[nodiscard]] Future<void> WebHostSystemStartupStep::Stop()
{
    ShutdownWebHostSystem();
    return MakePresetFuture();
}
