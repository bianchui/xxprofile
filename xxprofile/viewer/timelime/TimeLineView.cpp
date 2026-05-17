// Copyright (C) 2025, bianchui. All rights reserved.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "TimeLineView.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <cmath>

#pragma mark - TimeLineView::ThreadData

void TimeLineView::ThreadData::rebuildDepths() {
    _frameNodeOffsets.clear();
    _nodeDepths.clear();
    for (const auto& frame : _data->_frames) {
        const uint32_t nodeCount = frame.nodeCount();
        const uint32_t base = (uint32_t)_nodeDepths.size();
        _frameNodeOffsets.push_back(base);
        _nodeDepths.resize(base + nodeCount);
        for (uint32_t i = 0; i < nodeCount; ++i) {
            const auto& node = frame._nodes[i];
            uint32_t depth = 0;
            if (node._parentNodeId) {
                const uint32_t parentIndex = node._parentNodeId - 1;
                if (parentIndex < i) {
                    depth = _nodeDepths[base + parentIndex] + 1;
                }
            }
            _nodeDepths[base + i] = depth;
        }
    }
}

uint32_t TimeLineView::ThreadData::nodeDepth(const xxprofile::FrameData& frame, uint32_t nodeIndex) const {
    const xxprofile::FrameData* first = _data->_frames.data();
    const ptrdiff_t frameIndex = &frame - first;
    if (frameIndex < 0 || (size_t)frameIndex >= _frameNodeOffsets.size()) {
        return 0;
    }
    const uint32_t offset = _frameNodeOffsets[(size_t)frameIndex];
    const uint32_t index = offset + nodeIndex;
    if (index < _nodeDepths.size()) {
        return _nodeDepths[index];
    }
    return 0;
}

#pragma mark - TimeLineView

static const ImVec4 kTimelineTextColor(0.72f, 0.74f, 0.78f, 1.0f);
static const ImVec4 kTimelineMutedTextColor(0.48f, 0.50f, 0.55f, 1.0f);

TimeLineView::TimeLineView(EventHandler* handler)
: _handler(handler)
, _loader(nullptr)
, _processStart(0)
, _processEnd(1)
, _viewStart(0)
, _viewEnd(1)
, _selectedFrame(nullptr) {
}

TimeLineView::~TimeLineView() {

}

float TimeLineView::getHeight() {
    return calcContentHeight();
}

void TimeLineView::setLoader(const xxprofile::Loader* loader) {
    clear();
    _loader = loader;
    if (!loader) {
        return;
    }

    _processStart = 0;
    _processEnd = 0;
    bool hasFrame = false;
    const size_t tcount = loader->thread_count();
    for (size_t t = 0; t < tcount; ++t) {
        const auto& loader_thread = loader->thread(t);
        for (const auto& frame : loader_thread._frames) {
            if (!hasFrame || _processStart > frame.startTime()) {
                _processStart = frame.startTime();
            }
            if (_processEnd < frame.endTime()) {
                _processEnd = frame.endTime();
            }
            hasFrame = true;
        }
    }
    if (_processEnd <= _processStart) {
        _processEnd = _processStart + 1;
    }

    _threads.resize(tcount);
    for (size_t t = 0; t < tcount; ++t) {
        auto& thread = _threads[t];
        const auto& loader_thread = loader->thread(t);
        thread.init(&loader_thread, _processStart);
    }

    _viewStart = 0;
    _viewEnd = (double)(_processEnd - _processStart);
}

void TimeLineView::clear() {
    _threads.clear();
    _loader = nullptr;
    _processStart = 0;
    _processEnd = 1;
    _viewStart = 0;
    _viewEnd = 1;
    _selectedFrame = nullptr;
}

float TimeLineView::calcContentHeight() const {
    float height = _rulerHeight;
    for (const auto& thread : _threads) {
        height += _threadHeaderHeight + _rowGap;
        height += _rowHeight + _rowGap;
        if (thread._expended && thread._data->_maxCallDepth) {
            height += thread._data->_maxCallDepth * (_rowHeight + _rowGap);
        }
    }
    return height + ImGui::GetStyle().WindowPadding.y * 2.0f;
}

float TimeLineView::timeToX(uint64_t time, const ImRect& bodyRect, double ticksToPixels) const {
    const double rel = (double)(time - _processStart);
    return bodyRect.Min.x + (float)((rel - _viewStart) * ticksToPixels);
}

ImU32 TimeLineView::nameColor(const char* name, float saturation, float value) const {
    uint32_t hash = 2166136261u;
    if (name) {
        for (const char* p = name; *p; ++p) {
            hash = (hash ^ (uint8_t)*p) * 16777619u;
        }
    }
    const float hue = (float)(hash % 360) / 360.0f;
    return ImColor::HSV(hue, saturation, value);
}

