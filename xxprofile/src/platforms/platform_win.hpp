// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_win_h__
#define xxprofile_platform_win_h__
#include "../../xxprofile_macros.hpp"
#include "win/win_SystemLock.h"
#include "win/win_timer.h"

XX_NAMESPACE_BEGIN(xxprofile);

typedef SystemLock_win SystemLock;
typedef Timer_win Timer;

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_win_h__
