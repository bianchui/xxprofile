//
//  xxprofile_platform.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/9.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_platform_hpp
#define xxprofile_platform_hpp

#if defined(PLATFORM_WINDOWS) && PLATFORM_WINDOWS
// win
#  include "platform_win.h"

#elif defined(__APPLE__) && defined(__MACH__)
// ios & mac
#  include <TargetConditionals.h>
#  include "platform_apple.hpp"

#  if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1

#  elif TARGET_OS_MAC == 1

#  endif//APPLE OTHER

#elif defined(ANDROID) || defined(__ANDROID__)
// android
#  include "platform_android.hpp"

#endif//ANDROID

#include <atomic>

XX_NAMESPACE_BEGIN(xxprofile);

class CSystemScopedLock {
public:
    CSystemScopedLock(CSystemLock& Lock) : _Lock(Lock) {
        _Lock.Lock();
    }
    ~CSystemScopedLock() {
        _Lock.Unlock();
    }

private:
    XX_CLASS_DELETE_COPY(CSystemScopedLock);
    XX_CLASS_DELETE_MOVE(CSystemScopedLock);
    CSystemLock& _Lock;
};

static_assert(sizeof(std::atomic<void*>) == sizeof(void*), "sizeof atomic is not same");

template<typename T>
inline T make_align(const T v, size_t align) {
    return (T)(((uintptr_t)v + align - 1) & ~(align - 1));
}

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platform_hpp
