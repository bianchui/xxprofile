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

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t systemGetTid() {
    return ::syscall(SYS_thread_selfid);
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
