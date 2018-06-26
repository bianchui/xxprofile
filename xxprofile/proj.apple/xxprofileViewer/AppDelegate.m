//
//  AppDelegate.m
//  xxprofileViewer
//
//  Created by bianchui on 2018/6/26.
//  Copyright © 2018年 bianchui. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
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

@end
