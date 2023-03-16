//
//  xxprofile.cpp
//  xxprofile
//
//  Created by bianchui on 2017/11/7.
//  Copyright 2017 bianchui. All rights reserved.
//
#include "xxprofile_internal.hpp"
#include "xxprofile.hpp"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "xxprofile_tls.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

SystemLock g_mutex;
static ThreadLocal<XXProfileTLS> g_profile_tls;
static SharedArchive* g_archive;

static void StaticCloseGArchive() {
    SystemScopedLock lock(g_mutex);
    if (g_archive) {
        g_archive->markClose();
        g_archive->release();
        g_archive = nullptr;
    }
}

static void StaticInitUnSafe(const char* savePath) {
    if (!g_archive) {
        std::string filePath;
        if (savePath) {
            filePath.assign(savePath);
            if (filePath.length() > 0 && filePath.back() != '/') {
                filePath.push_back('/');
            }
        } else {
            filePath = systemGetWritablePath();
        }
        char timeBuf[64];
#define XXProfile_TimeFormat "[%04d-%02d-%02d-%02d-%02d-%02d.%03d]"
#define XXProfile_TImeArgs (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, (int)(tv.tv_usec / 1000)
        struct timeval tv;
        gettimeofday(&tv, 0);
        struct tm lt;
        localtime_r(&tv.tv_sec, &lt);
        snprintf(timeBuf, 64, XXProfile_TimeFormat, XXProfile_TImeArgs);
        timeBuf[63] = 0;
        filePath.append(timeBuf);
        filePath.append(systemGetAppName());
        filePath.append(".xxprofile");
        g_archive = new SharedArchive(filePath.c_str());
        atexit(StaticCloseGArchive);
    }
}

// XXProfile
bool XXProfile::StaticInit(const char* savePath) {
    SystemScopedLock lock(g_mutex);
    StaticInitUnSafe(savePath);
    return true;
}

void XXProfile::StaticUninit() {
    StaticCloseGArchive();
}

XXProfileTLS* XXProfileTLS::Get() {
    XXProfileTLS* profile = nullptr;
    profile = g_profile_tls.get();
    if (!profile) {
        SystemScopedLock lock(g_mutex);
        StaticInitUnSafe(nullptr);
        if (g_archive) {
            profile = new XXProfileTLS(g_archive);
            g_profile_tls.set(profile);
        }
    }
    return profile;
}

void XXProfileTLS::Clear() {
    XXProfileTLS* profile = nullptr;
    profile = g_profile_tls.get();
    if (profile) {
        g_profile_tls.set(nullptr);
        delete profile;
    }
}

bool XXProfile::IncreaseFrame() {
    XXProfileTLS* profile = XXProfileTLS::Get();
    if (!profile) {
        return false;
    }
    return profile->increaseFrame();
}

// XXProfileScope
XXProfileScope::XXProfileScope(const SName name) {
    auto tls = XXProfileTLS::Get();
    if (tls && tls->canEndFrame() && tls->isClosing()) {
        XXProfileTLS::Clear();
        tls = XXProfileTLS::Get();
    }
    _profile = tls;
    assert(_profile);
    if (_profile) {
        _node = static_cast<XXProfileTLS*>(_profile)->beginScope(name);
    } else {
        _node = nullptr;
    }
}

XXProfileScope::~XXProfileScope() {
    if (_profile) {
        static_cast<XXProfileTLS*>(_profile)->endScope(_node);
        if (static_cast<XXProfileTLS*>(_profile)->isClosing()) {
            XXProfileTLS::Clear();
        }
    }
}

XX_NAMESPACE_END(xxprofile);
