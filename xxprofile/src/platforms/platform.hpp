//
//  xxprofile_platform.hpp
//  xxprofile
//
//  Created by bianchui on 2017/11/9.
//  Copyright (C) 2017, bianchui. All rights reserved.
//

#ifndef xxprofile_platform_hpp
#define xxprofile_platform_hpp

#define XX_TARGET_WINDOWS    1
#define XX_TARGET_MACOS      2
#define XX_TARGET_IOS        3
#define XX_TARGET_ANDROID    4
#define XX_TARGET_LINUX      5
#define XX_TARGET_EMSCRIPTEN 6

#if defined(XX_PLATFORM_WINDOWS) && XX_PLATFORM_WINDOWS
// win
#  include "platform_win.hpp"
#  define XX_TARGET XX_TARGET_WINDOWS

#elif defined(__APPLE__) && defined(__MACH__)
// ios & mac
#  include <TargetConditionals.h>
#  include "platform_apple.hpp"

#  if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
#    define XX_TARGET XX_TARGET_IOS

#  elif TARGET_OS_MAC == 1
#    define XX_TARGET XX_TARGET_MACOS

#  endif // APPLE OTHER

#elif defined(ANDROID) || defined(__ANDROID__)
// android
#  include "platform_android.hpp"
#  define XX_TARGET XX_TARGET_ANDROID

#elif defined(__EMSCRIPTEN__)
// emscripten
#  include "platform_emscripten.hpp"
#  define XX_TARGET XX_TARGET_EMSCRIPTEN

#endif // ANDROID

#define XX_IS_TARGET(x) (XX_TARGET == XX_TARGET_##x)

#ifndef XXPROFILE_HAS_FILE_IO
#  define XXPROFILE_HAS_FILE_IO 1
#endif //XXPROFILE_HAS_FILE_IO

#include <stdio.h>

#if XXPROFILE_HAS_FILE_IO

#  ifdef __EMSCRIPTEN__
#    error XXPROFILE_HAS_FILE_IO is not supported when EMSCRIPTEN is defined
#  endif //__EMSCRIPTEN__

inline FILE* xxopen(const char* name, bool write) {
    return fopen(name, write ? "wb" : "rb");
}
inline void xxclose(FILE* fp) {
    fclose(fp);
}
inline size_t xxwrite(const void* ptr, size_t size, FILE* stream) {
    return fwrite(ptr, 1, size, stream);
}

#else //XXPROFILE_HAS_FILE_IO

#  ifdef XXPROFILE_HAS_DECOMPRESS
#    error XXPROFILE_HAS_DECOMPRESS is not supported when XXPROFILE_HAS_FILE_IO is 0
#  endif //XXPROFILE_HAS_DECOMPRESS

inline FILE* xxopen(const char* name, bool write) {
    return write ? stdout : nullptr;
}
inline void xxclose(FILE* fp) {
}

extern "C" void xxwrite_impl(const void* ptr, size_t size);
inline size_t xxwrite(const void* ptr, size_t size, FILE* stream) {
    xxwrite_impl(ptr, size);
    return size;
}

#endif //XXPROFILE_HAS_FILE_IO

#include <atomic>
#include <string>

XX_NAMESPACE_BEGIN(xxprofile);

std::string systemGetWritablePath();
uint32_t systemGetTid();
void systemSetThreadName(const char* name);
std::string systemGetThreadName();
std::string systemGetAppName();

void log(const char* format, ...);

class SystemScopedLock {
public:
    SystemScopedLock(SystemLock& Lock)
        : _Lock(Lock) {
        _Lock.Lock();
    }
    ~SystemScopedLock() {
        _Lock.Unlock();
    }

private:
    XX_CLASS_DELETE_COPY(SystemScopedLock);
    XX_CLASS_DELETE_MOVE(SystemScopedLock);
    SystemLock& _Lock;
};

static_assert(sizeof(std::atomic<void*>) == sizeof(void*), "sizeof atomic is not same");

template <typename T>
inline T make_align(const T v, size_t align) {
    return (T)(((uintptr_t)v + align - 1) & ~(align - 1));
}

XX_NAMESPACE_END(xxprofile);

#endif // xxprofile_platform_hpp
