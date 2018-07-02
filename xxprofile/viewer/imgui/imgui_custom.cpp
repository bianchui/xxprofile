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

void ImGui::PlotEx(ImGuiPlotType plot_type, ImHistogramWithHitTest& value) {
    value.hoverItem = -1;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    if (value.graph_size.x == 0.0f) {
        value.graph_size.x = CalcItemWidth();
    }
    if (value.graph_size.y == 0.0f) {
        value.graph_size.y = (style.FramePadding.y * 2);
    }

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + value.graph_size);
    const ImRect inner_bb(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding);
    const ImRect total_bb(frame_bb.Min, frame_bb.Max);
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, 0, &frame_bb)) {
        return;
    }
    const bool hovered = ItemHoverable(inner_bb, 0);

    // Determine scale from values if not specified
    if (value.scale_min == FLT_MAX || value.scale_max == FLT_MAX) {
        float v_min = FLT_MAX;
        float v_max = -FLT_MAX;
        for (int i = 0; i < value.values_count; i++) {
            const float v = value.value(i);
            v_min = ImMin(v_min, v);
            v_max = ImMax(v_max, v);
        }
        if (value.scale_min == FLT_MAX) {
            value.scale_min = v_min;
        }
        if (value.scale_max == FLT_MAX) {
            value.scale_max = v_max;
        }
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    if (value.values_count > 0) {
        int res_w = ImMin((int)value.graph_size.x, value.values_count) + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);
        int item_count = value.values_count + ((plot_type == ImGuiPlotType_Lines) ? -1 : 0);

        // Tooltip on hover
        int v_hovered = -1;
        if (hovered) {
            const float t = ImClamp((g.IO.MousePos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x), 0.0f, 0.9999f);
            const int v_idx = (int)(t * item_count);
            IM_ASSERT(v_idx >= 0 && v_idx < value.values_count);

            const float v0 = value.value((v_idx + value.values_offset) % value.values_count);
            const float v1 = value.value((v_idx + 1 + value.values_offset) % value.values_count);
            if (plot_type == ImGuiPlotType_Lines) {
                SetTooltip("%d: %8.4g\n%d: %8.4g", v_idx, v0, v_idx+1, v1);
            } else if (plot_type == ImGuiPlotType_Histogram) {
                SetTooltip("%d: %8.4g", v_idx, v0);
            }
            v_hovered = v_idx;

            value.hoverItem = v_hovered;

            if (g.IO.MouseReleased[0]) {
                value.clickedItem = v_hovered;
            }
        }

        const float t_step = 1.0f / (float)res_w;
        const float inv_scale = (value.scale_min == value.scale_max) ? 0.0f : (1.0f / (value.scale_max - value.scale_min));

        float v0 = value.value((0 + value.values_offset) % value.values_count);
        float t0 = 0.0f;
        // Point in the normalized space of our target rectangle
        ImVec2 tp0 = ImVec2( t0, 1.0f - ImSaturate((v0 - value.scale_min) * inv_scale) );
        // Where does the zero line stands
        float histogram_zero_line_t = (value.scale_min * value.scale_max < 0.0f) ? (-value.scale_min * inv_scale) : (value.scale_min < 0.0f ? 0.0f : 1.0f);

        const ImU32 col_base = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLines : ImGuiCol_PlotHistogram);
        const ImU32 col_hovered = GetColorU32((plot_type == ImGuiPlotType_Lines) ? ImGuiCol_PlotLinesHovered : ImGuiCol_PlotHistogramHovered);
        const ImU32 col_selected = GetColorU32(ImGuiCol_CheckMark);

        for (int n = 0; n < res_w; n++) {
            const float t1 = t0 + t_step;
            const int v1_idx = (int)(t0 * item_count + 0.5f);
            IM_ASSERT(v1_idx >= 0 && v1_idx < value.values_count);
            const float v1 = value.value((v1_idx + value.values_offset + 1) % value.values_count);
            const ImVec2 tp1 = ImVec2( t1, 1.0f - ImSaturate((v1 - value.scale_min) * inv_scale) );

            // NB: Draw calls are merged together by the DrawList system. Still, we should render our batch are lower level to save a bit of CPU.
            ImVec2 pos0 = ImLerp(inner_bb.Min, inner_bb.Max, tp0);
            ImVec2 pos1 = ImLerp(inner_bb.Min, inner_bb.Max, (plot_type == ImGuiPlotType_Lines) ? tp1 : ImVec2(tp1.x, histogram_zero_line_t));
            const ImU32 col = v_hovered == v1_idx ? col_hovered : col_base;

            if (plot_type == ImGuiPlotType_Lines) {
                window->DrawList->AddLine(pos0, pos1, col);
            } else if (plot_type == ImGuiPlotType_Histogram) {
                if (pos1.x >= pos0.x + 2.0f) {
                    pos1.x -= 1.0f;
                }
                window->DrawList->AddRectFilled(pos0, pos1, col);
            }
            
            if (v1_idx == value.selectedItem) {
                ImVec2 vmin(pos0.x, inner_bb.Min.y), vmax(pos1.x, inner_bb.Max.y);
                float w = vmax.x - vmin.x;
                if (w < 3) {
                    vmin.x -= 1;
                    vmax.x += 1;
                }
                window->DrawList->AddRect(vmin, vmax, col_selected);
            }
            
            t0 = t1;
            tp0 = tp1;
        }
    }
    
    // Text overlay
    if (value.overlay_text) {
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value.overlay_text, NULL, NULL, ImVec2(0.5f,0.0f));
    }
}

void ImGui::PlotHistogram(ImHistogramWithHitTest& value) {
    PlotEx(ImGuiPlotType_Histogram, value);
}
