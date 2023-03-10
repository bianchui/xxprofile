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
    const xxprofile::FrameDetail* _frameDetail;
    bool _combined;
    uint64_t _frameStart;
};

#endif//xxprofileviewer_FrameView_hpp
