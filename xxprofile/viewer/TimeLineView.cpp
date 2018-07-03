#include "TimeLineView.hpp"
#include "imgui.h"

static const char* TimeLineView_scaleText = " Scale";

TimeLineView::TimeLineView(EventHandler* handler) : _handler(handler), _loader(NULL) {

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
        size_t tcount = loader->_threads.size();
        _threads.resize(tcount);
        for (size_t t = 0; t < tcount; ++t) {
            auto& thread = _threads[t];
            const auto& loader_thread = loader->_threads[t];
            thread.init(&loader_thread);
        }
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

void TimeLineView::ThreadData::init(const xxprofile::ThreadData* data) {
    _data = data;
    assert(data);
}

float TimeLineView::ThreadData::StaticGetData(void* p, int idx) {
    const ThreadData* data = (const ThreadData*)p;
    const auto& frames = data->_data->_frames;
    const int index = data->startIndex + idx;
    const xxprofile::FrameData& frameData = frames[index % frames.size()];
    double value = frameData.frameCycles() * data->_data->_secondsPerCycle;
    return (float)value;
}

void TimeLineView::ThreadData::setTo(ImGui::ImPlotWithHitTest& plot) const {
    plot.data = (void*)this;
    plot.valuesGetter = StaticGetData;
    plot.scaleMax = (float)(_data->_maxCycleCount * _data->_secondsPerCycle);
    const size_t count = _data->_frames.size();
    plot.valuesCount = (int)count;
}

void TimeLineView::draw() {
    ImGuiWindowFlags window_flags = 0;
    const ImGuiStyle& style = ImGui::GetStyle();

    const float rw = ImGui::GetWindowContentRegionWidth();

    if (!ImGui::BeginChild("TimeLine", ImVec2(rw, calcHeight()), true, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::EndChild();
        return;
    }
    ImGui::PushItemWidth(-1);

    float s = ImGui::GetIndent();

    {// Frames
        const float framesGraphWidth = rw - style.WindowPadding.x * 2 - s + style.FramePadding.x * 2;
        const int maxItemCount = (int)framesGraphWidth * 0.5f;
        assert(_threads.size() == _loader->_threads.size());
        ImGui::ImPlotWithHitTest plot;
        memset(&plot, 0, sizeof(plot));
        plot.scaleMin = 0;
        plot.scaleMax = 1.0f;
        plot.graphSize = ImVec2(framesGraphWidth, FramesItemHeight);
        const auto& io = ImGui::GetIO();

        for (size_t t = 0; t < _threads.size(); ++t) {
            auto& thread = _threads[t];
            thread.setTo(plot);
            if (plot.valuesCount > maxItemCount) {
                plot.valuesCount = maxItemCount;
            }
            plot.selectedItem = thread.selectedItem;

            ImGui::PlotHistogram(plot);

            if (io.MouseDown[0] && plot.hoverItem >= 0) {
                thread.selectedItem = plot.hoverItem - thread.startIndex;
                _handler->onFrameSelectChange((int)t, thread.selectedItem);
            }
        }
    }

    {// Scale
        static int is = 0;
        ImGui::SliderInt(TimeLineView_scaleText, &is, 0, 5);
    }

    ImGui::EndChild();
}
