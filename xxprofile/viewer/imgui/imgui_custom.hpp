// Copyright 2018 bianchui. All rights reserved.
#ifndef imgui_custom_hpp
#define imgui_custom_hpp
#include "imgui.h"
#include <shared/utils/StrBuf.h>

namespace ImGui {
    IMGUI_API float GetIndent();
    IMGUI_API float GetContentWidth();

    struct ImPlotWithHitTest {
        float (*valuesGetter)(void* data, int idx);
        void (*formatValue)(void* data, shared::StrBuf& buf, int idx);

        const char* keyId;
        void* data;
        int valuesCount;
        const char* overlayText;
        float scaleMin;
        float scaleMax;
        ImVec2 graphSize;
        int selectedItem;
        int selectedCount;
        int clickedItem;
        int hoverItem;
        int trackingItem;

        float value(int idx) {
            return valuesGetter(data, idx);
        }
        void format(shared::StrBuf& buf, int idx) {
            formatValue(data, buf, idx);
        }
    };

    IMGUI_API void PlotHistogram(ImPlotWithHitTest& value);
}

#endif//imgui_custom_hpp
