// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_mainwin_hpp
#define xxprofileviewer_mainwin_hpp
#include "TimeLineView.hpp"
#include "FrameView.hpp"
#include "../loader/xxprofile_loader.hpp"
#include "EventHandler.hpp"

struct MainWin : EventHandler {
    MainWin();
    bool load(const char* file);

    void draw(int w, int h);

    TimeLineView _timeLineView;
    FrameView _frameView;

    xxprofile::Loader _loader;

    void onFrameSelectChange(int tid, int item) override {
        const xxprofile::FrameData* frameData = NULL;
        if (tid < _loader._threads.size()) {
            auto& thread = _loader._threads[tid];
            if (item < thread._frames.size()) {
                frameData = &thread._frames[item];
            }
        }
        _frameView.setFrameData(frameData);
    }

private:
    bool _load(const char* file);
};

#endif//xxprofileviewer_mainwin_hpp
