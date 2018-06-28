//
//  AppDelegate.m
//  xxprofileViewer
//
//  Created by bianchui on 2018/6/26.
//  Copyright © 2018年 bianchui. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()
@property (nonatomic, retain) NSTimer *renderTimer;
@property (nonatomic, readwrite) int fps;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    NSLog(@"applicationWillTerminate");
}

- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename {
    filename = [filename stringByStandardizingPath];
    NSLog(@"%@", filename);
    NSString* path = [filename stringByDeletingLastPathComponent];
    NSString* resourcesDir = [path stringByAppendingPathComponent:@"Resources"];

    NSString* bid = [[filename lastPathComponent] stringByDeletingPathExtension];
    NSString* dataDir = [[path stringByAppendingPathComponent:@"data"] stringByAppendingPathComponent:bid];

    NSString* plistFilename = @"playerconfig.plist";
    NSString* configPath = [dataDir stringByAppendingPathComponent:plistFilename];
    NSLog(@"Bid %@", bid);
    //self.title = bid;
    return YES;
}

- (void)timerFired:(id)sender {
}

- (void)tickEnd {
    [_renderTimer invalidate];
    _renderTimer = nil;
}

- (void)setFps:(int)fps {
    _fps = fps;
    if (_renderTimer != nil) {
        [_renderTimer invalidate];
        _renderTimer = nil;
    }
    _renderTimer = [NSTimer timerWithTimeInterval:1 / _fps
                                           target:self
                                         selector:@selector(timerFired:)
                                         userInfo:nil
                                          repeats:YES];

    [[NSRunLoop currentRunLoop] addTimer:_renderTimer forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] addTimer:_renderTimer forMode:NSEventTrackingRunLoopMode];
}

@end
