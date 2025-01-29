// Local headers
#include "AppDelegate.h"

// Platform headers
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

void PlatformInitialize()
{
    // Ensure that the shared NSApplication object is created
    [NSApplication sharedApplication];

    // Attach the AppDelegate to get application callbacks
    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    NSApp.delegate = appDelegate;
}

void PlatformShutdown()
{
}
