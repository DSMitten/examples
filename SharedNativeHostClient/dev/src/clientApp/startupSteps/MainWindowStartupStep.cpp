// Matching header
#include "MainWindowStartupStep.h"

// Local headers
#include "CommonMain.h"
#include "WebHostSystemStartupStep.h"
#include "InteropObjectsStartupStep.h"
#include "StartupShutdown.h"

// vcpkg headers
#include <nativehost/nh-webHost-api/WebHostManager.h>
#include <nativehost/misc-uri-api/Uri.h>

// STL headers
#include <array>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

[[nodiscard]] std::string_view MainWindowStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread MainWindowStartupStep::GetStartupThread() const
{
    return StartupThread::UI;
}

[[nodiscard]] std::span<const std::string_view> MainWindowStartupStep::GetDependencies() const
{
    static constexpr std::array c_Dependencies{ InteropObjectsStartupStep::c_Name };
    return c_Dependencies;
}

[[nodiscard]] std::span<const std::string_view> MainWindowStartupStep::GetServiceDependencies() const
{
    static constexpr std::array c_Dependencies{ WellKnownServices::WebHostManager };
    return c_Dependencies;
}

[[nodiscard]] Future<void> MainWindowStartupStep::Start()
{
#ifdef PLATFORM_WIN
    const std::filesystem::path c_MainWindowSiteParentDir{ GetCurrentProcessExeDirectoryPath() };
#elif PLATFORM_MAC
    const std::filesystem::path c_MainWindowSiteParentDir{ GetCurrentProcessExeDirectoryPath().parent_path() / L"Helpers" };
#endif
    xstring c_MainWindowUriPath{ XSTR("file://") + ToXString((c_MainWindowSiteParentDir / XSTR("site") / XSTR("index.html"))) };

    const MessageSourceValidator sourceValidator =
        [mainWindowUri = Uri(c_MainWindowUriPath)](xstring_view messageSource, xstring_view /*viewSource*/) -> bool
        {
            const Uri sourceUri{ messageSource };

            if (sourceUri.Scheme() != mainWindowUri.Scheme())
            {
                return false;
            }
            if (sourceUri.Host() != mainWindowUri.Host())
            {
                return false;
            }
            if (sourceUri.Port() != mainWindowUri.Port())
            {
                return false;
            }
            if (sourceUri.Path() != mainWindowUri.Path())
            {
                return false;
            }

            return true;
        };

    // clang-format off
    NewWebHostSpec newWebHostSpec
    {
        .NewWindowSpec
        {
            .WindowText {c_AppName},
            .StartVisible = true,
            .WindowFrameRect {50, 50, 1000, 800},
        },
        .NewWebView2Spec
        {
            .Uri = std::move(c_MainWindowUriPath),
            .MessageSourceValidator = sourceValidator
        }
    };
    // clang-format on

    (void)co_await GetWebHostManager().CreateWebHost(newWebHostSpec);
}
