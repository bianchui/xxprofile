// Copyright 2018-2025 bianchui. All rights reserved.
#ifndef xxprofileviewer_EventHandler_hpp
#define xxprofileviewer_EventHandler_hpp
#include "../loader/xxprofile_loader.hpp"
#include <cmath>

struct EventHandler {
    virtual void onFrameSelectChange(const xxprofile::FrameData* frameData) = 0;
};

#endif//xxprofileviewer_EventHandler_hpp
