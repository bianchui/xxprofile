// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_win_h__
#define xxprofile_platform_win_h__
#include "../xxprofile_macros.hpp"
#include "win/win_SystemLock.h"
#include "win/win_timer.h"
#include "win/win_ThreadLocal.h"
#include <string>

XX_NAMESPACE_BEGIN(xxprofile);

FORCEINLINE uint32_t systemGetTid() {
    return ::GetCurrentThreadId();
}

FORCEINLINE std::string systemGetWritablePath() {
    char buf[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buf);
    return buf;
}

FORCEINLINE std::string systemGetAppName() {
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return buf;
}

typedef SystemLock_win SystemLock;
typedef Timer_win Timer;

template <typename T>
struct ThreadLocal : public ThreadLocal_win<T> {
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_win_h__
