#include "mainwin.hpp"
#include "imgui.h"
#include <stdio.h>
#include <math.h>

#include <limits.h>         // INT_MIN, INT_MAX

static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void ShowExampleMenuFile()
{
    ImGui::MenuItem("(dummy menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}
    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        static bool b = true;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::Checkbox("Check", &b);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x+sz, p.y+sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

void MainWin::draw(int w, int h) {
    // Examples apps
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;
    static bool show_app_style_editor = false;

    static bool show_app_metrics = false;

    //if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    //if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    //if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    //if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    //if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    //if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    //if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    //if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    //if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    //if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    //if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    if (show_app_metrics)             { ImGui::ShowMetricsWindow(&show_app_metrics); }
    //if (show_app_style_editor)        { ImGui::Begin("Style Editor", &show_app_style_editor); ImGui::ShowStyleEditor(); ImGui::End(); }

    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_nav = false;

    // Demonstrate the various window flags. Typically you would just use the default.
    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)       window_flags |= ImGuiWindowFlags_NoNav;

    if (!ImGui::Begin("ImGui Demo", NULL, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(w, h));

    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
    ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

    // Menu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {
            ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            ImGui::MenuItem("Console", NULL, &show_app_console);
            ImGui::MenuItem("Log", NULL, &show_app_log);
            ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
            ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
            ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (ImGui::CollapsingHeader("Window options"))
    {
        ImGui::Checkbox("No titlebar", &no_titlebar); ImGui::SameLine(150);
        ImGui::Checkbox("No scrollbar", &no_scrollbar); ImGui::SameLine(300);
        ImGui::Checkbox("No menu", &no_menu);
        ImGui::Checkbox("No nav", &no_nav);
    }

    if (ImGui::CollapsingHeader("Widgets"))
    {
        if (ImGui::TreeNode("Trees"))
        {
            if (ImGui::TreeNode("Basic trees"))
            {
                for (int i = 0; i < 5; i++)
                    if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
                    {
                        ImGui::Text("blah blah");
                        ImGui::SameLine();
                        if (ImGui::SmallButton("button")) { };
                        ImGui::TreePop();
                    }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Advanced, with Selectable nodes"))
            {
                ShowHelpMarker("This is a more standard looking tree with selectable nodes.\nClick to select, CTRL+Click to toggle, click on arrows or double-click to open.");
                static bool align_label_with_current_x_position = false;
                ImGui::Checkbox("Align label with current X position)", &align_label_with_current_x_position);
                ImGui::Text("Hello!");
                if (align_label_with_current_x_position)
                    ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

                static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
                int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize()*3); // Increase spacing to differentiate leaves from expanded contents.
                for (int i = 0; i < 6; i++)
                {
                    // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
                    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
                    if (i < 3)
                    {
                        // Node
                        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                        if (ImGui::IsItemClicked())
                            node_clicked = i;
                        if (node_open)
                        {
                            ImGui::Text("Blah blah\nBlah Blah");
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        // Leaf: The only reason we have a TreeNode at all is to allow selection of the leaf. Otherwise we can use BulletText() or TreeAdvanceToLabelPos()+Text().
                        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                        ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                        if (ImGui::IsItemClicked())
                            node_clicked = i;
                    }
                }
                if (node_clicked != -1)
                {
                    // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
                    if (ImGui::GetIO().KeyCtrl)
                        selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                    else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
                        selection_mask = (1 << node_clicked);           // Click to single-select
                }
                ImGui::PopStyleVar();
                if (align_label_with_current_x_position)
                    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Bullets"))
        {
            ImGui::BulletText("Bullet point 1");
            ImGui::BulletText("Bullet point 2\nOn multiple lines");
            ImGui::Bullet(); ImGui::Text("Bullet point 3 (two calls)");
            ImGui::Bullet(); ImGui::SmallButton("Button");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Plots Widgets"))
        {
            static bool animate = true;
            ImGui::Checkbox("Animate", &animate);

            // Create a dummy array of contiguous float values to plot
            // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
            static float values[90] = { 0 };
            static int values_offset = 0;
            static float refresh_time = 0.0f;
            if (!animate || refresh_time == 0.0f)
                refresh_time = ImGui::GetTime();

            while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
            {
                static float phase = 0.0f;
                values[values_offset] = cosf(phase);
                values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
                phase += 0.10f*values_offset;
                refresh_time += 1.0f/60.0f;
            }
            ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0,80));

            // Use functions to generate output
            // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
            struct Funcs
            {
                static float Sin(void*, int i) { return sinf(i * 0.1f); }
                static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
            };
            static int func_type = 0, display_count = 1000;
            ImGui::Separator();
            ImGui::PushItemWidth(100); ImGui::Combo("func", &func_type, "Sin\0Saw\0"); ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::SliderInt("Sample count", &display_count, 1, 400);
            float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;

            //const char* label,
            //float (*values_getter)(void* data, int idx),
            //void* data,
            //int values_count,
            //int values_offset,
            //const char* overlay_text,
            //float scale_min,
            //float scale_max,
            //ImVec2 graph_size

            ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, "hahaha", -1.0f, 1.0f, ImVec2(0,100));
            ImGui::Separator();

            // Animate a simple progress bar
            static float progress = 0.0f, progress_dir = 1.0f;
            if (animate)
            {
                progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
                if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
                if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
            }

            // Typically we would use ImVec2(-1.0f,0.0f) to use all available width, or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
            ImGui::ProgressBar(progress, ImVec2(0.0f,0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("Progress Bar");

            float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
            char buf[32];
            sprintf(buf, "%d/%d", (int)(progress_saturated*1753), 1753);
            ImGui::ProgressBar(progress, ImVec2(0.f,0.f), buf);
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Columns"))
    {
        ImGui::PushID("Columns");

        // Basic columns
        if (ImGui::TreeNode("Basic"))
        {
            ImGui::Text("Without border:");
            ImGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
            ImGui::Separator();
            for (int n = 0; n < 14; n++)
            {
                char label[32];
                sprintf(label, "Item %d", n);
                if (ImGui::Selectable(label)) {}
                //if (ImGui::Button(label, ImVec2(-1,0))) {}
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();

            ImGui::Text("With border:");
            ImGui::Columns(4, "mycolumns"); // 4-ways, with border
            ImGui::Separator();
            ImGui::Text("ID"); ImGui::NextColumn();
            ImGui::Text("Name"); ImGui::NextColumn();
            ImGui::Text("Path"); ImGui::NextColumn();
            ImGui::Text("Hovered"); ImGui::NextColumn();
            ImGui::Separator();
            const char* names[3] = { "One", "Two", "Three" };
            const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
            static int selected = -1;
            for (int i = 0; i < 3; i++)
            {
                char label[32];
                sprintf(label, "%*d", i + 5, i);
                if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
                    selected = i;
                bool hovered = ImGui::IsItemHovered();
                ImGui::NextColumn();
                ImGui::Text(names[i]); ImGui::NextColumn();
                ImGui::Text(paths[i]); ImGui::NextColumn();
                ImGui::Text("%d", hovered); ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }

        // Create multiple items in a same cell before switching to next column
        if (ImGui::TreeNode("Mixed items"))
        {
            ImGui::Columns(3, "mixed");
            ImGui::Separator();

            ImGui::Text("Hello");
            ImGui::Button("Banana");
            ImGui::NextColumn();

            ImGui::Text("ImGui");
            ImGui::Button("Apple");
            static float foo = 1.0f;
            ImGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
            ImGui::Text("An extra line here.");
            ImGui::NextColumn();

            ImGui::Text("Sailor");
            ImGui::Button("Corniflower");
            static float bar = 1.0f;
            ImGui::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
            ImGui::NextColumn();

            if (ImGui::CollapsingHeader("Category A")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            if (ImGui::CollapsingHeader("Category B")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            if (ImGui::CollapsingHeader("Category C")) { ImGui::Text("Blah blah blah"); } ImGui::NextColumn();
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Borders"))
        {
            // NB: Future columns API should allow automatic horizontal borders.
            static bool h_borders = true;
            static bool v_borders = true;
            ImGui::Checkbox("horizontal", &h_borders);
            ImGui::SameLine();
            ImGui::Checkbox("vertical", &v_borders);
            ImGui::Columns(4, NULL, v_borders);
            for (int i = 0; i < 4*3; i++)
            {
                if (h_borders && ImGui::GetColumnIndex() == 0)
                    ImGui::Separator();
                ImGui::Text("%c%c%c", 'a'+i, 'a'+i, 'a'+i);
                ImGui::Text("Width %.2f\nOffset %.2f", ImGui::GetColumnWidth(), ImGui::GetColumnOffset());
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            if (h_borders)
                ImGui::Separator();
            ImGui::TreePop();
        }

        bool node_open = ImGui::TreeNode("Tree within single cell");
        ImGui::SameLine(); ShowHelpMarker("NB: Tree node must be poped before ending the cell. There's no storage of state per-cell.");
        if (node_open)
        {
            ImGui::Columns(2, "tree items");
            ImGui::Separator();
            if (ImGui::TreeNode("Hello")) { ImGui::BulletText("Sailor"); ImGui::TreePop(); } ImGui::NextColumn();
            if (ImGui::TreeNode("Bonjour")) { ImGui::BulletText("Marin"); ImGui::TreePop(); } ImGui::NextColumn();
            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    ImGui::End();
}
