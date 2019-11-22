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
#include <mutex>
#include "xxprofile_tls.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class XXProfileTLSHandle {
public:
    XXProfileTLSHandle() : _tls(NULL) {
    }
    ~XXProfileTLSHandle() {
        if (_tls) {
            delete _tls;
            _tls = NULL;
        }
    }
    
    XXProfileTLS* get();
private:
    XXProfileTLS* _tls;
};

static std::string g_filePath;
static thread_local XXProfileTLSHandle g_profile_tls;
static std::mutex g_mutex;

// XXProfile
bool XXProfile::StaticInit(const char* savePath) {
    Timer::InitTiming();
    if (savePath) {
        g_filePath.assign(savePath);
        if (g_filePath.length() > 0 && g_filePath.back() != '/') {
            g_filePath.push_back('/');
        }
    } else {
        g_filePath = systemGetWritablePath();
    }
    return true;
}

void XXProfile::StaticUninit() {
}

XXProfileTLS* XXProfileTLS::Get() {
    return g_profile_tls.get();
}

XXProfileTLS* XXProfileTLSHandle::get() {
    XXProfileTLS* profile = _tls;
    if (!profile) {
        _tls = profile = new XXProfileTLS(g_filePath.c_str());
    }
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