void TimeLineView::drawRuler(ImDrawList* drawList, const ImRect& rulerRect, const ImRect& bodyRect, double visibleTicks, double ticksToPixels) const {
    const double secondsPerCycle = _loader ? _loader->secondsPerCycle() : 0;
    const double visibleSeconds = visibleTicks * secondsPerCycle;
    if (visibleSeconds <= 0) {
        return;
    }

    drawList->AddRectFilled(rulerRect.Min, rulerRect.Max, ImColor(0.10f, 0.11f, 0.13f, 1.0f));
    drawList->AddLine(ImVec2(bodyRect.Min.x, rulerRect.Max.y - 1), ImVec2(bodyRect.Max.x, rulerRect.Max.y - 1), ImColor(1.0f, 1.0f, 1.0f, 0.16f));

    const double targetPixels = 90.0;
    const double rawStepSeconds = visibleSeconds * targetPixels / std::max(1.0f, bodyRect.GetWidth());
    const double base = std::pow(10.0, std::floor(std::log10(rawStepSeconds)));
    double stepSeconds = base;
    if (rawStepSeconds / base > 5.0) {
        stepSeconds = base * 10.0;
    } else if (rawStepSeconds / base > 2.0) {
        stepSeconds = base * 5.0;
    } else if (rawStepSeconds / base > 1.0) {
        stepSeconds = base * 2.0;
    }
    const double stepTicks = stepSeconds / secondsPerCycle;
    const double firstTick = std::floor(_viewStart / stepTicks) * stepTicks;

    shared::StrBuf text;
    for (double tick = firstTick; tick <= _viewEnd; tick += stepTicks) {
        if (tick < _viewStart) {
            continue;
        }
        const float x = bodyRect.Min.x + (float)((tick - _viewStart) * ticksToPixels);
        drawList->AddLine(ImVec2(x, rulerRect.Min.y + 8.0f), ImVec2(x, bodyRect.Max.y), ImColor(1.0f, 1.0f, 1.0f, 0.07f));
        drawList->AddLine(ImVec2(x, rulerRect.Max.y - 8.0f), ImVec2(x, rulerRect.Max.y), ImColor(kTimelineMutedTextColor));
        text.clear();
        Format::Time(text, tick * secondsPerCycle);
        drawList->AddText(ImVec2(x + 4.0f, rulerRect.Min.y + 5.0f), ImColor(kTimelineTextColor), text.c_str());
    }
}

void TimeLineView::drawFrame(ImDrawList* drawList, const ThreadData& thread, const xxprofile::FrameData& frame, const ImRect& bodyRect, float y, double ticksToPixels) {
    if (frame.endTime() < _processStart + (uint64_t)_viewStart || frame.startTime() > _processStart + (uint64_t)_viewEnd) {
        return;
    }

    const float x0 = timeToX(frame.startTime(), bodyRect, ticksToPixels);
    const float x1 = timeToX(frame.endTime(), bodyRect, ticksToPixels);
    ImRect rect(ImVec2(std::max(x0, bodyRect.Min.x), y), ImVec2(std::min(std::max(x1, x0 + _minBarWidth), bodyRect.Max.x), y + _rowHeight));
    if (rect.Max.x <= bodyRect.Min.x || rect.Min.x >= bodyRect.Max.x) {
        return;
    }

    const bool selected = _selectedFrame == &frame;
    const ImU32 color = selected ? ImColor(0.97f, 0.73f, 0.28f, 1.0f) : ImColor(0.28f, 0.52f, 0.88f, 1.0f);
    drawList->AddRectFilled(rect.Min, rect.Max, color, 2.0f);
    drawList->AddRect(rect.Min, rect.Max, selected ? ImColor(1.0f, 0.95f, 0.65f, 1.0f) : ImColor(1.0f, 1.0f, 1.0f, 0.18f), 2.0f);

    if (rect.GetWidth() > 42.0f) {
        const char* text;
        ImFormatStringToTempBuffer(&text, nullptr, "F%d", frame.frameId());
        drawList->AddText(rect.Min + ImVec2(4.0f, 2.0f), ImColor(0.04f, 0.05f, 0.06f, 1.0f), text);
    }

    if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max)) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            _selectedFrame = &frame;
            _handler->onFrameSelectChange(&frame);
        }
        shared::StrBuf buf;
        buf.appendf("Thread %d (%s)\nFrame %d\nStart: ", thread._data->_threadId, thread._data->_threadName ? thread._data->_threadName : "", frame.frameId());
        Format::Time(buf, (frame.startTime() - _processStart) * thread._data->_secondsPerCycle);
        buf.append("\nEnd:   ");
        Format::Time(buf, (frame.endTime() - _processStart) * thread._data->_secondsPerCycle);
        buf.append("\nTime:  ");
        Format::Time(buf, frame.frameCycles() * thread._data->_secondsPerCycle);
        buf.appendf("\nNodes: %d", frame.nodeCount());
        ImGui::SetTooltip("%s", buf.c_str());
    }
}

