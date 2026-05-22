// Copyright (C) 2018-2026, bianchui. All rights reserved.
#include "mainloop_backend.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <GLFW/glfw3.h>

void mainloopBackendApplyWindowHints() {
}

bool mainloopBackendInit(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
    return true;
}

bool mainloopBackendNewFrame(GLFWwindow* window, const ImVec4& clearColor, int display_w, int display_h, int window_w, int window_h) {
    (void)window;
    (void)clearColor;
    (void)display_w;
    (void)display_h;
    (void)window_w;
    (void)window_h;
    ImGui_ImplOpenGL2_NewFrame();
    return true;
}

void mainloopBackendRender(GLFWwindow* window, ImDrawData* drawData, const ImVec4& clearColor, int display_w, int display_h) {
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(drawData);

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void mainloopBackendShutdown(GLFWwindow* window) {
    (void)window;
    ImGui_ImplOpenGL2_Shutdown();
}
