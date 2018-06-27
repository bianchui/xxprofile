// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_platforms_posix_posix_timer_h
#define xxprofile_platforms_posix_posix_timer_h
#include <time.h>
#include "../platform_base.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct Timer_posix : Timer_base {
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
        uint64_t cycles = ((uint64_t)tp.tv_sec) * 1000000000 + tp.tv_nsec;
        return cycles;
    }
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_posix_posix_timer_h
