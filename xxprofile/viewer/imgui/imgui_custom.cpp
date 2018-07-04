#include "imgui_custom.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_custom_internal.hpp"

float ImGui::GetIndent() {
    ImGuiWindow* window = GetCurrentWindow();
    return window->DC.IndentX;
}

float ImGui::GetContentWidth() {
    ImGuiWindow* window = GetCurrentWindow();
    return window->SizeContents.x;
}

void ImGui::PlotEx(ImGuiPlotType plot_type, ImPlotWithHitTest& value) {
    value.hoverItem = -1;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = value.keyId ? window->GetID(value.keyId) : window->GetID(value.data);

    if (value.graphSize.x == 0.0f) {
        value.graphSize.x = CalcItemWidth();
    }
    if (value.graphSize.y == 0.0f) {
        value.graphSize.y = (style.FramePadding.y * 2);
    }

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + value.graphSize);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb)) {
        return;
    }
    const bool hovered = ItemHoverable(inner_bb, id);

    // Determine scale from values if not specified
    if (value.scaleMin == FLT_MAX || value.scaleMax == FLT_MAX) {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < value.valuesCount; i++) {
            const float v = value.value(i);
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (value.scaleMin == FLT_MAX) {
            value.scaleMin = v_min;
        }
        if (value.scaleMax == FLT_MAX) {
            value.scaleMax = v_max;
        }
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    if (value.valuesCount > 0) {
        int res_w = ImMin((int)inner_bb.GetWidth(), value.valuesCount) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
        int item_count = value.valuesCount + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

        //const bool mouseInItem = IsMouseHoveringRect(inner_bb.Min, inner_bb.Max);

        const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
        const int v_idx = (int)(t * item_count);
        IM_ASSERT(v_idx >= 0 && v_idx < value.valuesCount);

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered) {
            const float v0 = value.value((v_idx) % value.valuesCount);
            const float v1 = value.value((v_idx + 1) % value.valuesCount);
            if (plot_type == ImGuiPlotType_Lines) {
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
            } else if (plot_type == ImGuiPlotType_Histogram) {
                if (value.formatValue) {
                    shared::StrBuf buf;
                    value.format(buf, v_idx);
                    SetTooltip("%s", buf.c_str());
                } else {
                    SetTooltip("%d: %8.4g", v_idx, v0);
                }
            }
            v_hovered = v_idx;
        }

        if (hovered) {
            value.hoverItem = v_idx;
        }
        if (hovered) {
            value.clickedItem = v_idx;
        }
        

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (value.scaleMin == value.scaleMax) ? 0.0f : (1.0f / (value.scaleMax - value.scaleMin));

        float v0 = value.value(0);
        float t0 = 0.0f;
        // Point in the normalized space of our target rectangle
        ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - value.scaleMin) * inv_scale) );
        // Where does the zero line stands
        float histogram_zero_line_t = (value.scaleMin * value.scaleMax < 0.0f) ? (-value.scaleMin * inv_scale) : (value.scaleMin < 0.0f ? 0.0f : 1.0f);

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_selected = GetColorU32(ImGuiCol_CheckMark);

        ImVec2 vmin(inner_bb.Min), vmax(inner_bb.Max);
        for (int n = 0; n < res_w; n++) {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < value.valuesCount);
            const float v1 = value.value((v1_idx + 1) % value.valuesCount);
            const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - value.scaleMin) * inv_scale) );

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            const ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            const ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            const ImU32 col = v_hovered == v1_idx ? col_hovered : col_base;

            if (plot_type == ImGuiPlotType_Lines) {
                window->DrawList->AddLine(pos0, pos1, col);
            } else if (plot_type == ImGuiPlotType_Histogram) {
                //if (pos1.x >= pos0.x + 2.0f) {
                //    pos1.x -= 1.0f;
                //}
                window->DrawList->AddRectFilled(pos0, pos1, col);
            }
            
            if (v1_idx == value.selectedItem) {
                vmin.x = pos0.x;
                vmax.x = pos1.x;
                if (value.selectedCount > 1) {
                    vmax.x = ImLerp(inner_bb.Min.x, inner_bb.Max.x, t0 + t_step * value.selectedCount);
                }
            }
            
            t0 = t1;
            tp0 = tp1;
        }

        if (value.selectedItem >= 0 && value.selectedCount > 0) {
            vmin.x -= 1;
            vmax.x += 1;
            window->DrawList->AddRect(vmin, vmax, col_selected);
        }
    }

    // Text overlay
    if (value.overlayText) {
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value.overlayText, NULL, NULL, ImVec2(0.5f,0.0f));
    }
}

void ImGui::PlotHistogram(ImPlotWithHitTest& value) {
    PlotEx(ImGuiPlotType_Histogram, value);
}
