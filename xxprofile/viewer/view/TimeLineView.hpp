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
        struct DepthIndex {
            std::vector<uint32_t> _nodes;
            std::vector<uint64_t> _blockMaxDurations;
        };

        struct FrameIndex {
            std::vector<DepthIndex> _depths;
        };

        const xxprofile::ThreadData* _data;
        bool _expended;
        uint32_t _visibleDepth;
        std::vector<FrameIndex> _frameIndexes;

        void init(const xxprofile::ThreadData* data);

        void rebuildIndex();

        uint32_t findFirstFrame(uint64_t startTime) const;
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
    void selectThreadFrame(const ThreadData& thread);
    float timeToX(uint64_t time, const ImRect& bodyRect, double ticksToPixels) const;
    ImU32 nameColor(xxprofile::SName name, float saturation = 0.55f, float value = 0.72f);

    EventHandler* _handler;
    const xxprofile::Loader* _loader;
    std::vector<ThreadData> _threads;
    uint64_t _processStart;
    uint64_t _processEnd;
    double _viewStart;
    double _viewEnd;
    const xxprofile::FrameData* _selectedFrame;
    std::vector<ImU32> _nameColors;

    float _leftWidth = 190.0f;
    float _rulerHeight = 26.0f;
    float _threadHeaderHeight = 22.0f;
    float _rowHeight = 18.0f;
    float _rowGap = 2.0f;
    float _minBarWidth = 1.0f;
};

#endif /* TimeLineView_hpp */
