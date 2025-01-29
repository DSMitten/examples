// Matching header
#include "LoggingStartup.h"

// Local headers
#include "../AppDirectories.h"
#include "../CommonMain.h"

// vcpkg headers
#include <nativehost/misc-filesystem-api/Directory.h>
#include <nativehost/misc-filesystem-api/FilenameUtils.h>
#include <nativehost/misc-process-api/Process.h>
#include <nativehost/misc-strings-api/Strings.h>
#include <nativehost/nh-logging-api/Logging.h>
#include <nativehost/nh-logging-errorCheckCallbacks-api/ErrorCheckCallbacks.h>
#include <nativehost/nh-logging-textFileLogger-api/TextFileLogger.h>

#ifdef PLATFORM_MAC
#include <nativehost/nh-logging-nsLogLogger-api/NSLogLogger.h>
#endif

// STL headers
#include <chrono>
#include <filesystem>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Logging;

bool EnsureLogFilesPath()
{
    try
    {
        EnsureDirectoryExists(GetLogFilesPath());
    }
    catch (std::filesystem::filesystem_error&)
    {
        // If there was an error, we don't want the exception to bubble up.
        // Just report failure in the return value.
        return false;
    }

    return true;
}

void InitializeLogging()
{
    const bool ensuredLogFilesPath = EnsureLogFilesPath();
    if (ensuredLogFilesPath)
    {
        #if defined(PLATFORM_WIN)
        const std::wstring appName { c_AppName };
        #elif defined(PLATFORM_POSIX)
        const std::wstring appName { ToWString(c_AppName) };
        #endif
        const std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
        const std::filesystem::path logFilePath = GetLogFilesPath() / GetDateEncodedFilename(appName, std::nullopt, currentTime, L"log", FileNameParams::Time);
        std::unique_ptr<ILogger> spTextFileLogger = MakeTextFileLogger(logFilePath);
        AddLogger(std::move(spTextFileLogger));
    }

    ConfigureErrorResponsesForLoggingSystem();
#ifdef PLATFORM_MAC
    AddLogger(MakeNSLogLogger());
#endif
    SetLogLevel(LogLevel::Verbose);
}

void ShutdownLogging()
{
    ClearLoggers();
}

// ----------------------------------------------------------------------------
// Custom handler replaces std::terminate and allows us to flush the log file.
// Earlier examples didn't require this because they were writing to the
// console.
// ----------------------------------------------------------------------------
void CustomTerminateHandler()
{
    using namespace Microsoft::NativeHost::Logging;
    ITextFileLogger& textFileLogger = dynamic_cast<ITextFileLogger&>(GetLogger(c_TextFileLoggerId));
    textFileLogger.FlushToDisk();
    std::abort();
}
