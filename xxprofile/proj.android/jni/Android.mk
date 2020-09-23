LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := xxprofile_static
LOCAL_MODULE_FILENAME := xxprofile

LOCAL_SRC_FILES := xxprofile_main.cpp

#LOCAL_SRC_FILES += \
#    ../../../libs/lz4-1.9.2/lib/lz4.c \
#    ../../../libs/lz4-1.9.2/lib/lz4hc.c \


LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../src \
    $(LOCAL_PATH)/../../.. \

LOCAL_CFLAGS += -fvisibility=hidden -fno-stack-protector
LOCAL_CFLAGS += -DXX_PLATFORM_ANDROID=1
LOCAL_CPPFLAGS += -std=c++11 -fno-rtti -fno-exceptions -fno-stack-protector
LOCAL_CPPFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -O2

LOCAL_EXPORT_LDLIBS += -llog

include $(BUILD_STATIC_LIBRARY)
