// Copyright (C) 2018-2026, bianchui. All rights reserved.
#include "mainloop_backend.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include <stdio.h>

#if __has_feature(objc_arc)
#define XX_OBJC_RETAIN(obj) (obj)
#define XX_OBJC_RELEASE(obj) do { (void)(obj); } while (0)
#else
#define XX_OBJC_RETAIN(obj) [(obj) retain]
#define XX_OBJC_RELEASE(obj) [(obj) release]
#endif

static id<MTLDevice> g_device = nil;
static id<MTLCommandQueue> g_commandQueue = nil;
static CAMetalLayer* g_layer = nil;
static NSWindow* g_nswin = nil;
static MTLRenderPassDescriptor* g_renderPassDescriptor = nil;
static id<CAMetalDrawable> g_drawable = nil;
static id<MTLCommandBuffer> g_commandBuffer = nil;
static id<MTLRenderCommandEncoder> g_renderEncoder = nil;

static void mainloopMetalClearFrameObjects() {
    if (g_renderPassDescriptor) {
        g_renderPassDescriptor.colorAttachments[0].texture = nil;
    }
    XX_OBJC_RELEASE(g_renderEncoder);
    g_renderEncoder = nil;
    XX_OBJC_RELEASE(g_commandBuffer);
    g_commandBuffer = nil;
    XX_OBJC_RELEASE(g_drawable);
    g_drawable = nil;
}

void mainloopBackendApplyWindowHints() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

bool mainloopBackendInit(GLFWwindow* window) {
    @autoreleasepool {
        g_device = MTLCreateSystemDefaultDevice();
        if (!g_device) {
            fprintf(stderr, "Metal is not supported on this Mac.\n");
            return false;
        }

        g_commandQueue = [g_device newCommandQueue];
        if (!g_commandQueue) {
            fprintf(stderr, "Failed to create Metal command queue.\n");
            XX_OBJC_RELEASE(g_device);
            g_device = nil;
            return false;
        }

        g_nswin = glfwGetCocoaWindow(window);
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = g_device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.framebufferOnly = YES;
        layer.opaque = YES;
        g_nswin.contentView.layer = layer;
        g_nswin.contentView.wantsLayer = YES;
        g_layer = XX_OBJC_RETAIN(layer);

        g_renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];

        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_ImplMetal_Init(g_device);
        return true;
    }
}

bool mainloopBackendNewFrame(GLFWwindow* window, const ImVec4& clearColor, int display_w, int display_h, int window_w, int window_h) {
    (void)window;
    (void)window_w;
    (void)window_h;

    if (!g_layer || !g_commandQueue || !g_renderPassDescriptor || display_w <= 0 || display_h <= 0) {
        return false;
    }

    @autoreleasepool {
        mainloopMetalClearFrameObjects();

        g_layer.drawableSize = CGSizeMake(display_w, display_h);
        id<CAMetalDrawable> drawable = [g_layer nextDrawable];
        if (!drawable) {
            return false;
        }
        g_drawable = XX_OBJC_RETAIN(drawable);

        g_renderPassDescriptor.colorAttachments[0].clearColor =
            MTLClearColorMake(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
        g_renderPassDescriptor.colorAttachments[0].texture = g_drawable.texture;
        g_renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        g_renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

        id<MTLCommandBuffer> commandBuffer = [g_commandQueue commandBuffer];
        if (!commandBuffer) {
            mainloopMetalClearFrameObjects();
            return false;
        }
        g_commandBuffer = XX_OBJC_RETAIN(commandBuffer);

        id<MTLRenderCommandEncoder> renderEncoder = [g_commandBuffer renderCommandEncoderWithDescriptor:g_renderPassDescriptor];
        if (!renderEncoder) {
            mainloopMetalClearFrameObjects();
            return false;
        }
        g_renderEncoder = XX_OBJC_RETAIN(renderEncoder);

        ImGui_ImplMetal_NewFrame(g_renderPassDescriptor);
        return true;
    }
}

void mainloopBackendRender(GLFWwindow* window, ImDrawData* drawData, const ImVec4& clearColor, int display_w, int display_h) {
    (void)window;
    (void)clearColor;
    (void)display_w;
    (void)display_h;

    if (!g_commandBuffer || !g_renderEncoder || !g_drawable) {
        mainloopMetalClearFrameObjects();
        return;
    }

    @autoreleasepool {
        ImGui_ImplMetal_RenderDrawData(drawData, g_commandBuffer, g_renderEncoder);
        [g_renderEncoder endEncoding];
        [g_commandBuffer presentDrawable:g_drawable];
        [g_commandBuffer commit];
    }

    mainloopMetalClearFrameObjects();
}

void mainloopBackendShutdown(GLFWwindow* window) {
    (void)window;

    mainloopMetalClearFrameObjects();
    ImGui_ImplMetal_Shutdown();

    if (g_nswin) {
        g_nswin.contentView.layer = nil;
        g_nswin = nil;
    }

    XX_OBJC_RELEASE(g_renderPassDescriptor);
    g_renderPassDescriptor = nil;
    XX_OBJC_RELEASE(g_layer);
    g_layer = nil;
    XX_OBJC_RELEASE(g_commandQueue);
    g_commandQueue = nil;
    XX_OBJC_RELEASE(g_device);
    g_device = nil;
}

#undef XX_OBJC_RETAIN
#undef XX_OBJC_RELEASE
