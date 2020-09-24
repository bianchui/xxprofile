APP_STL := c++_static
APP_CPPFLAGS := -fno-rtti
APP_CPPFLAGS += -fno-exceptions
APP_CFLAGS += -fvisibility=hidden
APP_CFLAGS += -fno-stack-protector
APP_CFLAGS += -Os
APP_CPPFLAGS += -fvisibility-inlines-hidden
APP_CPPFLAGS += -D_GLIBCXX_HAVE_ATTRIBUTE_VISIBILITY=0
APP_PLATFORM := android-19
NDK_TOOLCHAIN_VERSION := clang
APP_ABI := armeabi armeabi-v7a x86 arm64-v8a x86_64
