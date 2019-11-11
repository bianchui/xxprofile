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

SHARED_NAMESPACE_END;
