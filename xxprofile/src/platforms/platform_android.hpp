// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_android_hpp
#define xxprofile_platform_android_hpp
#include "platform_base.hpp"
#include "posix/pthread_SystemLock.h"
#include "posix/posix_timer.h"
#include "posix/pthread_ThreadLocal.h"

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t GetTid();

typedef SystemLock_pthread SystemLock;
typedef Timer_posix Timer;

template <typename T>
struct ThreadLocal : public ThreadLocal_pthread<T> {
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_android_hpp
