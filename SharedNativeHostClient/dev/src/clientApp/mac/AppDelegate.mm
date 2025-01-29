// Matching header
#import "AppDelegate.h"

// Platform headers
#import <Cocoa/Cocoa.h>

// AppDelegate class implementation
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)event
{
    NSLog(@"ApplicationFinishedLaunching");
}

- (void)applicationWillTerminate:(NSNotification*)event
{
    NSLog(@"ApplicationWillTerminate");
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    NSLog(@"ApplicationShouldTerminateAfterLastWindowClosed");
    return NO;
}

@end