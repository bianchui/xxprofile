// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_FrameView_hpp
#define xxprofileviewer_FrameView_hpp
#include "../loader/xxprofile_loader.hpp"

class FrameView {
public:
    FrameView();

    void draw();

private:
    xxprofile::Loader::FrameData* _frameData;

};

#endif//xxprofileviewer_FrameView_hpp
