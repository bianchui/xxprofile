//
//  xxprofile_apple.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/8.
//  Copyright 2017 bianchui. All rights reserved.
//

#include "platform_apple.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#import <Foundation/Foundation.h>
#include <string>
#include <dlfcn.h>

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t systemGetTid() {
#if 1
    uint64_t tid64;
    pthread_threadid_np(NULL, &tid64);
    return static_cast<uint32_t>(tid64);
#else
    return ::syscall(SYS_thread_selfid);
#endif
}

std::string systemGetAppName() {
    std::string strRet;
    NSString *name = [[[NSBundle mainBundle] infoDictionary] objectForKey:(__bridge NSString*)kCFBundleIdentifierKey];
    if (name != nil) {
        strRet = [name UTF8String];
    } else {
        static int value_ = 0;
        Dl_info info;
        dladdr(&value_, &info);
        const char* fname = strrchr(info.dli_fname, '/');
        if (fname) {
            strRet.assign(fname + 1);
        } else {
            strRet = info.dli_fname;
        }
    }
    return strRet;
}

double Timer_apple::InitTiming(void) {
    // https://developer.apple.com/library/archive/qa/qa1398/_index.html
    // Time base is in nano seconds.
    mach_timebase_info_data_t Info;
    mach_timebase_info(&Info);
    secondsPerCycle = 1e-9 * (double)Info.numer / (double)Info.denom;
    return Timer::Seconds();
}

XX_NAMESPACE_END(xxprofile);
