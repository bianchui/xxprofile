// Copyright 2018 bianchui. All rights reserved.
#ifndef shared_platforms_PlatformTargets_h__
#define shared_platforms_PlatformTargets_h__

#define PLATFORM_TARGET_UNKNOWN 0
#define PLATFORM_TARGET_IOS 1
#define PLATFORM_TARGET_ANDROID 2
#define PLATFORM_TARGET_WINDOWS 3
#define PLATFORM_TARGET_MACOS 4

#if defined(__APPLE__)
#  include <TargetConditionals.h>

// macos
#  if TARGET_OS_MAC
#    undef  PLATFORM_TARGET_PLATFORM
#    define PLATFORM_TARGET_PLATFORM PLATFORM_TARGET_MACOS
#  endif

// ios
#  if TARGET_OS_IOS
#    undef  PLATFORM_TARGET_PLATFORM
#    define PLATFORM_TARGET_PLATFORM PLATFORM_TARGET_IOS
#  endif

#endif

// android
#if defined(ANDROID)
#  undef  PLATFORM_TARGET_PLATFORM
#  define PLATFORM_TARGET_PLATFORM PLATFORM_TARGET_ANDROID
#endif

// windows
#if defined(_WIN32) && defined(_WINDOWS)
#  undef  PLATFORM_TARGET_PLATFORM
#  define PLATFORM_TARGET_PLATFORM PLATFORM_TARGET_WINDOWS
#endif

#define PLATFORM_IS_TARGET(target) (PLATFORM_TARGET_PLATFORM == PLATFORM_TARGET_##target)


#endif//shared_platforms_PlatformTargets_h__
