// Copyright 2008 bianchui. All rights reserved.
#ifndef xxprofile_platforms_win_win_SystemLock_h__
#define xxprofile_platforms_win_win_SystemLock_h__
#include "win_inc_windows.h"
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class CSystemLock_win {
public:
    CSystemLock_win() {
#ifndef WP8
        ::InitializeCriticalSectionAndSpinCount(&_cs, 0x80001000);
#else//WP8
        ::InitializeCriticalSectionEx(&_cs, 0x1000, CRITICAL_SECTION_NO_DEBUG_INFO);
#endif//
    }

    ~CSystemLock_win() {
        ::DeleteCriticalSection(&_cs);
    }

    bool TryLock() {
        return !!::TryEnterCriticalSection(&_cs);
    }

    void Lock() {
        ::EnterCriticalSection(&_cs);
    }

    void Unlock() {
        ::LeaveCriticalSection(&_cs);
    }

protected:
    CRITICAL_SECTION _cs;

    XX_CLASS_DELETE_COPY_AND_MOVE(CSystemLock_win);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_win_win_SystemLock_h__
