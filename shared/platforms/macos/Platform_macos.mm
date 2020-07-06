// Copyright 2018 bianchui. All rights reserved.
#include "shared/platforms/Platform.h"
#import <Foundation/Foundation.h>
#include <sys/syscall.h>
#include <pthread.h>

SHARED_NAMESPACE_BEGIN;

uint32_t OSGetVersion() {
    static uint32_t s_version;
    if (!s_version) {
        NSOperatingSystemVersion v = [NSProcessInfo processInfo].operatingSystemVersion;
        s_version = (uint32_t)((v.majorVersion << 24) + (v.minorVersion << 16) + v.patchVersion);
    }
    return s_version;
}

uint32_t platformGetTid() {
    return ::syscall(SYS_thread_selfid);
}

void platformSetThreadName(const char* name) {
    pthread_setname_np(name);
}

#pragma mark - TLS

TLSKey platformTLSCreate(TLSValueDestructor des) {
    static_assert(sizeof(pthread_key_t) == sizeof(TLSKey), "TLSKeySize");
    pthread_key_t thread_key;
    pthread_key_create(&thread_key, des);
    return (TLSKey)thread_key;
}

void* platformTLSGet(TLSKey key) {
    return pthread_getspecific((pthread_key_t)key);
}

bool platformTLSSet(TLSKey key, void* value) {
    return 0 == pthread_setspecific((pthread_key_t)key, value);
}

void platformTLSDestroy(TLSKey key) {
    pthread_key_delete((pthread_key_t)key);
}

SHARED_NAMESPACE_END;
