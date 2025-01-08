// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofileviewer_format_hpp
#define xxprofileviewer_format_hpp
#include <shared/utils/StrBuf.h>
#include <cmath>

struct Format {
    static void Time(shared::StrBuf& buf, double v) {
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
            buf.appendf("%.4g", v);
        }
    }

    static void TimeMS(shared::StrBuf& buf, double time) {
        if (time > 60) {
            uint32_t minutes = (uint32_t)time / 60;
            buf.appendf("%d:", minutes);
            time -= minutes * 60;
            uint32_t seconds = (uint32_t)time;
            buf.appendf("%02d", seconds);
            time -= seconds;
        } else {
            uint32_t seconds = (uint32_t)time;
            buf.appendf("%02d", seconds);
            time -= seconds;
        }
        uint32_t ms = (uint32_t)(time * 1000);
        buf.appendf(".%03d", ms);
    }
};

#endif//xxprofileviewer_format_hpp
