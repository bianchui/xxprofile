// Copyright 2019 bianchui. All rights reserved.
#ifndef platform_windows_Platform_windows_h__
#define platform_windows_Platform_windows_h__
#include "shared/SharedMacros.h"
#include "shared/platforms/windows/win_time.hpp"

#define __unused

#define gettimeofday(x, y) ::shared::gettimeofday(x, y)
#define localtime_r(a, b)  localtime_s(b, a)
#define gmtime_r(a, b)     gmtime_s(b, a)

#endif // !platform_windows_Platform_windows_h__
