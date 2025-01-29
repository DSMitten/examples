// Local headers
#include "../CommonMain.h"

// Platform headers
#include <Windows.h>

// ----------------------------------------------------------------------------
// Platform-specific entry point into the application, calls the cross-platform
// entry point equivalent.
// ----------------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    return CommonMain();
}
