#include "FramesLineView.hpp"
#include "imgui.h"

FramesLineView::FramesLineView(EventHandler* handler) : _handler(handler), _loader(NULL) {

}

FramesLineView::~FramesLineView() {
    clear();
}

void FramesLineView::clear() {

}

void FramesLineView::setLoader(const xxprofile::Loader* loader) {
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

float FramesLineView::calcHeight(int framesGraphWidthPixels) {
    float height = 0;
    const ImGuiStyle& style = ImGui::GetStyle();
    height += style.WindowPadding.y * 2;

    {// Frames
        for (size_t t = 0; t < _threads.size(); ++t) {
            auto& thread = _threads[t];
            thread.setGraphWidthPixels(framesGraphWidthPixels);
            if (thread.hasThumbnail()) {
                height += ThumbnailItemHeight;
                height += style.ItemSpacing.y;
            }
            height += FramesItemHeight;
            height += style.ItemSpacing.y;
        }
    }

    height -= style.ItemSpacing.y;

    return height;
}

void FramesLineView::draw() {
    if (_threads.empty()) {
        return;
    }
    ImGuiWindowFlags window_flags = 0;
    const ImGuiStyle& style = ImGui::GetStyle();

    const float rw = ImGui::GetWindowContentRegionWidth();
    const float indent = ImGui::GetIndent();
    const float plotItemWidth = (rw - style.WindowPadding.x * 2 - indent + style.FramePadding.x * 2);
    const int framesGraphWidthPixels = (int)(plotItemWidth - style.FramePadding.x * 2);

    if (!ImGui::BeginChild("FramesLine", ImVec2(rw, calcHeight(framesGraphWidthPixels)), true, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::EndChild();
        return;
    }
    ImGui::PushItemWidth(-1);

    {// Frames
        assert(_threads.size() == _loader->_threads.size());
        ImGui::ImPlotWithHitTest plot;
        shared::StrBuf text;
        const auto& io = ImGui::GetIO();
        shared::StrBuf keyId;
        for (size_t t = 0; t < _threads.size(); ++t) {
            auto& thread = _threads[t];

            // thumbnail
            if (thread.hasThumbnail()) {
                memset(&plot, 0, sizeof(plot));
                thread.setThumbnail(plot);
                keyId.printf("Thumbnail_%d", t);
                plot.keyId = keyId;
                plot.graphSize = ImVec2(plotItemWidth, ThumbnailItemHeight);
                ImGui::PlotHistogram(plot);
            }

            // frames
            memset(&plot, 0, sizeof(plot));
            thread.setFrames(plot);
            keyId.printf("Frames_%d", t);
            plot.keyId = keyId;
            plot.graphSize = ImVec2(plotItemWidth, FramesItemHeight);
            Math::FormatTime(text, thread.frameUseTime());
            plot.overlayText = text;

            ImGui::PlotHistogram(plot);

            if (io.MouseDown[0] && plot.hoverItem >= 0) {
                thread._selectedItem = plot.hoverItem - thread._startIndex;
                _handler->onFrameSelectChange((int)t, thread._selectedItem);
            }
        }
    }

    ImGui::EndChild();
}
