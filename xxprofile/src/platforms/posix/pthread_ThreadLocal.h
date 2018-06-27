// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_platforms_posix_pthread_ThreadLocal_h
#define xxprofile_platforms_posix_pthread_ThreadLocal_h
#include <pthread.h>
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

template <typename T>
class ThreadLocal_pthread {
public:
    ThreadLocal_pthread() : _key(NULL) {
        pthread_key_create(&_key, _on_thread_exit);
    }
    ~ThreadLocal_pthread() {
        T* p = get();
        if (p) {
            delete p;
        }
        pthread_key_delete(_key);
    }

    void set(T* value) {
        pthread_setspecific(_key, (void*)value);
    }

    T* get() {
        return (T*)pthread_getspecific(_key);
    }

private:
    static void _on_thread_exit(void* data) {
        T* p = (T*)data;
        delete p;
    }

protected:
    pthread_key_t _key;
    XX_CLASS_DELETE_COPY_AND_MOVE(ThreadLocal_pthread);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_posix_pthread_ThreadLocal_h
