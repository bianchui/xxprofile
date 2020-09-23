// Copyright 2017 bianchui. All rights reserved.
#include "platform_android.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <dlfcn.h>
#include <string>

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t systemGetTid() {
    return ::gettid();
}

std::string android_getDlName() {
    static int value_ = 0;
    Dl_info info;
    dladdr(&value_, &info);
    const char* fname = strrchr(info.dli_fname, '/');
    if (fname) {
        ++fname;
    } else {
        fname = info.dli_fname;
    }
    return fname;
}

std::string systemGetAppName() {
    return android_getDlName();
}

XX_NAMESPACE_END(xxprofile);
