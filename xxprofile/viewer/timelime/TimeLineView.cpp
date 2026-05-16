// Copyright (C) 2025, bianchui. All rights reserved.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "TimeLineView.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#pragma mark - TimeLineView::ThreadData

#pragma mark - TimeLineView

ImVec4 BGTextColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

TimeLineView::TimeLineView(EventHandler* handler) : _handler(handler), _loader(nullptr) {
}

TimeLineView::~TimeLineView() {

}

float TimeLineView::getHeight() {
    return 0;
}

void TimeLineView::setLoader(const xxprofile::Loader* loader) {
    clear();
    _loader = loader;
    if (loader) {
        const size_t tcount = loader->thread_count();
        _threads.resize(tcount);
        uint32_t minFrame = -1;
        uint32_t maxFrame = 0;
        for (size_t t = 0; t < tcount; ++t) {
            const auto& loader_thread = loader->thread(t);
            if (loader_thread._frames.size() > 0) {
                const auto& frame0 = loader_thread._frames[0];
                if (minFrame == -1 || minFrame > frame0.frameId()) {
                    minFrame = frame0.frameId();
                }
                const auto& frameN = loader_thread._frames.back();
                if (maxFrame < frameN.frameId()) {
                    maxFrame = frameN.frameId();
                }
            }
        }
        if (minFrame == -1) {
            minFrame = 0;
        }

        for (size_t t = 0; t < tcount; ++t) {
            auto& thread = _threads[t];
            const auto& loader_thread = loader->thread(t);
            thread.init(&loader_thread, loader->processStart());
        }
    }
}

void TimeLineView::clear() {

}

void TimeLineView::draw() {
    if (_threads.empty()) {
        return;
    }

    ImGuiStyle& style = ImGui::GetStyle();

    ImVec2 sizeActual = ImGui::GetContentRegionAvail();
    ImRect timelineRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + sizeActual);
    ImGui::ItemSize(timelineRect.GetSize());

    // The current (scaled) size of the timeline
    float timelineWidth = timelineRect.GetWidth() * _timelineScale;

    ImVec2 cursor = ImVec2(timelineRect.Min.x + _timelineOffset, timelineRect.Min.y);
    ImVec2 cursorStart = cursor;
    ImDrawList* pDraw = ImGui::GetWindowDrawList();

    ImGuiID timelineID = ImGui::GetID("Timeline");
    timelineRect.Max -= ImVec2(_scrollBarSize, _scrollBarSize);
    if (!ImGui::ItemAdd(timelineRect, timelineID)) {
        return;
    }
    ImGui::PushClipRect(timelineRect.Min, timelineRect.Max, true);
    pDraw->AddRectFilled(timelineRect.Min, ImVec2(timelineRect.Max.x, timelineRect.Min.y + _barHeight), ImColor(0.0f, 0.0f, 0.0f, 0.1f));
    pDraw->AddRect(timelineRect.Min - ImVec2(10, 0), ImVec2(timelineRect.Max.x + 10, timelineRect.Min.y + _barHeight), ImColor(1.0f, 1.0f, 1.0f, 0.4f));

    ImGui::RenderArrow(pDraw, ImVec2(timelineRect.Min.x - 0 + 10, timelineRect.Min.y), 0xffff00ff, ImGuiDir_Right, 1.0f);
    ImGui::RenderArrow(pDraw, ImVec2(timelineRect.Min.x - 0 + 50, timelineRect.Min.y), 0xffff00ff, ImGuiDir_Down, 1.0f);
    uint64_t frequency = 100000000;
    const float MsToTicks = (float)frequency / 1000.0f;
    const float TicksToMs = 1000.0f / frequency;
    float ticksInTimeline = MsToTicks * _maxTime;
    const float TicksToPixels = timelineWidth / ticksInTimeline;

    for (int i = 0; i < _maxTime; ++i) {
        float x0 = (float)i * MsToTicks * TicksToPixels;
        float msWidth = 1.0f * MsToTicks * TicksToPixels;
        ImVec2 tickPos = ImVec2(cursor.x + x0, timelineRect.Min.y);
        pDraw->AddLine(tickPos + ImVec2(0, _barHeight * 0.5f), tickPos + ImVec2(0, _barHeight), ImColor(BGTextColor));

        if (i % 2 == 0) {
            pDraw->AddRectFilled(tickPos + ImVec2(0, _barHeight), tickPos + ImVec2(msWidth, timelineRect.Max.y), ImColor(1.0f, 1.0f, 1.0f, 0.02f));
            const char* pBarText;
            ImFormatStringToTempBuffer(&pBarText, nullptr, "%d ms", i);
            pDraw->AddText(tickPos + ImVec2(5, 0), ImColor(BGTextColor), pBarText);
        }
    }
    ImGui::PopClipRect();
}
