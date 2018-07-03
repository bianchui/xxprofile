// Copyright 2017 bianchui. All rights reserved.
#ifndef xxprofileviewer_FramesLineView_hpp
#define xxprofileviewer_FramesLineView_hpp
#include "../loader/xxprofile_loader.hpp"
#include "imgui/imgui_custom.hpp"
#include "EventHandler.hpp"

class FramesLineView {
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

    FramesLineView(EventHandler* handler);
    ~FramesLineView();

    float calcHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
};

#endif//xxprofileviewer_FramesLineView_hpp
