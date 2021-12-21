// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofileviewer_EventHandler_hpp
#define xxprofileviewer_EventHandler_hpp
#include "../loader/xxprofile_loader.hpp"
#include <shared/utils/StrBuf.h>
#include <cmath>

struct EventHandler {
    virtual void onFrameSelectChange(const xxprofile::FrameData* frameData) = 0;
};

struct Math {
    static void FormatTime(shared::StrBuf& buf, double v) {
        if (v < 0) {
            buf.append("-");
            v = std::fabs(v);
        }
        if (v >= 1) {
            buf.appendf("%.3fs", v);
        } else if (v >= 0.001) {
            buf.appendf("%.3fms", v * 1000);
        } else if (v >= 0.000001) {
            buf.appendf("%.3fus", v * 1000000);
        } else if (v >= 0.000000001) {
            buf.appendf("%.3fns", v * 1000000000);
        } else {
            buf.appendf("%8.4g", v);
        }
    }
};

#endif//xxprofileviewer_EventHandler_hpp
