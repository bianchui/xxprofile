// Copyright 2019 bianchui. All rights reserved.
#ifndef shared_time_conv_h
#define shared_time_conv_h
#include <shared/SharedMacros.h>
#include <time.h>

SHARED_NAMESPACE_BEGIN;

struct TimeConv {
    static constexpr int32_t S_TO_MS = 1000;
    static constexpr int32_t S_TO_US = 1000 * 1000;
    static constexpr int32_t S_TO_NS = 1000 * 1000 * 1000;
    
    static constexpr int32_t MS_TO_US = 1000;
    static constexpr int32_t MS_TO_NS = 1000 * 1000;
    
    static constexpr int32_t US_TO_NS = 1000;
};

template <typename T0, typename T1>
forceinline void timespec_to_timeval(const T0& i, T1* o) {
    o->tv_sec = i.tv_sec;
    o->tv_usec = (int32_t)(i.tv_nsec / TimeConv::US_TO_NS);
}

template <typename T>
forceinline void timespec_gettime(T* tv) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_nsec = ts.tv_nsec;
}

template <typename T>
forceinline void timeval_gettime(T* tv) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = (int32_t)(ts.tv_nsec / TimeConv::US_TO_NS);
}

template <typename T0, typename T1>
forceinline int64_t timespec_sub_us(const T0& end, const T1& start) {
    return ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * TimeConv::S_TO_US + ((int64_t)end.tv_nsec - (int64_t)start.tv_nsec) / TimeConv::US_TO_NS;
}

template <typename T0, typename T1>
forceinline int64_t timespec_sub_ms(const T0& end, const T1& start) {
    return ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * TimeConv::S_TO_MS + ((int64_t)end.tv_nsec - (int64_t)start.tv_nsec) / TimeConv::MS_TO_NS;
}

template <typename T0, typename T1>
forceinline int64_t timeval_sub_us(const T0& end, const T1& start) {
    return ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * TimeConv::S_TO_US + ((int64_t)end.tv_usec - (int64_t)start.tv_usec);
}

template <typename T0, typename T1>
forceinline int64_t timeval_sub_ms(const T0& end, const T1& start) {
    return ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * TimeConv::S_TO_MS + ((int64_t)end.tv_usec - (int64_t)start.tv_usec) / TimeConv::MS_TO_US;
}

template <typename T>
forceinline void timespec_add_ns(T& tv, uint64_t add_ns) {
    tv.tv_nsec += add_ns;
    uint64_t sec = (tv.tv_usec / TimeConv::S_TO_NS);
    tv.tv_nsec = (tv.tv_nsec % TimeConv::S_TO_NS);
    tv.tv_sec += sec;
}

template <typename T>
forceinline void timespec_add_us(T& tv, uint64_t add_us) {
    timespec_add_ns<T>(tv, add_us * TimeConv::US_TO_NS);
}

template <typename T>
forceinline void timespec_add_ms(T& tv, uint64_t add_ms) {
    timespec_add_ns<T>(tv, add_ms * TimeConv::MS_TO_NS);
}

template <typename T>
forceinline void timeval_add_us(T& tv, uint64_t add_us);

template <typename T>
forceinline void timeval_add_ns(T& tv, uint64_t add_ns) {
    timeval_add_us<T>(tv, add_ns / TimeConv::US_TO_NS);
}

template <typename T>
forceinline void timeval_add_us(T& tv, uint64_t add_us) {
    tv.tv_usec += add_us;
    uint64_t sec = (tv.tv_usec / TimeConv::S_TO_US);
    tv.tv_usec = (tv.tv_usec % TimeConv::S_TO_US);
    tv.tv_sec += sec;
}

template <typename T>
forceinline void timeval_add_ms(T& tv, uint64_t add_ms) {
    timeval_add_us<T>(tv, add_ms * TimeConv::MS_TO_US);
}

template <typename T0, typename T1>
forceinline bool timespec_less(const T0& tv0, const T1& tv1) {
    return tv0.tv_sec < tv1.tv_sec || (tv0.tv_sec == tv1.tv_sec && tv0.tv_nsec < tv1.tv_nsec);
}

template <typename T0, typename T1>
forceinline bool timeval_less(const T0& tv0, const T1& tv1) {
    return tv0.tv_sec < tv1.tv_sec || (tv0.tv_sec == tv1.tv_sec && tv0.tv_usec < tv1.tv_usec);
}

SHARED_NAMESPACE_END;

#endif//shared_time_conv_h
