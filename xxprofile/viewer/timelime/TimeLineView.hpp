//
//  TimeLineView.hpp
//  xxprofileViewer
//
//  Created by bianchui on 2025/1/7.
//  Copyright (C) 2025, bianchui. All rights reserved.
//

#ifndef TimeLineView_hpp
#define TimeLineView_hpp
#include "../../loader/xxprofile_loader.hpp"
#include "../EventHandler.hpp"
#include "../format.hpp"
#include "imgui.h"
#include "imgui_internal.h"

class TimeLineView {
public:
    struct ThreadData {
        const xxprofile::ThreadData* _data;
        uint64_t _processStart;
        bool _expended;
        uint32_t _visibleDepth;
        std::vector<uint32_t> _frameNodeOffsets;
        std::vector<uint32_t> _nodeDepths;

        void init(const xxprofile::ThreadData* data, uint64_t processStart) {
            assert(data);
            _data = data;
            _expended = true;
            _visibleDepth = 20;
            _processStart = processStart;
            rebuildDepths();
        }

        void rebuildDepths();
        uint32_t nodeDepth(const xxprofile::FrameData& frame, uint32_t nodeIndex) const;
    };

    TimeLineView(EventHandler* handler);
    ~TimeLineView();

    float getHeight();
    void setLoader(const xxprofile::Loader* loader);
    void clear();

    void draw();

private:
    float calcContentHeight() const;
    void drawRuler(ImDrawList* drawList, const ImRect& rulerRect, const ImRect& bodyRect, double visibleTicks, double ticksToPixels) const;
    void drawFrame(ImDrawList* drawList, const ThreadData& thread, const xxprofile::FrameData& frame, const ImRect& bodyRect, float y, double ticksToPixels);
    void drawNode(ImDrawList* drawList, const ThreadData& thread, const xxprofile::FrameData& frame, uint32_t nodeIndex, const ImRect& bodyRect, float y, double ticksToPixels);
    float timeToX(uint64_t time, const ImRect& bodyRect, double ticksToPixels) const;
    ImU32 nameColor(const char* name, float saturation = 0.55f, float value = 0.72f) const;

    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
    uint64_t _processStart;
    uint64_t _processEnd;
    double _viewStart;
    double _viewEnd;
    const xxprofile::FrameData* _selectedFrame;

    float _leftWidth = 190.0f;
    float _rulerHeight = 26.0f;
    float _threadHeaderHeight = 22.0f;
    float _rowHeight = 18.0f;
    float _rowGap = 2.0f;
    float _minBarWidth = 1.0f;
};

#endif /* TimeLineView_hpp */
