LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := xxprofile_static
LOCAL_MODULE_FILENAME := xxprofile

LOCAL_SRC_FILES := xxprofile_main.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../src \


LOCAL_CFLAGS := -fvisibility=hidden -fno-stack-protector
LOCAL_CPPFLAGS += -std=c++11 -fno-rtti -fno-exceptions -fvisibility-inlines-hidden

include $(BUILD_STATIC_LIBRARY)
