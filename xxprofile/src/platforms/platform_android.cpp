// Copyright 2017 bianchui. All rights reserved.
#include "platform_android.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

XX_NAMESPACE_BEGIN(xxprofile);

uint32_t GetTid() {
    return ::gettid();
}

XX_NAMESPACE_END(xxprofile);
