// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_TimeLineView_hpp
#define xxprofileviewer_TimeLineView_hpp
#include "../loader/xxprofile_loader.hpp"
#include "imgui/imgui_custom.hpp"
#include "EventHandler.hpp"

class TimeLineView {
public:
    enum {
        FramesItemHeight = 100,
    };

    struct ThreadData {
        int startIndex;
        int selectedItem;
        const xxprofile::ThreadData* _data;

        void init(const xxprofile::ThreadData* data);
        
        static float StaticGetData(void*, int);

        void setTo(ImGui::ImPlotWithHitTest& plot) const;
    };

    TimeLineView(EventHandler* handler);
    ~TimeLineView();

    float calcHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
};

#endif//xxprofileviewer_TimeLineView_hpp
