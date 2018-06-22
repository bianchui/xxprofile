//
//  xxprofile_apple.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/8.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_platform_apple_hpp
#define xxprofile_platform_apple_hpp
#include <mach/mach_time.h>
#include "platform_base.hpp"
#include "posix/pthread_SystemLock.h"

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t GetTid();

struct XXProfileTimer_apple : XXProfileTimer_base {
    static double InitTiming();
    
    static FORCEINLINE double Seconds() {
        uint64_t cycles = mach_absolute_time();
        // Add big number to make bugs apparent where return value is being passed to float
        return cycles * GetSecondsPerCycle() + 16777216.0;
    }

    static FORCEINLINE uint64_t Cycles64() {
        uint64_t cycles = mach_absolute_time();
        return cycles;
    }
};

typedef CSystemLock_pthread CSystemLock;
typedef XXProfileTimer_apple XXProfileTimer;

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_apple_hpp
