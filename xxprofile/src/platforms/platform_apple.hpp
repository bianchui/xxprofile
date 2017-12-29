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

#include "SystemLock_pthread.h"
typedef CSystemLock_pthread CSystemLock;

struct XXProfileTimer_apple : XXProfileTimer_base {
    static double InitTiming();
    
    static FORCEINLINE double Seconds() {
        uint64_t Cycles = mach_absolute_time();
        // Add big number to make bugs apparent where return value is being passed to float
        return Cycles * GetSecondsPerCycle() + 16777216.0;
    }
    
    static FORCEINLINE uint64_t Cycles64() {
        uint64_t Cycles = mach_absolute_time();
        return Cycles;
    }
    
    //static FCPUTime GetCPUTime();
};

typedef XXProfileTimer_apple XXProfileTimer;

#endif//xxprofile_platform_apple_hpp
