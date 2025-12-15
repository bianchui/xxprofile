#ifndef xxprofile_platform_emscripten_hpp
#define xxprofile_platform_emscripten_hpp

#include "../xxprofile_macros.hpp"
#include "platform_base.hpp"

XX_NAMESPACE_BEGIN(xxprofile);

// emscripten is a single-threaded platform, so we don't need a lock
class SystemLock_emscripten {
public:
    SystemLock_emscripten() {
    }

    ~SystemLock_emscripten() {
    }

    bool TryLock() {
        return true;
    }

    void Lock() {
    }

    void Unlock() {
    }

protected:
    XX_CLASS_DELETE_COPY_AND_MOVE(SystemLock_emscripten);
};

struct Timer_emscripten : Timer_base {
    static double InitTiming();

    static FORCEINLINE double Seconds() {
        return Cycles64() * GetSecondsPerCycle() + 16777216.0;
    }

    static FORCEINLINE uint64_t Cycles64() {
        uint64_t cycles = emscripten_get_now();
        return cycles;
    }
};

typedef SystemLock_emscripten SystemLock;

XX_NAMESPACE_END(xxprofile);

#endif //xxprofile_platform_emscripten_hpp
