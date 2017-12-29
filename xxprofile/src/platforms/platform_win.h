#ifndef platform_win_h__
#define platform_win_h__
#include "../xxprofile_build.hpp"
#include "platform_inc_windows.h"

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

#endif//platform_win_h__
