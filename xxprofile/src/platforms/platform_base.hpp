//
//  xxprofile_platform_base.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/9.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_platform_base_hpp
#define xxprofile_platform_base_hpp
#include <stdint.h>
#include "../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

#define FORCEINLINE inline

struct Timer_base {
    //** start interfaces
    //static double InitTiming();
    //static double Seconds();
    //static uint64_t Cycles64();
    //** end interfaces

    static double GetSecondsPerCycle() {
        return secondsPerCycle;
    }

protected:
    static double secondsPerCycle;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_base_hpp
