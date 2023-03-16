// Copyright 2018 bianchui. All rights reserved.
#include "mainwin.hpp"
#include "imgui/imgui_custom.hpp"
#include <stdio.h>
#include <math.h>

#include <limits.h>         // INT_MIN, INT_MAX

MainWin::MainWin() : _framesLineView(this) {
    _framesLineView.setLoader(&_loader);
    _frameView.setLoader(&_loader);
}

bool MainWin::load(const char* file) {
    _loader.clear();
    bool success = _load(file);
    _framesLineView.setLoader(&_loader);
    _frameView.setLoader(&_loader);
    onFrameSelectChange(nullptr);
    return success;
}

bool MainWin::_load(const char* file) {
    xxprofile::Archive ar;
    if (!ar.open(file, false)) {
        return false;
    }
    _loader.load(ar);
    return true;
}

const char* formatTimeMS(char* buf, double time) {
    if (time > 60) {
        uint32_t minutes = (uint32_t)time / 60;
        buf += sprintf(buf, "%d:", minutes);
        time -= minutes * 60;
        uint32_t seconds = (uint32_t)time;
        buf += sprintf(buf, "%02d", seconds);
        time -= seconds;
    } else {
        uint32_t seconds = (uint32_t)time;
        buf += sprintf(buf, "%02d", seconds);
        time -= seconds;
    }
    uint32_t ms = (uint32_t)(time * 1000);
    sprintf(buf, ".%03d", ms);
    return buf;
}

/**
 * [time][frames][compressRate]
 */
std::string MainWin::getTitle() const {
    std::string ret;
    char buf[256];
    if (!_loader._threads.empty()) {
        uint64_t startTime = _loader._processStart;
        uint64_t endTime = _loader._threads[0].endTime();
        uint32_t frames = (uint32_t)_loader._threads[0]._frames.size();
        for (auto iter = _loader._threads.begin() + 1, end = _loader._threads.end(); iter != end; ++iter) {
            endTime = std::max(endTime, iter->endTime());
            frames = std::max(frames, (uint32_t)iter->_frames.size());
        }
        formatTimeMS(buf, (endTime - startTime) * _loader._secondsPerCycle);
        ret += "[";
        ret += buf;
        ret += "]";
        sprintf(buf, "[%dFrames][%02.2f%%]", frames, (100.0 * _loader._fileSize / _loader._dataSize));
        ret += buf;
    }
    return ret;
}

float GetItemMaxWidth() {
    float s = ImGui::GetIndent();
    float w = ImGui::GetContentWidth();

    const ImGuiStyle& style = ImGui::GetStyle();
    return w - s - 2 * style.WindowBorderSize - 2 * style.FrameBorderSize;
}

void MainWin::draw(int w, int h) {

    // Demonstrate the various window flags. Typically you would just use the default.
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

    if (!ImGui::Begin("MainWin", NULL, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(w, h));

    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
    ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

    _framesLineView.draw();
    _frameView.draw();

    ImGui::End();
}
