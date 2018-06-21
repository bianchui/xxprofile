#ifndef xxprofile_pthread_tls_h
#define xxprofile_pthread_tls_h
#include <pthread.h>
#include "../../xxprofile_macros.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

class Pthread_TLS {
public:
    static_assert(sizeof(uintptr_t) == sizeof(pthread_key_t), "sizeof(pthread_key_t)");
    static uintptr_t AllocSlot() {
        pthread_key_t slot = 0;
        if (pthread_key_create(&slot, NULL) != 0) {
            slot = 0xFFFFFFFF;
        }
        return slot;
    }

    static void FreeSlot(uintptr_t slot) {
        pthread_key_delete((pthread_key_t)slot);
    }

    static void SetValue(uintptr_t slot, void* value) {
        pthread_setspecific((pthread_key_t)slot, value);
    }

    static void* GetValue(uintptr_t slot) {
        return pthread_getspecific((pthread_key_t)slot);
    }

public:
    XX_CLASS_DELETE_COPY(Pthread_TLS);
    XX_CLASS_DELETE_MOVE(Pthread_TLS);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_pthread_tls_h
