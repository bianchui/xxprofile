// Copyright 2018 bianchui. All rights reserved.
#include "FrameView.hpp"
#include "imgui/imgui_custom.hpp"
#include "EventHandler.hpp"
#include <unordered_map>

FrameView::FrameView() : _loader(nullptr), _frameData(nullptr), _frameDetail(nullptr), _combined(false) {
    
}

FrameView::~FrameView() {
    clear();
}

void FrameView::clear() {
    _frameData = nullptr;
    if (_frameDetail) {
        delete _frameDetail;
        _frameDetail = nullptr;
    }
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
    _frameStart = data ? data->startTime() : 0;
    if (data && _loader) {
        _frameDetail = new xxprofile::FrameDetail(*_loader, *data);
    }
}

static const ImVec4 kColorRed(1, 0, 0, 1);
static const ImVec4 kColorWhite(1, 1, 1, 1);
static const ImVec4 kColorYellow(1, 1, 0, 1);

void FrameView::draw() {
    if (!_frameDetail) {
        return;
    }
    if (_frameDetail->anyCombined()) {
        ImGui::Checkbox("Combined", &_combined);
    } else {
        _combined = false;
    }
    ImGuiStyle& style = ImGui::GetStyle();
    typedef std::unordered_map<const char*, uint32_t> children_names_map;
    struct DrawTreeNode {
        double _secondsPerCycle;
        uint64_t processStart;
        uint64_t frameStart;
        uint64_t frameCycles;
        double frameTimes;
        ImGuiStyle* style;
        const xxprofile::FrameDetail* data;
        shared::StrBuf _name;
        shared::StrBuf _timeBuffer;
        void draw(const xxprofile::TreeItem* item, uint64_t parentCycles, children_names_map& names) {
            const double percentage = (item->useCycles() * 1000000 / parentCycles) * 0.0001;
            if (percentage > 50) {
                style->Colors[ImGuiCol_Text] = kColorRed;
            } else if (percentage > 30) {
                style->Colors[ImGuiCol_Text] = kColorYellow;
            } else {
                style->Colors[ImGuiCol_Text] = kColorWhite;
            }
            _timeBuffer.clear();
            Math::FormatTime(_timeBuffer, item->useCycles() * _secondsPerCycle);
            if (item->_children) {
                _name.printf("%d%s", names[item->_name]++, item->_name);
                const bool expanded = ImGui::TreeNode(_name, "(%0.4f%% %s) %s", percentage, _timeBuffer.c_str(), item->_name);
                tooltip(item, percentage);
                if (expanded) {
                    draw(*item->_children, item->useCycles());
                    //children_names_map children_name;
                    //for (auto iter = item->_children->begin(); iter != item->_children->end(); ++iter) {
                    //    draw(*iter, item->useCycles(), children_name);
                    //}
                    ImGui::TreePop();
                }
            } else {
                ImGui::BulletText("(%0.4f%% %s) %s", percentage, _timeBuffer.c_str(), item->_name);
                tooltip(item, percentage);
            }
        }
        void draw(const std::vector<xxprofile::TreeItem*>& items, uint64_t parentCycles) {
            children_names_map children_name;
            for (auto iter = items.begin(); iter != items.end(); ++iter) {
                draw(*iter, parentCycles, children_name);
            }
        }
        void tooltip(const xxprofile::TreeItem* item, double percentage) {
            if (ImGui::IsItemHovered()) {
                const double framePercentage = (item->useCycles() * 1000000 / frameCycles) * 0.0001;
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
                _timeBuffer.appendf("\nSelf:  ");
                Math::FormatTime(_timeBuffer, (item->useCycles() - item->childrenCycles()) * _secondsPerCycle);
                const double selfPercentage = item->useCycles() ? ((item->useCycles() - item->childrenCycles()) * 1000000 / item->useCycles()) * 0.0001 : 0;
                _timeBuffer.appendf("(%0.4f%%)", selfPercentage);
                _timeBuffer.appendf("\nStart: ");
                Math::FormatTime(_timeBuffer, (item->_node->_beginTime - frameStart) * _secondsPerCycle);
                _timeBuffer.appendf("\nEnd:   ");
                Math::FormatTime(_timeBuffer, (item->_node->_endTime - frameStart) * _secondsPerCycle);
                ImGui::Text("Frame: %0.4f%%\nParent:%0.4f%%\nTime:  %s\n%s", framePercentage, percentage, _timeBuffer.c_str(), item->_name);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        // combined
        void drawCombined(const xxprofile::CombinedTreeItem* item, uint64_t parentCycles, children_names_map& names) {
            const double percentage = (item->useCycles() * 1000000 / parentCycles) * 0.0001;
            if (percentage > 50) {
                style->Colors[ImGuiCol_Text] = kColorRed;
            } else if (percentage > 30) {
                style->Colors[ImGuiCol_Text] = kColorYellow;
            } else {
                style->Colors[ImGuiCol_Text] = kColorWhite;
            }
            _timeBuffer.clear();
            Math::FormatTime(_timeBuffer, item->useCycles() * _secondsPerCycle);
            int combinedCount = (int)item->combinedCount();
            if (item->_children) {
                _name.printf("%d%s", names[item->name()]++, item->name());
                const bool expanded = ImGui::TreeNode(_name, "[%d](%0.4f%% %s) %s", combinedCount, percentage, _timeBuffer.c_str(), item->name());
                tooltip(item, percentage);
                if (expanded) {
                    drawCombined(*item->_children, item->useCycles());
                    ImGui::TreePop();
                }
            } else {
                ImGui::BulletText("[%d](%0.4f%% %s) %s", combinedCount, percentage, _timeBuffer.c_str(), item->name());
                tooltip(item, percentage);
            }
        }
        void drawCombined(const std::vector<xxprofile::CombinedTreeItem*>& items, uint64_t parentCycles) {
            children_names_map children_name;
            for (auto iter = items.begin(); iter != items.end(); ++iter) {
                drawCombined(*iter, parentCycles, children_name);
            }
        }

        void tooltip(const xxprofile::CombinedTreeItem* item, double percentage) {
            if (ImGui::IsItemHovered()) {
                const double framePercentage = (item->useCycles() * 1000000 / frameCycles) * 0.0001;
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);
                ImGui::Text("CallTimes: %d", (int)item->combinedCount());
                _timeBuffer.appendf("\nSelf:  ");
                Math::FormatTime(_timeBuffer, (item->useCycles() - item->childrenCycles()) * _secondsPerCycle);
                const double selfPercentage = item->useCycles() ? ((item->useCycles() - item->childrenCycles()) * 1000000 / item->useCycles()) * 0.0001 : 0;
                _timeBuffer.appendf("(%0.4f%%)", selfPercentage);
                ImGui::Text("Frame: %0.4f%%\nParent:%0.4f%%\nTime:  %s", framePercentage, percentage, _timeBuffer.c_str());

                if (item->combinedCount() > 1) {
                    _timeBuffer.clear();
                    Math::FormatTime(_timeBuffer, item->useCycles() * _secondsPerCycle / item->combinedCount());
                    ImGui::Text("Average: %0.4f%%\nParent:%0.4f%%\nTime:  %s", framePercentage / item->combinedCount(), percentage / item->combinedCount(), _timeBuffer.c_str());
                }

                ImGui::Text("%s", item->name());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
    };
    DrawTreeNode drawtv;
    drawtv.data = _frameDetail;
    drawtv._secondsPerCycle = 0;
    if (_loader->_threads.size()) {
        drawtv._secondsPerCycle = _loader->_threads[0]._secondsPerCycle;
    }
    drawtv.processStart = _loader->_processStart;
    drawtv.frameStart = _frameStart;
    drawtv.frameCycles = _frameData->frameCycles();
    if (drawtv.frameCycles == 0) {
        drawtv.frameCycles = 1;
    }
    drawtv.frameTimes = drawtv._secondsPerCycle * drawtv.frameCycles;
    drawtv.style = &style;
    const ImVec4 colorOld = style.Colors[ImGuiCol_Text];
    if (!_combined) {
        drawtv.draw(_frameDetail->roots(), drawtv.frameCycles);
    } else {
        drawtv.drawCombined(_frameDetail->combinedRoots(), drawtv.frameCycles);
    }
    style.Colors[ImGuiCol_Text] = colorOld;
}
