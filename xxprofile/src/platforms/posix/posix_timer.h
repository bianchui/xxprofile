// Copyright 2018 bianchui. All rights reserved.
#ifndef platforms_posix_timer_h
#define platforms_posix_timer_h
#include <time.h>
#include "../platform_base.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct XXProfileTimer_posix : XXProfileTimer_base {
    static double InitTiming() {
        secondsPerCycle = 0.000000001;
        return Seconds();
    }

    static FORCEINLINE double Seconds() {
        // Add big number to make bugs apparent where return value is being passed to float
        return Cycles64() * GetSecondsPerCycle() + 16777216.0;
    }

    static FORCEINLINE uint64_t Cycles64() {
        struct timespec tp;
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        uint64_t Cycles = ((uint64_t)tp.tv_sec) * 100000000 + tp.tv_nsec;
        return Cycles;
    }
};

XX_NAMESPACE_END(xxprofile);

#endif//platforms_posix_timer_h
