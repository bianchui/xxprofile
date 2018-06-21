// Copyright 2017 bianchui. All rights reserved.
#include "platform_android.hpp"
#include <sys/types.h>
#include <time.h>
#include <chrono>

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t GetTid() {
    return ::gettid();
}

double XXProfileTimer_android::InitTiming(void) {
    // Time base is in nano seconds.
    mach_timebase_info_data_t Info;
    mach_timebase_info(&Info);
    secondsPerCycle = 1e-9 * (double)Info.numer / (double)Info.denom;
    return XXProfileTimer::Seconds();
}

XX_NAMESPACE_END(xxprofile);
