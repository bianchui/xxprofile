// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_android_hpp
#define xxprofile_platform_android_hpp
#include <mach/mach_time.h>
#include "platform_base.hpp"

#include "pthread/SystemLock_pthread.h"

XX_NAMESPACE_BEGIN(xxprofile);

typedef CSystemLock_pthread CSystemLock;

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_android_hpp
