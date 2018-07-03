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
    onFrameSelectChange(0, 0);
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

    if (!ImGui::Begin("ImGui Demo", NULL, window_flags)) {
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
