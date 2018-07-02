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

    struct ThreadData {
        float valueMax;
        uint64_t maxCycles;
        int startIndex;
        const xxprofile::Loader::ThreadData* _data;

        void init(const xxprofile::Loader::ThreadData* data);
        
        static float StaticGetData(void*, int);

        void setTo(ImGui::ImPlotWithHitTest& plot) const;
    };

    TimeLineView();
    ~TimeLineView();

    float calcHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
};

#endif//xxprofileviewer_TimeLineView_hpp
