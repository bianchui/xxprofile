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
#include "posix/pthread_ThreadLocal.h"

XX_NAMESPACE_BEGIN(xxprofile);

struct Timer_apple : Timer_base {
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

typedef SystemLock_pthread SystemLock;
typedef Timer_apple Timer;

template <typename T>
struct ThreadLocal : public ThreadLocal_pthread<T> {
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_apple_hpp
