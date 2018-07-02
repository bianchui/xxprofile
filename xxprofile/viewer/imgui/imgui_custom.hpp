// Copyright 2018 bianchui. All rights reserved.
#ifndef imgui_custom_hpp
#define imgui_custom_hpp
#include "imgui.h"

namespace ImGui {
    IMGUI_API float GetIndent();
    IMGUI_API float GetContentWidth();

    struct ImHistogramWithHitTest {
        float (*values_getter)(void* data, int idx);
        void* data;
        int values_count;
        int values_offset;
        const char* overlay_text;
        float scale_min;
        float scale_max;
        ImVec2 graph_size;
        int selectedItem;
        int clickedItem;
        int hoverItem;

        float value(int idx) {
            return values_getter(data, idx);
        }
    };

    IMGUI_API void PlotHistogram(ImHistogramWithHitTest& value);
}

#endif//imgui_custom_hpp
