// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_mainwin_hpp
#define xxprofileviewer_mainwin_hpp
#include "TimeLineView.hpp"
#include "FrameView.hpp"
#include "../loader/xxprofile_loader.hpp"

struct MainWin {
    bool load(const char* file);

    void draw(int w, int h);

    TimeLineView _timeLineView;
    FrameView _frameView;

    xxprofile::Loader _loader;
};

#endif//xxprofileviewer_mainwin_hpp
