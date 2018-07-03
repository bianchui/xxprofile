// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_EventHandler_hpp
#define xxprofileviewer_EventHandler_hpp
#include "../loader/xxprofile_loader.hpp"

struct EventHandler {
    virtual void onFrameSelectChange(int tid, int item) = 0;
};

#endif//xxprofileviewer_EventHandler_hpp
