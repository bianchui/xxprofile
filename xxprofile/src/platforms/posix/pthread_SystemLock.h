//
//  pthread_SystemLock.h
//  xxprofile
//
//  Created by bianchui on 2017/11/29.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef xxprofile_platforms_posix_pthread_SystemLock_h
#define xxprofile_platforms_posix_pthread_SystemLock_h
#include <pthread.h>
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class SystemLock_pthread {
public:
    SystemLock_pthread() {
        pthread_mutex_init(&_cs, NULL);
    }

    ~SystemLock_pthread() {
        pthread_mutex_destroy(&_cs);
    }
    
    bool TryLock() {
        return pthread_mutex_trylock(&_cs) == 0;
    }

    void Lock() {
        pthread_mutex_lock(&_cs);
    }

    void Unlock() {
        pthread_mutex_unlock(&_cs);
    }

public:
    pthread_mutex_t _cs;

    XX_CLASS_DELETE_COPY_AND_MOVE(SystemLock_pthread);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_posix_pthread_SystemLock_h
