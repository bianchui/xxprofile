#ifndef platform_windows_Platform_windows_h__
#define platform_windows_Platform_windows_h__
#include "shared/SharedMacros.h"
#include "shared/platforms/windows/time.hpp"

#define __unused

SHARED_NAMESPACE_BEGIN;

using NS_SHARED::timeval;
using NS_SHARED::gettimeofday;

SHARED_NAMESPACE_END;

#define gettimeofday(x, y) ::egret::gettimeofday(x, y)
#define localtime_r(a, b)  localtime_s(b, a)
#define gmtime_r(a, b)     gmtime_s(b, a)

#endif // !platform_windows_Platform_windows_h__
