// Matching header
#include "MainWindowStartupStep.h"

// Local headers
#include "CommonMain.h"
#include "WebHostSystemStartupStep.h"
#include "startup/StartupShutdown.h"

// vcpkg headers
#include <nativehost/nh-webHost-api/WebHostManager.h>

// STL headers
#include <array>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Startup;

// Sets up early platform initialization steps
[[nodiscard]] std::string_view MainWindowStartupStep::GetName() const
{
    return c_Name;
}

[[nodiscard]] StartupThread MainWindowStartupStep::GetStartupThread() const
{
    return StartupThread::UI;
}

[[nodiscard]] std::span<const std::string_view> MainWindowStartupStep::GetServiceDependencies() const
{
    static constexpr std::array c_Dependencies{ WellKnownServices::WebHostManager };
    return c_Dependencies;
}

[[nodiscard]] Future<void> MainWindowStartupStep::Start()
{
    // Replace with app uri
    constexpr zxstring_view c_MainWindowUri{ XSTR("http://www.bing.com") };

    const MessageSourceValidator sourceValidator =
        [c_MainWindowUri](xstring_view messageSource, xstring_view /*viewSource*/) -> bool
        {
            const auto sourcePath = std::filesystem::path(messageSource);
            const auto mainWindowPath = std::filesystem::path(xstring_view{ c_MainWindowUri });
            return sourcePath == mainWindowPath;
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
            .Uri = xstring{ c_MainWindowUri },
            .MessageSourceValidator = sourceValidator
        }
    };
    // clang-format on

    (void)co_await GetWebHostManager().CreateWebHost(newWebHostSpec);
}
