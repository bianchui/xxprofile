// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofileviewer_mainwin_hpp
#define xxprofileviewer_mainwin_hpp
#include "FramesLineView.hpp"
#include "FrameView.hpp"
#include "../loader/xxprofile_loader.hpp"
#include "EventHandler.hpp"

struct MainWin : EventHandler {
    MainWin();
    bool load(const char* file);
    std::string getTitle() const;

    void draw(int w, int h);

    FramesLineView _framesLineView;
    FrameView _frameView;

    xxprofile::Loader _loader;

    void onFrameSelectChange(const xxprofile::FrameData* frameData) override {
        //const xxprofile::FrameData* frameData = NULL;
        //if (tid < _loader._threads.size()) {
        //    auto& thread = _loader._threads[tid];
        //    if (item < thread._frames.size()) {
        //        frameData = &thread._frames[item];
        //    }
        //}
        _frameView.setFrameData(frameData);
    }

private:
    bool _load(const char* file);
};

#endif//xxprofileviewer_mainwin_hpp