void TimeLineView::drawNode(ImDrawList* drawList, const ThreadData& thread, const xxprofile::FrameData& frame, uint32_t nodeIndex, const ImRect& bodyRect, float y, double ticksToPixels) {
    const auto& node = frame._nodes[nodeIndex];
    if (node._endTime < _processStart + (uint64_t)_viewStart || node._beginTime > _processStart + (uint64_t)_viewEnd) {
        return;
    }

    const char* name = _loader->name(node._name);
    const float x0 = timeToX(node._beginTime, bodyRect, ticksToPixels);
    const float x1 = timeToX(node._endTime, bodyRect, ticksToPixels);
    ImRect rect(ImVec2(std::max(x0, bodyRect.Min.x), y), ImVec2(std::min(std::max(x1, x0 + _minBarWidth), bodyRect.Max.x), y + _rowHeight));
    if (rect.Max.x <= bodyRect.Min.x || rect.Min.x >= bodyRect.Max.x) {
        return;
    }

    drawList->AddRectFilled(rect.Min, rect.Max, nameColor(name), 2.0f);
    drawList->AddRect(rect.Min, rect.Max, ImColor(0.0f, 0.0f, 0.0f, 0.18f), 2.0f);
    if (rect.GetWidth() > 56.0f) {
        drawList->PushClipRect(rect.Min, rect.Max, true);
        drawList->AddText(rect.Min + ImVec2(4.0f, 2.0f), ImColor(0.05f, 0.05f, 0.06f, 1.0f), name);
        drawList->PopClipRect();
    }

    if (ImGui::IsMouseHoveringRect(rect.Min, rect.Max)) {
        shared::StrBuf buf;
        buf.appendf("%s\nFrame %d\nStart: ", name, frame.frameId());
        Format::Time(buf, (node._beginTime - frame.startTime()) * thread._data->_secondsPerCycle);
        buf.append("\nEnd:   ");
        Format::Time(buf, (node._endTime - frame.startTime()) * thread._data->_secondsPerCycle);
        buf.append("\nTime:  ");
        Format::Time(buf, (node._endTime - node._beginTime) * thread._data->_secondsPerCycle);
        ImGui::SetTooltip("%s", buf.c_str());
    }
}

