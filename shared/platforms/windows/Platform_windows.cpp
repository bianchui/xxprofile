// Copyright 2019 bianchui. All rights reserved.
#include "shared/platforms/Platform.h"
#include "Platform_windows.h"
#include "shared/platforms/windows/inc_windows.h"

SHARED_NAMESPACE_BEGIN;

uint32_t OSGetVersion() {
    static uint32_t s_version;
    if (!s_version) {
        OSVERSIONINFOEXW osvi;
        memset(&osvi, 0, sizeof(OSVERSIONINFOEXW));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
        if (GetVersionExW((OSVERSIONINFO*)&osvi)) {
            s_version = (uint32_t)((osvi.dwMajorVersion << 24) + (osvi.dwMinorVersion << 16) + osvi.dwBuildNumber);
        }
    }
    return s_version;
}

uint32_t platformGetTid() {
    return ::GetCurrentThreadId();
}

static void SetThreadName(DWORD dwThreadID, const char* threadName) {
    static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)  
    typedef struct tagTHREADNAME_INFO {
        DWORD dwType;     // Must be 0x1000.
        LPCSTR szName;    // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
#pragma warning(pop)
}

void platformSetThreadName(const char* name) {
    SetThreadName(::GetCurrentThreadId(), name);
}

const char* platformAbiName() {
#ifdef _WIN64

#  if defined(_M_AMD64)
    return "amd64";
#  elif defined(_M_ARM64)
    return "arm64";
#  else
    return "unknown64";
#  endif//_M_ARM64

#else//_WIN64

#  if defined(_M_IX86)
    return "x86";
#  elif defined(_M_ARM)
    return "arm32";
#  else
    return "unknown32";
#  endif//_M_ARM

#endif//_WIN64
}

SHARED_NAMESPACE_END;
