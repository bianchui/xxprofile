// Copyright (C) 2018-2026, bianchui. All rights reserved.
#pragma once

struct GLFWwindow;
struct ImDrawData;
struct ImVec4;

void mainloopBackendApplyWindowHints();
bool mainloopBackendInit(GLFWwindow* window);
bool mainloopBackendNewFrame(GLFWwindow* window, const ImVec4& clearColor, int display_w, int display_h, int window_w, int window_h);
void mainloopBackendRender(GLFWwindow* window, ImDrawData* drawData, const ImVec4& clearColor, int display_w, int display_h);
void mainloopBackendShutdown(GLFWwindow* window);
