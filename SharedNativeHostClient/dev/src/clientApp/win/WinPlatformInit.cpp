// Platform headers
#include <Windows.h>

void PlatformInitialize()
{
    (void)CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
}

void PlatformShutdown()
{
    CoUninitialize();
}
