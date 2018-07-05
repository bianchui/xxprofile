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
#include <unistd.h>
#include <pthread.h>
#include "xxprofile_tls.hpp"

#define XX_ThreadLocal 1

XX_NAMESPACE_BEGIN(xxprofile);

#if !XX_ThreadLocal
static pthread_key_t g_profile_tls_key;
//static pthread_once_t g_profile_init_once = PTHREAD_ONCE_INIT;
#else//XX_ThreadLocal
static ThreadLocal<XXProfileTLS> g_profile_tls;
#endif//XX_ThreadLocal

// XXProfile
#if XX_ThreadLocal
static void profile_on_thread_exit(void* data) {
    XXProfileTLS* profile = (XXProfileTLS*)data;
    delete profile;
}

//static void profile_tls_init_once() {
//}
#endif//XX_ThreadLocal

static std::string g_filePath;

bool XXProfile::StaticInit() {
    
#if !XX_ThreadLocal
    //pthread_once(&g_profile_init_once, profile_tls_init_once);
    if (!g_profile_tls_key) {
        pthread_key_create(&g_profile_tls_key, profile_on_thread_exit);
    }
#endif//XX_ThreadLocal
    Timer::InitTiming();
    g_filePath = systemGetWritablePath();
    return true;
}

void XXProfile::StaticUninit() {
#if !XX_ThreadLocal
    if (g_profile_tls_key) {
        XXProfileTLS* profile = (XXProfileTLS*)pthread_getspecific(g_profile_tls_key);
        if (profile) {
            delete profile;
            pthread_setspecific(g_profile_tls_key, NULL);
        }
    }
    pthread_key_delete(g_profile_tls_key);
    g_profile_tls_key = 0;
#endif//XX_ThreadLocal
}

XXProfileTLS* XXProfileTLS::Get() {
    XXProfileTLS* profile = NULL;
#if XX_ThreadLocal
    profile = g_profile_tls.get();
    if (!profile) {
        profile = new XXProfileTLS(g_filePath.c_str());
        g_profile_tls.set(profile);
    }
#else//XX_ThreadLocal
    if (g_profile_tls_key) {
        profile = (XXProfileTLS*)pthread_getspecific(g_profile_tls_key);
        if (!profile) {
            profile = new XXProfileTLS();
            pthread_setspecific(g_profile_tls_key, profile);
        }
    }
#endif//XX_ThreadLocal
    return profile;
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
    _profile = XXProfileTLS::Get();
    assert(_profile);
    if (_profile) {
        _node = static_cast<XXProfileTLS*>(_profile)->beginScope(name);
    } else {
        _node = NULL;
    }
}

XXProfileScope::~XXProfileScope() {
    if (_profile) {
        static_cast<XXProfileTLS*>(_profile)->endScope(_node);
    }
}

XX_NAMESPACE_END(xxprofile);
