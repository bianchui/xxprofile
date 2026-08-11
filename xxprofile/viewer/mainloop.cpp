// Copyright (C) 2018-2026, bianchui. All rights reserved.
#include "mainloop_backend.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <shared/utils/StrBuf.h>

#define XX_ENABLE_PROFILE 0

#include "../src/xxprofile.hpp"

#include "mainwin.hpp"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static uint64_t getTimeStamp() {
    return (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

static void sleepToMinFrameTime(uint64_t start) {
    const uint64_t end = getTimeStamp();
    const uint64_t used = end - start;
    const uint64_t minTime = 33000;
    if (used >= minTime) {
        return;
    }

#ifdef _WIN32
    Sleep((DWORD)((minTime - used) / 1000));
#else
    struct timespec rqtp, rmtp;
    rqtp.tv_sec = 0;
    rqtp.tv_nsec = (minTime - used) * 1000;
    while (true) {
        int ret = nanosleep(&rqtp, &rmtp);
        if (!ret || rmtp.tv_nsec == 0) {
            break;
        }
        rqtp = rmtp;
    }
#endif
}

static void setupStyle() {
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameRounding = 3.0f;
    style.WindowRounding = 0.0f;
}

static MainWin mainwin;
static GLFWwindow* g_win = NULL;
static const char* kTitle = "xxprofileViewer";
static std::string g_title = kTitle;

static void glfw_setTitle(const char* title) {
    XX_PROFILE_SCOPE_FUNCTION();
    g_title = title;
    if (g_win) {
        glfwSetWindowTitle(g_win, title);
    }
}

static bool isProfileFile(const char* path) {
    if (!path) {
        return false;
    }
    const char* ext = strrchr(path, '.');
    if (!ext) {
        return false;
    }
    const char* expected = ".xxprofile";
    for (size_t i = 0; expected[i] || ext[i]; ++i) {
        char a = ext[i];
        char b = expected[i];
        if (a >= 'A' && a <= 'Z') {
            a = (char)(a - 'A' + 'a');
        }
        if (b >= 'A' && b <= 'Z') {
            b = (char)(b - 'A' + 'a');
        }
        if (a != b) {
            return false;
        }
    }
    return true;
}

static int glfw_onDocumentOpen(const char* name) {
    XX_PROFILE_SCOPE_FUNCTION();
    printf("%s\n", name);
    if (!mainwin.load(name)) {
        glfw_setTitle(kTitle);
        return GLFW_FALSE;
    }
    const char* p_name = strrchr(name, '/');
    if (!p_name) {
        p_name = name;
    } else {
        ++p_name;
    }
    shared::StrBuf title;
    title.printf("%s - %s - %s", p_name, mainwin.getTitle().c_str(), kTitle);
    glfw_setTitle(title);
    return GLFW_TRUE;
}

static void glfw_drop_callback(GLFWwindow* window, int count, const char** paths) {
    (void)window;
    for (int i = 0; i < count; ++i) {
        if (isProfileFile(paths[i])) {
            glfw_onDocumentOpen(paths[i]);
            return;
        }
    }
    if (count > 0 && paths && paths[0]) {
        printf("Drop ignored: expected a .xxprofile file, got %s\n", paths[0]);
    }
}

static void _mainLoop(const char* openFile) {
    XX_PROFILE_SCOPE_FUNCTION();
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return;
    }

    glfwSetOnDocumentOpen(glfw_onDocumentOpen);
    glfwDefaultWindowHints();
    mainloopBackendApplyWindowHints();

    GLFWwindow* window = glfwCreateWindow(1280, 720, kTitle, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    g_win = window;
    glfwSetWindowTitle(g_win, g_title.c_str());
    glfwSetDropCallback(window, glfw_drop_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // GLFW changes the working directory to the app bundle's Resources folder
    // on macOS.  Letting ImGui write its default relative imgui.ini there would
    // mutate the signed bundle and invalidate its code signature.
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    if (!mainloopBackendInit(window)) {
        ImGui::DestroyContext();
        g_win = NULL;
        glfwDestroyWindow(window);
        glfwTerminate();
        return;
    }

    setupStyle();

    if (openFile) {
        glfw_onDocumentOpen(openFile);
    }

    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        const uint64_t start = getTimeStamp();
        glfwPollEvents();

        int display_w, display_h, w, h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glfwGetWindowSize(window, &w, &h);
        if (display_w <= 0 || display_h <= 0) {
            sleepToMinFrameTime(start);
            continue;
        }

        if (!mainloopBackendNewFrame(window, clear_color, display_w, display_h, w, h)) {
            sleepToMinFrameTime(start);
            continue;
        }

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        io.DisplaySize = ImVec2((float)w, (float)h);
        io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

        mainwin.draw(w, h);

        ImGui::Render();
        mainloopBackendRender(window, ImGui::GetDrawData(), clear_color, display_w, display_h);

        sleepToMinFrameTime(start);
    }

    mainloopBackendShutdown(window);
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    g_win = NULL;
    glfwDestroyWindow(window);
    glfwTerminate();
}

extern "C" void mainLoop(const char* openFile) {
    XX_PROFILE_STATIC_INIT(NULL);
    _mainLoop(openFile);
    XX_PROFILE_STATIC_UNINIT();
}
