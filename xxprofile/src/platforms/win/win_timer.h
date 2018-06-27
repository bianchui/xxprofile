// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_platforms_win_win_timer_h
#define xxprofile_platforms_win_win_timer_h
#include "../platform_base.hpp"
#include "win_inc_windows.h"
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

struct Timer_win : Timer_base {
    static double InitTiming() {
        uint64_t frequency;
        ::QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        secondsPerCycle = 1.0 ／ (double)frequency;
        return Seconds();
    }

    static FORCEINLINE double Seconds() {
        // Add big number to make bugs apparent where return value is being passed to float
        return Cycles64() * GetSecondsPerCycle() + 16777216.0;
    }

    static FORCEINLINE uint64_t Cycles64() {
        uint64_t cycles;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&cycles);
        return cycles;
    }
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_win_win_timer_h
