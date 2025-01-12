// Copyright 2025 bianchui. All rights reserved.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "TimeLineView.hpp"
#include "imgui.h"
#include "imgui_internal.h"

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
}
