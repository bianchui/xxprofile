// Copyright (C) 2018, bianchui. All rights reserved.
#include "mainwin.hpp"
#include "imgui/imgui_custom.hpp"
#include <stdio.h>
#include <math.h>
#include <imgui_internal.h>

#include <limits.h>         // INT_MIN, INT_MAX

MainWin::MainWin() : _viewType(0), _framesLineView(this), _timeLineView(this) {
    _framesLineView.setLoader(&_loader);
    _frameView.setLoader(&_loader);
    _timeLineView.setLoader(&_loader);
}

bool MainWin::load(const char* file) {
    _loader.clear();
    bool success = _load(file);
    _framesLineView.setLoader(&_loader);
    _frameView.setLoader(&_loader);
    _timeLineView.setLoader(&_loader);
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

/**
 * [time][frames][compressRate]
 */
std::string MainWin::getTitle() const {
    shared::StrBuf buf;
    if (_loader.thread_count()) {
        uint64_t startTime = _loader.processStart();
        uint64_t endTime = _loader.processEnd();
        uint32_t frames = (uint32_t)_loader.thread(0)._frames.size();
        for (auto iter = _loader.threads().begin() + 1, end = _loader.threads().end(); iter != end; ++iter) {
            frames = std::max(frames, (uint32_t)iter->_frames.size());
        }
        buf.append("[");
        Format::TimeMS(buf, (endTime - startTime) * _loader.secondsPerCycle());
        buf.append("]");
        buf.appendf("[%dFrames][%02.2f%%]", frames, (100.0 * _loader.fileSize() / _loader.dataSize()));
    }
    return buf.c_str();
}

float GetItemMaxWidth() {
    float s = ImGui::GetIndent();
    float w = ImGui::GetContentWidth();

    const ImGuiStyle& style = ImGui::GetStyle();
    return w - s - 2 * style.WindowBorderSize - 2 * style.FrameBorderSize;
}

void ImGui_CenteredText(const char* text) {
    float window_width = ImGui::GetWindowWidth();
    float window_height = ImGui::GetWindowHeight();
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + window_width * 0.5f - size.x * 0.5f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + window_height * 0.5f - size.y * 0.5f);
    ImGui::TextUnformatted(text);
}

void ImGui_HideTabBar() {
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);
}

void MainWin::draw(int w, int h) {

    // Demonstrate the various window flags. Typically you would just use the default.
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    window_flags |= ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    //window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
    window_flags |= ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    const auto mainWndBegin = ImGui::Begin("MainWin", NULL, window_flags);
    ImGui::PopStyleVar(3);

    if (mainWndBegin) {
        drawContent();
    }

    ImGui::End();
}

void MainWin::drawContent() {
    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
    ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            if (ImGui::BeginMenu("View Type")) {
                ImGui::RadioButton("Frame", &_viewType, 0);
                ImGui::RadioButton("Timeline", &_viewType, 1);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    //dockspace_flags |= ImGuiDockNodeFlags_AutoHideTabBar;
    dockspace_flags |= ImGuiDockNodeFlags_NoUndocking;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGuiWindowFlags window_flags = 0;
    //window_flags |= ImGuiWindowFlags_NoTitleBar;
    //window_flags |= ImGuiWindowFlags_NoMove;
    //window_flags |= ImGuiWindowFlags_NoResize;
    //window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
    window_flags |= ImGuiWindowFlags_HorizontalScrollbar;

    static int layoutViewType = -1;
    if (_loader.thread_count() == 0) {
        ImGui_CenteredText("drop or double click a .xxprofile file to open.");
    } else if (_viewType == 0) {
        if (layoutViewType != _viewType) {
            layoutViewType = _viewType;
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, nullptr, &dockspace_id);
            ImGui::DockBuilderDockWindow("Left", dock_id_left);
            ImGui::DockBuilderDockWindow("Right", dockspace_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        ImGui_HideTabBar();
        ImGui::Begin("Left", NULL, window_flags);
        _framesLineView.draw();
        ImGui::End();
        ImGui_HideTabBar();
        ImGui::Begin("Right", NULL, window_flags);
        _frameView.draw();
        ImGui::End();
    } else if (_viewType == 1) {
        if (layoutViewType != _viewType) {
            layoutViewType = _viewType;
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            ImGui::DockBuilderDockWindow("Timeline", dockspace_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        ImGui_HideTabBar();
        ImGui::Begin("Timeline", NULL, window_flags);
        _timeLineView.draw();
        ImGui::End();
    }
}
