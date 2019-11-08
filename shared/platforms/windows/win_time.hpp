// Copyright 2019 bianchui. All rights reserved.
#ifndef shared_platform_windows_win_time_hpp__
#define shared_platform_windows_win_time_hpp__
#include "shared/SharedMacros.h"
#include <time.h>

SHARED_NAMESPACE_BEGIN;

struct timeval {
    time_t  tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};

int gettimeofday(struct timeval * tp, struct timezone * tzp);

SHARED_NAMESPACE_END;

#endif//shared_platform_windows_win_time_hpp__
