LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../src/ \
    $(LOCAL_PATH)/../../../ \

LOCAL_SRC_FILES := \
    ../../test/main.cpp \
    ../../../shared/platforms/android/Platform_android.cpp \

LOCAL_CFLAGS += -fPIE -funwind-tables
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_STATIC_LIBRARIES += xxprofile_static

LOCAL_MODULE := xxprofile_test

include $(BUILD_EXECUTABLE)

$(call import-add-path, $(LOCAL_PATH)/..)
$(call import-module, jni)
