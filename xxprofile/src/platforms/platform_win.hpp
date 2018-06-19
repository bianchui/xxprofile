// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofile_platform_win_h__
#define xxprofile_platform_win_h__
#include "../../xxprofile_macros.hpp"
#include "win/platform_inc_windows.h"

XX_NAMESPACE_BEGIN(xxprofile);

class CSystemLock {
public:
	CSystemLock() {
#ifndef WP8
		::InitializeCriticalSectionAndSpinCount(&_cs, 0x80001000);
#else//WP8
		::InitializeCriticalSectionEx(&_cs, 0x1000, CRITICAL_SECTION_NO_DEBUG_INFO);
#endif//
	}

	~CSystemLock() {
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

public:
	DISALLOW_COPY_AND_ASSIGN(CSystemLock);
	CRITICAL_SECTION _cs;
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_win_h__
