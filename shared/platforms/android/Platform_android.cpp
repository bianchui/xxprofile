// Copyright 2018 bianchui. All rights reserved.
#include "shared/platforms/Platform.h"
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/system_properties.h>
#include <pthread.h>

SHARED_NAMESPACE_BEGIN;

uint32_t OSGetVersion() {
    static uint32_t s_Android_SDK_INT;
    if (!s_Android_SDK_INT) {
        char prop[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.sdk", prop);
        s_Android_SDK_INT = atoi(prop);
    }
    return s_Android_SDK_INT;
}

//#define RT_DBG_ABI_BUILD

// https://github.com/googlesamples/android-ndk/blob/master/hello-jni/app/src/main/cpp/hello-jni.c
const char* platformAbiName() {
#if defined(__arm__)

#  if defined(__ARM_ARCH_7A__)
#    if defined(__ARM_NEON__)
#      if defined(__ARM_PCS_VFP)
#        define Android_ABI "armeabi-v7a/NEON (hard-float)"
#      else
#        define Android_ABI "armeabi-v7a/NEON"
#      endif
#    else
#      if defined(__ARM_PCS_VFP)
#        define Android_ABI "armeabi-v7a (hard-float)"
#      else
#        define Android_ABI "armeabi-v7a"
#      endif
#    endif
#  else
#    define Android_ABI "armeabi"
#  endif//not __ARM_ARCH_7A__

#elif defined(__i386__)
#  define Android_ABI "x86"
#elif defined(__x86_64__)
#  define Android_ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#  define Android_ABI "mips64"
#elif defined(__mips__)
#  define Android_ABI "mips"
#elif defined(__aarch64__)
#  define Android_ABI "arm64-v8a"
#else
#  define Android_ABI "unknown"
#endif

#  ifdef RT_DBG_ABI_BUILD
#    pragma message("ABI: " M_EXPEND(Android_ABI))
#  endif//RT_DBG_ABI_BUILD

    return Android_ABI;
#undef Android_ABI
}

uint32_t platformGetTid() {
    return ::gettid();
}

void platformSetThreadName(const char* name) {
    pthread_setname_np(pthread_self(), name);
}

TLSKey platformTLSCreate(TLSValueDestructor des) {
    static_assert(sizeof(pthread_key_t) <= sizeof(TLSKey), "TLSKeySize");
    pthread_key_t thread_key;
    pthread_key_create(&thread_key, des);
    return (TLSKey)(uintptr_t)thread_key;
}

void* platformTLSGet(TLSKey key) {
    return pthread_getspecific((pthread_key_t)(uintptr_t)key);
}

bool platformTLSSet(TLSKey key, void* value) {
    return 0 == pthread_setspecific((pthread_key_t)(uintptr_t)key, value);
}

void platformTLSDestroy(TLSKey key) {
    pthread_key_delete((pthread_key_t)(uintptr_t)key);
}

SHARED_NAMESPACE_END;
