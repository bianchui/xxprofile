#include "win_time.hpp"
#include "inc_windows.h"
#include <stdint.h>

SHARED_NAMESPACE_BEGIN;

// https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows/26085827#26085827
int gettimeofday(struct timeval * tp, struct timezone * tzp) {
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME system_time;
	union {
		ULARGE_INTEGER ll;
		FILETIME ft;
	} time;
 
    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &time.ft);
    tp->tv_sec = (time_t)((time.ll.QuadPart - EPOCH) / 10000000L);
    tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
    return 0;
}

SHARED_NAMESPACE_END;
