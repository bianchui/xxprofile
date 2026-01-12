#ifndef xxprofile_platform_emscripten_hpp
#define xxprofile_platform_emscripten_hpp

#include "../xxprofile_macros.hpp"
#include "platform_base.hpp"
#include "posix/posix_timer.h"

XX_NAMESPACE_BEGIN(xxprofile);

// emscripten is a single-threaded platform, so we don't need a lock
class SystemLock_empty {
public:
    SystemLock_empty() {
    }

    ~SystemLock_empty() {
    }

    bool TryLock() {
        return true;
    }

    void Lock() {
    }

    void Unlock() {
    }

protected:
    XX_CLASS_DELETE_COPY_AND_MOVE(SystemLock_empty);
};

template <typename T>
class ThreadLocal_empty {
public:
    ThreadLocal_empty() {
    }
    ~ThreadLocal_empty() {
        T* p = get();
        if (p) {
            delete p;
        }
    }

    void set(T* value) {
        _value = value;
    }

    T* get() {
        return _value;
    }

protected:
    T* _value;
    XX_CLASS_DELETE_COPY_AND_MOVE(ThreadLocal_empty);
};

typedef SystemLock_empty SystemLock;
typedef Timer_posix Timer;

template <typename T>
struct ThreadLocal : public ThreadLocal_empty<T> {};

XX_NAMESPACE_END(xxprofile);

#endif //xxprofile_platform_emscripten_hpp
