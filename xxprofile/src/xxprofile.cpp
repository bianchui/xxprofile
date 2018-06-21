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

XX_NAMESPACE_BEGIN(xxprofile);

static pthread_key_t g_profile_tls_key;
static pthread_once_t g_profile_init_once = PTHREAD_ONCE_INIT;
static __thread XXProfileTLS* g_tls_profile;

// XXProfile
static void profile_on_thread_exit(void* data) {
    XXProfileTLS* profile = (XXProfileTLS*)data;
    delete profile;
}

static void profile_tls_init_once() {
    pthread_key_create(&g_profile_tls_key, profile_on_thread_exit);
    XXProfileTimer::InitTiming();
}

bool XXProfile::StaticInit() {
    pthread_once(&g_profile_init_once, profile_tls_init_once);
    return true;
}

XXProfileTLS* XXProfileTLS::Get() {
    if (!g_tls_profile) {
        XXProfileTLS* profile = new XXProfileTLS();
        pthread_setspecific(g_profile_tls_key, profile);
        g_tls_profile = profile;
    }
    return g_tls_profile;
}

void XXProfile::IncreaseFrame() {
    XXProfileTLS* profile = XXProfileTLS::Get();
    profile->increaseFrame();
}

// XXProfileScope
XXProfileScope::XXProfileScope(const SName name) {
    _profile = XXProfileTLS::Get();
    _node = static_cast<XXProfileTLS*>(_profile)->beginScope(name);
}

XXProfileScope::~XXProfileScope() {
    static_cast<XXProfileTLS*>(_profile)->endScope(_node);
}

XX_NAMESPACE_END(xxprofile);