void TimeLineView::draw() {
    if (_threads.empty()) {
        return;
    }

    ImGuiWindowFlags childFlags = ImGuiWindowFlags_HorizontalScrollbar;
    if (!ImGui::BeginChild("TimelineView", ImVec2(0, 0), false, childFlags)) {
        ImGui::EndChild();
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float contentHeight = std::max(calcContentHeight(), avail.y);
    ImGui::InvisibleButton("TimelineCanvas", ImVec2(std::max(avail.x, _leftWidth + 200.0f), contentHeight));
    const bool hovered = ImGui::IsItemHovered();

    ImRect canvas(origin, origin + ImVec2(std::max(avail.x, _leftWidth + 200.0f), contentHeight));
    ImRect leftRect(canvas.Min, ImVec2(canvas.Min.x + _leftWidth, canvas.Max.y));
    ImRect rulerRect(ImVec2(leftRect.Max.x, canvas.Min.y), ImVec2(canvas.Max.x, canvas.Min.y + _rulerHeight));
    ImRect bodyRect(ImVec2(leftRect.Max.x, rulerRect.Max.y), canvas.Max);

    const uint64_t totalTicks = std::max<uint64_t>(1, _processEnd - _processStart);
    const double minTicks = std::max(1.0, totalTicks / 100000.0);
    double visibleTicks = std::max(minTicks, _viewEnd - _viewStart);
    if (visibleTicks > totalTicks) {
        visibleTicks = (double)totalTicks;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (hovered) {
        if (io.KeyCtrl && io.MouseWheel != 0.0f) {
            const double oldVisible = visibleTicks;
            const double zoom = io.MouseWheel > 0 ? 0.82 : 1.22;
            visibleTicks = std::max(minTicks, std::min((double)totalTicks, visibleTicks * zoom));
            const double mouseRatio = bodyRect.GetWidth() > 0 ? ImClamp((io.MousePos.x - bodyRect.Min.x) / bodyRect.GetWidth(), 0.0f, 1.0f) : 0.5f;
            const double anchor = _viewStart + oldVisible * mouseRatio;
            _viewStart = anchor - visibleTicks * mouseRatio;
            _viewEnd = _viewStart + visibleTicks;
        }
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            const double ticksPerPixel = visibleTicks / std::max(1.0f, bodyRect.GetWidth());
            const double delta = -io.MouseDelta.x * ticksPerPixel;
            _viewStart += delta;
            _viewEnd += delta;
        }
        if (io.MouseWheelH != 0.0f) {
            const double delta = -io.MouseWheelH * visibleTicks * 0.08;
            _viewStart += delta;
            _viewEnd += delta;
        }
    }

    if (_viewStart < 0) {
        _viewEnd -= _viewStart;
        _viewStart = 0;
    }
    if (_viewEnd > totalTicks) {
        const double overflow = _viewEnd - totalTicks;
        _viewStart = std::max(0.0, _viewStart - overflow);
        _viewEnd = (double)totalTicks;
    }
    visibleTicks = std::max(minTicks, _viewEnd - _viewStart);
    const double ticksToPixels = bodyRect.GetWidth() / visibleTicks;

    drawList->AddRectFilled(canvas.Min, canvas.Max, ImColor(0.075f, 0.080f, 0.090f, 1.0f));
    drawList->AddRectFilled(leftRect.Min, leftRect.Max, ImColor(0.095f, 0.100f, 0.115f, 1.0f));
    drawList->AddLine(ImVec2(leftRect.Max.x, canvas.Min.y), ImVec2(leftRect.Max.x, canvas.Max.y), ImColor(1.0f, 1.0f, 1.0f, 0.12f));
    drawRuler(drawList, rulerRect, bodyRect, visibleTicks, ticksToPixels);

    ImGui::PushClipRect(canvas.Min, canvas.Max, true);
    float y = bodyRect.Min.y;
    shared::StrBuf label;
    for (size_t t = 0; t < _threads.size(); ++t) {
        ThreadData& thread = _threads[t];
        const auto* data = thread._data;
        const ImRect headerRect(ImVec2(canvas.Min.x, y), ImVec2(canvas.Max.x, y + _threadHeaderHeight));
        drawList->AddRectFilled(headerRect.Min, headerRect.Max, ImColor(1.0f, 1.0f, 1.0f, 0.035f));

        ImGui::SetCursorScreenPos(ImVec2(leftRect.Min.x + 4.0f, y + 2.0f));
        ImGui::PushID((int)t);
        if (ImGui::ArrowButton("expand", thread._expended ? ImGuiDir_Down : ImGuiDir_Right)) {
            thread._expended = !thread._expended;
        }
        ImGui::PopID();
        label.clear();
        label.appendf("Thread %d", data->_threadId);
        if (data->_threadName && data->_threadName[0]) {
            label.appendf(" (%s)", data->_threadName);
        }
        drawList->AddText(ImVec2(leftRect.Min.x + 26.0f, y + 3.0f), ImColor(kTimelineTextColor), label.c_str());

        y += _threadHeaderHeight + _rowGap;
        drawList->AddText(ImVec2(leftRect.Min.x + 26.0f, y + 2.0f), ImColor(kTimelineMutedTextColor), "Frames");
        drawList->AddLine(ImVec2(bodyRect.Min.x, y + _rowHeight + _rowGap), ImVec2(bodyRect.Max.x, y + _rowHeight + _rowGap), ImColor(1.0f, 1.0f, 1.0f, 0.05f));
        for (const auto& frame : data->_frames) {
            drawFrame(drawList, thread, frame, bodyRect, y, ticksToPixels);
        }

        y += _rowHeight + _rowGap;
        if (thread._expended && data->_maxCallDepth) {
            const float depthStartY = y;
            for (uint32_t depth = 0; depth < data->_maxCallDepth; ++depth) {
                label.clear();
                label.appendf("Depth %d", depth);
                drawList->AddText(ImVec2(leftRect.Min.x + 26.0f, y + 2.0f), ImColor(kTimelineMutedTextColor), label.c_str());
                drawList->AddLine(ImVec2(bodyRect.Min.x, y + _rowHeight + _rowGap), ImVec2(bodyRect.Max.x, y + _rowHeight + _rowGap), ImColor(1.0f, 1.0f, 1.0f, 0.035f));
                y += _rowHeight + _rowGap;
            }
            for (const auto& frame : data->_frames) {
                for (uint32_t n = 0; n < frame.nodeCount(); ++n) {
                    const uint32_t depth = thread.nodeDepth(frame, n);
                    const float nodeY = depthStartY + depth * (_rowHeight + _rowGap);
                    drawNode(drawList, thread, frame, n, bodyRect, nodeY, ticksToPixels);
                }
            }
        }
    }
    ImGui::PopClipRect();

    ImGui::EndChild();
}
