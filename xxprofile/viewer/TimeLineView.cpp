#include "TimeLineView.hpp"
#include "imgui.h"

static const char* TimeLineView_scaleText = " Scale";

TimeLineView::TimeLineView() : _loader(NULL) {

    struct Funcs {
        static float Fun(void*, int i) { return 1.0f * i / 1000; }
    };

    memset(&_hitTest, 0, sizeof(_hitTest));

    _hitTest.values_getter = Funcs::Fun;
    _hitTest.data = this;
    //_hitTest.values_offset;
    //_hitTest.overlay_text;
    _hitTest.scale_min = 0;
    _hitTest.scale_max = 1.0f;
    //_hitTest.graph_size;
}

TimeLineView::~TimeLineView() {
    clear();
}

void TimeLineView::clear() {

}

void TimeLineView::setLoader(const xxprofile::Loader* loader) {
    clear();
    _loader = loader;
    if (loader) {
        //loader->
    }
}

float TimeLineView::calcHeight() {
    float height = 0;
    const ImGuiStyle& style = ImGui::GetStyle();

    {// Frames
        height += FramesItemHeight;
        height += style.WindowPadding.y * 2;
    }

    height += style.ItemSpacing.y;

    {// Scale
        const ImVec2 label_size = ImGui::CalcTextSize(TimeLineView_scaleText, NULL, true);
        height += label_size.y;
        height += style.FramePadding.y * 2.0f;
    }

    return height;
}

void TimeLineView::draw() {
    ImGuiWindowFlags window_flags = 0;
    ImGuiStyle& style = ImGui::GetStyle();

    const float rw = ImGui::GetWindowContentRegionWidth();

    if (!ImGui::BeginChild("TimeLine", ImVec2(rw, calcHeight()), true, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::EndChild();
        return;
    }
    ImGui::PushItemWidth(-1);

    float s = ImGui::GetIndent();

    static int display_count = 1000;

    {// Frames
        const float framesWidth = rw - style.WindowPadding.x * 2 - s + style.FramePadding.x * 2;

        float itemCount = framesWidth;

        static int i = 0;
        _hitTest.values_count = 500;
        _hitTest.selectedItem = (++i)%_hitTest.values_count;
        _hitTest.graph_size = ImVec2(framesWidth, FramesItemHeight);

        ImGui::PlotHistogram(_hitTest);
    }

    {// Scale
        static int is = 0;
        ImGui::SliderInt(TimeLineView_scaleText, &is, 0, 100);
    }

    ImGui::EndChild();
}
