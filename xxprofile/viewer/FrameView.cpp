// Copyright 2018 bianchui. All rights reserved.
#include "FrameView.hpp"
#include "imgui/imgui_custom.hpp"
#include "EventHandler.hpp"

FrameView::FrameView() : _loader(NULL), _frameData(NULL) {
    
}

FrameView::~FrameView() {
    clear();
}

void FrameView::clear() {
    _frameData = NULL;
}

void FrameView::setLoader(const xxprofile::Loader* loader) {
    _loader = loader;
}

void FrameView::setFrameData(const xxprofile::FrameData* data) {
    if (data == _frameData) {
        return;
    }
    clear();
    _frameData = data;
}

static const ImVec4 kColorRed(1, 0, 0, 1);
static const ImVec4 kColorWhite(1, 1, 1, 1);
static const ImVec4 kColorYellow(1, 1, 0, 1);

void FrameView::draw() {
    if (!_frameData) {
        return;
    }
    ImGuiStyle& style = ImGui::GetStyle();
    struct DrawTreeNode {
        double _secondsPerCycle;
        uint64_t frameCycles;
        double frameTimes;
        ImGuiStyle* style;
        const xxprofile::FrameData* data;
        shared::StrBuf _timeBuffer;
        void draw(const xxprofile::TreeItem* item, uint64_t parentCycles) {
            double percentage = (item->useCycles() * 1000000 / parentCycles) * 0.0001;
            double framePercentage = (item->useCycles() * 1000000 / frameCycles) * 0.0001;
            if (percentage > 50) {
                style->Colors[ImGuiCol_Text] = kColorRed;
            } else if (percentage > 30) {
                style->Colors[ImGuiCol_Text] = kColorYellow;
            } else {
                style->Colors[ImGuiCol_Text] = kColorWhite;
            }
            _timeBuffer.clear();
            Math::FormatTime(_timeBuffer, item->useCycles() * _secondsPerCycle);
            if (item->children) {
                bool expanded = ImGui::TreeNode(item->name, "(%0.4f%% %s) %s", percentage, _timeBuffer.c_str(), item->name);
                tooltip("Frame: %0.4f%%\nParent:%0.4f%%\nTime:  %s\n%s", framePercentage, percentage, _timeBuffer.c_str(), item->name);
                if (expanded) {
                    for (auto iter = item->children->begin(); iter != item->children->end(); ++iter) {
                        draw(*iter, item->useCycles());
                    }
                    ImGui::TreePop();
                }
            } else {
                ImGui::BulletText("(%0.4f%% %s) %s", percentage, _timeBuffer.c_str(), item->name);
                tooltip("Frame: %0.4f%%\nParent:%0.4f%%\nTime:  %s\n%s", framePercentage, percentage, _timeBuffer.c_str(), item->name);
            }
        }
        void tooltip(const char* fmt, ...) {
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
                va_list args;
                va_start(args, fmt);
                ImGui::TextV(fmt, args);
                va_end(args);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
    };
    DrawTreeNode drawtv;
    drawtv.data = _frameData;
    drawtv._secondsPerCycle = 0;
    if (_loader->_threads.size()) {
        drawtv._secondsPerCycle = _loader->_threads[0]._secondsPerCycle;
    }
    drawtv.frameCycles = _frameData->frameCycles();
    if (drawtv.frameCycles == 0) {
        drawtv.frameCycles = 1;
    }
    drawtv.frameTimes = drawtv._secondsPerCycle * drawtv.frameCycles;
    drawtv.style = &style;
    const ImVec4 colorOld = style.Colors[ImGuiCol_Text];
    for (auto iter = _frameData->roots().begin(); iter != _frameData->roots().end(); ++iter) {
        drawtv.draw(*iter, drawtv.frameCycles);
    }
    style.Colors[ImGuiCol_Text] = colorOld;
}
