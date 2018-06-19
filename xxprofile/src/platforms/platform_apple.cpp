//
//  xxprofile_apple.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/8.
//  Copyright 2017 bianchui. All rights reserved.
//

#include "platform_apple.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

double XXProfileTimer_apple::InitTiming(void) {
    // Time base is in nano seconds.
    mach_timebase_info_data_t Info;
    mach_timebase_info( &Info );
    secondsPerCycle = 1e-9 * (double)Info.numer / (double)Info.denom;
    return XXProfileTimer::Seconds();
}

XX_NAMESPACE_END(xxprofile);
