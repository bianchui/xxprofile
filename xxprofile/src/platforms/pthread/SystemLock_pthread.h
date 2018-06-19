//
//  SystemLock_pthread.h
//  xxprofile
//
//  Created by bianchui on 2017/11/29.
//  Copyright 2017 bianchui. All rights reserved.
//

#ifndef SystemLock_pthread_h
#define SystemLock_pthread_h
#include <pthread.h>
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class CSystemLock_pthread {
public:
    CSystemLock_pthread() {
        pthread_mutex_init(&_cs, NULL);
    }
    
    ~CSystemLock_pthread() {
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
    XX_CLASS_DELETE_COPY(CSystemLock_pthread);
    XX_CLASS_DELETE_MOVE(CSystemLock_pthread);
    pthread_mutex_t _cs;
};

XX_NAMESPACE_END(xxprofile);

#endif//SystemLock_pthread_h
