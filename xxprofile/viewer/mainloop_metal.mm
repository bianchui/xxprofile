// Copyright (C) 2018-2026, bianchui. All rights reserved.
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <shared/utils/StrBuf.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#define XX_ENABLE_PROFILE 0

#include "../src/xxprofile.hpp"

#include "mainwin.hpp"

#if __has_feature(objc_arc)
#define XX_OBJC_RELEASE(obj) do { (void)(obj); } while (0)
#else
#define XX_OBJC_RELEASE(obj) [(obj) release]
#endif

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

uint64_t getTimeStamp() {
    return (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

static void sleepToMinFrameTime(uint64_t start) {
    const uint64_t end = getTimeStamp();
    const uint64_t used = end - start;
    const uint64_t minTime = 33000;
    if (used >= minTime) {
        return;
    }

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
}

void setupStyle() {
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameRounding = 3.0f;
    style.WindowRounding = 0.0f;
}

MainWin mainwin;
GLFWwindow* g_win = NULL;
const char* kTitle = "xxprofileViewer";
std::string g_title = kTitle;

void glfw_setTitle(const char* title) {
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

int glfw_onDocumentOpen(const char* name) {
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

void _mainLoop(const char* openFile) {
    XX_PROFILE_SCOPE_FUNCTION();
    @autoreleasepool {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            return;
        }
        glfwSetOnDocumentOpen(glfw_onDocumentOpen);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(1280, 720, kTitle, NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }

        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device) {
            fprintf(stderr, "Metal is not supported on this Mac.\n");
            glfwDestroyWindow(window);
            glfwTerminate();
            return;
        }

        id<MTLCommandQueue> commandQueue = [device newCommandQueue];
        if (!commandQueue) {
            fprintf(stderr, "Failed to create Metal command queue.\n");
            XX_OBJC_RELEASE(device);
            glfwDestroyWindow(window);
            glfwTerminate();
            return;
        }

        g_win = window;
        glfwSetWindowTitle(g_win, g_title.c_str());
        glfwSetDropCallback(window, glfw_drop_callback);

        NSWindow* nswin = glfwGetCocoaWindow(window);
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.framebufferOnly = YES;
        layer.opaque = YES;
        nswin.contentView.layer = layer;
        nswin.contentView.wantsLayer = YES;

        MTLRenderPassDescriptor* renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];

        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_ImplMetal_Init(device);

        setupStyle();

        if (openFile) {
            glfw_onDocumentOpen(openFile);
        }

        const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            @autoreleasepool {
                const uint64_t start = getTimeStamp();
                // Poll and handle events (inputs, window resize, etc.)
                // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
                // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
                // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
                // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
                glfwPollEvents();

                int display_w, display_h, w, h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glfwGetWindowSize(window, &w, &h);
                if (display_w <= 0 || display_h <= 0) {
                    sleepToMinFrameTime(start);
                    continue;
                }

                layer.drawableSize = CGSizeMake(display_w, display_h);
                id<CAMetalDrawable> drawable = [layer nextDrawable];
                if (!drawable) {
                    sleepToMinFrameTime(start);
                    continue;
                }

                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

                id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
                id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

                // Start the ImGui frame
                ImGui_ImplMetal_NewFrame(renderPassDescriptor);
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                io.DisplaySize = ImVec2((float)w, (float)h);
                io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

                mainwin.draw(w, h);

                // Rendering
                ImGui::Render();
                ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

                [renderEncoder endEncoding];
                [commandBuffer presentDrawable:drawable];
                [commandBuffer commit];

                sleepToMinFrameTime(start);
            }
        }

        // Cleanup
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        nswin.contentView.layer = nil;
        XX_OBJC_RELEASE(renderPassDescriptor);
        XX_OBJC_RELEASE(commandQueue);
        XX_OBJC_RELEASE(device);

        g_win = NULL;
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

extern "C" void mainLoop(const char* openFile) {
    XX_PROFILE_STATIC_INIT(NULL);
    _mainLoop(openFile);
    XX_PROFILE_STATIC_UNINIT();
}

#undef XX_OBJC_RELEASE
