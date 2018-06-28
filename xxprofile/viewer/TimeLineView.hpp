// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_TimeLineView_hpp
#define xxprofileviewer_TimeLineView_hpp
#include "../loader/xxprofile_loader.hpp"
#include "imgui.h"

class TimeLineView {
public:
    TimeLineView();

    void setLoader(xxprofile::Loader* loader);

    void draw();

private:
    ImHistogramWithHitTest _hitTest;
    xxprofile::Loader* _loader;
};

#endif//xxprofileviewer_TimeLineView_hpp
