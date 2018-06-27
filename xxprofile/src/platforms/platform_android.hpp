// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_android_hpp
#define xxprofile_platform_android_hpp
#include "platform_base.hpp"
#include "posix/pthread_SystemLock.h"
#include "posix/posix_timer.h"

XX_NAMESPACE_BEGIN(xxprofile);

typedef SystemLock_pthread SystemLock;
typedef Timer_posix Timer;

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_android_hpp
