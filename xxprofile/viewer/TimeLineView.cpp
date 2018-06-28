#include "TimeLineView.hpp"
#include "imgui.h"

TimeLineView::TimeLineView() : _loader(NULL) {

    struct Funcs {
        static float Fun(void*, int i) { return 1.0f * i / 1000; }
    };

    memset(&_hitTest, 0, sizeof(_hitTest));

    _hitTest.label = "";
    _hitTest.values_getter = Funcs::Fun;
    _hitTest.data = this;
    //_hitTest.values_offset;
    //_hitTest.overlay_text;
    _hitTest.scale_min = 0;
    _hitTest.scale_max = 1.0f;
    //_hitTest.graph_size;
}

void TimeLineView::setLoader(xxprofile::Loader* loader) {
    _loader = loader;
}

void TimeLineView::draw() {
    ImGuiWindowFlags window_flags = 0;
    ImGuiStyle& style = ImGui::GetStyle();

    float rw = ImGui::GetWindowContentRegionWidth();

    float itemHeight = 100;

    if (!ImGui::BeginChild("TimeLine", ImVec2(rw, itemHeight + style.WindowPadding.y * 2), true, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::EndChild();
        return;
    }
    ImGui::PushItemWidth(-1);

    float s = ImGui::GetIndent();
    static int display_count = 1000;

    //  - style.FramePadding.y * 2
    float itemWidth = rw - style.WindowPadding.x * 2 - s;
    int itemCount = (int)itemWidth;

    static int i = 0;

    _hitTest.values_count = 100;
    _hitTest.hitItem = (++i)%_hitTest.values_count;
    _hitTest.graph_size = ImVec2(itemWidth, itemHeight);

    ImGui::PlotHistogram(_hitTest);
    ImGui::EndChild();
}
