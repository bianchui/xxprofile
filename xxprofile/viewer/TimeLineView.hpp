// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_TimeLineView_hpp
#define xxprofileviewer_TimeLineView_hpp
#include "../loader/xxprofile_loader.hpp"
#include "imgui/imgui_custom.hpp"

class TimeLineView {
public:
    enum {
        FramesItemHeight = 100,
    };

    TimeLineView();
    ~TimeLineView();

    float calcHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    ImGui::ImHistogramWithHitTest _hitTest;
    const xxprofile::Loader* _loader;
};

#endif//xxprofileviewer_TimeLineView_hpp
