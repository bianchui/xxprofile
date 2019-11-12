// Copyright 2018 bianchui. All rights reserved.
#ifndef shared_platforms_Platform_h
#define shared_platforms_Platform_h
#include "shared/SharedMacros.h"
#include <stdint.h>
#include "PlatformMacros.h"
#include "PlatformTargets.h"

#if PLATFORM_IS_TARGET(ANDROID)
#  define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#if PLATFORM_IS_TARGET(WINDOWS)

#  include "shared/platforms/windows/win_time.hpp"
#  include "shared/platforms/windows/win_strings.h"

#else
#  include <sys/time.h>
#  include <time.h>

SHARED_NAMESPACE_BEGIN;

// sys/time.h
using ::timeval;
using ::gettimeofday;
using ::localtime_r;
using ::gmtime_r;

SHARED_NAMESPACE_END;

#endif//PLATFORM_IS_TARGET(WINDOWS)

SHARED_NAMESPACE_BEGIN;

// for android platform id
// for ios/macos system version with (majorVersion << 24) + (minorVersion << 16) + patchVersion
// for windows system version with (majorVersion << 24) + (minorVersion << 16) + patchVersion
uint32_t OSGetVersion();

#define IS_OSVERSION_EQUAL_TO_2(major, minor)                 ((shared::OSGetVersion() >> 16) == (((uint32_t)(major) << 8) + (minor)))
#define IS_OSVERSION_GREATER_THAN_2(major, minor)             (shared::OSGetVersion() > ((((uint32_t)(major)) << 24) + (((uint32_t)(minor)) << 16)))
#define IS_OSVERSION_GREATER_THAN_OR_EQUAL_TO_2(major, minor) (shared::OSGetVersion() >= ((((uint32_t)(major)) << 24) + (((uint32_t)(minor)) << 16)))
#define IS_OSVERSION_LESS_THAN_2(v)                           (shared::OSGetVersion() < ((((uint32_t)(major)) << 24) + (((uint32_t)(minor)) << 16)))
#define IS_OSVERSION_LESS_THAN_OR_EQUAL_TO_2(v)               (shared::OSGetVersion() <= ((((uint32_t)(major)) << 24) + (((uint32_t)(minor)) << 16)))

#define OSVERSION_MAJOR() ((uint32_t)(shared::OSGetVersion() >> 24))
#define OSVERSION_MINOR() ((uint32_t)(shared::OSGetVersion() >> 16) & 0xff)

uint32_t platformGetTid();
const char* platformAbiName();
void platformSetThreadName(const char* name);
bool platformIsDebugable();

SHARED_NAMESPACE_END;

#endif//shared_platforms_Platform_h
