// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofileviewer_FrameView_hpp
#define xxprofileviewer_FrameView_hpp
#include "../loader/xxprofile_loader.hpp"

class FrameView {
public:
    FrameView();
    ~FrameView();

    void setLoader(const xxprofile::Loader* loader);
    void setFrameData(const xxprofile::FrameData* data);
    void draw();

    void clear();
    void setCombined(bool combined) {
        _combined = combined;
    }

private:
    const xxprofile::Loader* _loader;
    const xxprofile::FrameData* _frameData;
    bool _combined;
};

#endif//xxprofileviewer_FrameView_hpp
