// Copyright 2019 bianchui. All rights reserved.
#ifndef shared_platforms_windows_win_time_hpp__
#define shared_platforms_windows_win_time_hpp__
#include "shared/SharedMacros.h"
#include <time.h>

SHARED_NAMESPACE_BEGIN;

struct timeval {
    time_t  tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};

int gettimeofday(struct timeval * tp, struct timezone * tzp);

inline struct tm* localtime_r(const time_t* __t, struct tm* __tm) {
	::localtime_s(__tm, __t);
	return __tm;
}

inline struct tm* gmtime_r(const time_t* __t, struct tm* __tm) {
	::gmtime_s(__tm, __t);
	return __tm;
} 

SHARED_NAMESPACE_END;

#endif//shared_platforms_windows_win_time_hpp__
