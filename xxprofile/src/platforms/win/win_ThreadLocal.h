// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_platforms_win_win_ThreadLocal_h
#define xxprofile_platforms_win_win_ThreadLocal_h
#include "win_inc_windows.h"
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

template <typename T>
class ThreadLocal_win {
public:
    ThreadLocal_win() {
        _fls = ::FlsAlloc(_on_thread_exit);
    }
    ~ThreadLocal_win() {
        T* p = get();
        if (p) {
            delete p;
        }
        ::FlsFree(_fls);
    }

    void set(T* value) {
        ::FlsSetValue(_fls, (void*)value);
    }

    T* get() {
        return (T*)::FlsGetValue(_fls);
    }

private:
    static void __stdcall _on_thread_exit(void* data) {
        T* p = (T*)data;
        delete p;
    }

protected:
    DWORD _fls;
    XX_CLASS_DELETE_COPY_AND_MOVE(ThreadLocal_win);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_platforms_win_win_ThreadLocal_h
