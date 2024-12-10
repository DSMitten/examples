// Matching header
#include "LoggingStartup.h"

// vcpkg headers
#include <nativehost/nh-logging-api/Logging.h>
#include <nativehost/nh-logging-errorCheckCallbacks-api/ErrorCheckCallbacks.h>
#include <nativehost/nh-logging-textFileLogger-api/TextFileLogger.h>
#include <nativehost/misc-process-api/Process.h>
#include <nativehost/misc-strings-api/Strings.h>

#ifdef PLATFORM_MAC
#include <nativehost/nh-logging-nsLogLogger-api/NSLogLogger.h>
#endif

// STL headers
#include <filesystem>

using namespace Microsoft::NativeHost;
using namespace Microsoft::NativeHost::Logging;

void InitializeLogging()
{
    const std::filesystem::path logFilePath = GetCurrentProcessExeDirectoryPath() / L"SharedNativeHostClient.log";
    ConfigureErrorResponsesForLoggingSystem();
    AddLogger(MakeTextFileLogger(logFilePath));
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
