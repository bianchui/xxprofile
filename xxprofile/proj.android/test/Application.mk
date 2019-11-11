#NDK_TOOLCHAIN_VERSION := 4.9

APP_ABI := arm64-v8a
APP_PLATFORM=android-24
# Get C++11 working
APP_CPPFLAGS += -std=c++11 -fexceptions
#APP_STL := gnustl_static
#APP_STL := stlport_static
APP_STL := c++_static
