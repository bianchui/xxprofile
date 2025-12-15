cmake_minimum_required(VERSION 3.10)

project(xxprofile LANGUAGES C CXX)

# 选项：是否开启解压功能（和源码里的 XXPROFILE_HAS_DECOMPRESS 对应）
option(XXPROFILE_HAS_DECOMPRESS "Enable decompression support in xxprofile" OFF)

# 选项：是否作为 DLL 导出符号（默认关闭，因为这里只构建静态库）
option(XXPROFILE_DLL_EXPORT "Build xxprofile with XX_DLL_EXPORT=1" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 和 Android 一样做一些裁剪：默认关闭 RTTI/异常，可按需修改
if(NOT MSVC)
    add_compile_options(
        -fno-rtti
        -fno-exceptions
        -fvisibility=hidden
        -fvisibility-inlines-hidden
        -fno-stack-protector
        -Os
    )
endif()

# 根目录
set(XXPROFILE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(XXPROFILE_SRC_DIR  ${XXPROFILE_ROOT_DIR}/src)
set(XXPROFILE_LIBS_DIR ${XXPROFILE_ROOT_DIR}/../libs)
set(XXPROFILE_REPO_ROOT ${XXPROFILE_ROOT_DIR}/..) # repo root, contains "libs/"

# 平台相关宏：让实现和 Android 版本一致/兼容
if(ANDROID)
    add_compile_definitions(XX_PLATFORM_ANDROID=1)
elseif(APPLE)
    add_compile_definitions(XX_PLATFORM_APPLE=1)
elseif(WIN32)
    add_compile_definitions(XX_PLATFORM_WIN=1)
else()
    add_compile_definitions(XX_PLATFORM_POSIX=1)
endif()

if(XXPROFILE_HAS_DECOMPRESS)
    add_compile_definitions(XXPROFILE_HAS_DECOMPRESS=1)
endif()

if(XXPROFILE_DLL_EXPORT)
    add_compile_definitions(XX_DLL_EXPORT=1)
endif()

# 源文件列表（核心逻辑 + 压缩实现 + 平台层）
set(XXPROFILE_SOURCES
    ${XXPROFILE_SRC_DIR}/xxprofile.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_name.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_tls.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_archive.cpp

    # 平台无关/基类
    ${XXPROFILE_SRC_DIR}/platforms/platform_base.cpp
)

# Android / Apple / 其他平台可按需过滤
if(ANDROID)
    set(XXPROFILE_SOURCES
        ${XXPROFILE_SOURCES}
        ${XXPROFILE_SRC_DIR}/platforms/platform_android.cpp
    )
endif()

if(APPLE)
    set(XXPROFILE_SOURCES
        ${XXPROFILE_SOURCES}
        ${XXPROFILE_SRC_DIR}/platforms/platform_apple.mm
    )
endif()

set(XXPROFILE_PRIVATE_HEADERS
    ${XXPROFILE_SRC_DIR}/xxprofile_internal.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_data.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_macros.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_name.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_namepool.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_tls.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_archive.hpp
    ${XXPROFILE_SRC_DIR}/xxprofile_version.hpp
    ${XXPROFILE_SRC_DIR}/platforms/platform.hpp
    ${XXPROFILE_SRC_DIR}/platforms/platform_android.hpp
    ${XXPROFILE_SRC_DIR}/platforms/platform_apple.hpp
    ${XXPROFILE_SRC_DIR}/platforms/platform_base.hpp
    ${XXPROFILE_SRC_DIR}/platforms/platform_win.hpp
    ${XXPROFILE_SRC_DIR}/platforms/posix/posix_timer.h
    ${XXPROFILE_SRC_DIR}/platforms/posix/pthread_SystemLock.h
    ${XXPROFILE_SRC_DIR}/platforms/posix/pthread_ThreadLocal.h
    ${XXPROFILE_SRC_DIR}/platforms/win/win_inc_windows.h
    ${XXPROFILE_SRC_DIR}/platforms/win/win_SystemLock.h
    ${XXPROFILE_SRC_DIR}/platforms/win/win_ThreadLocal.h
    ${XXPROFILE_SRC_DIR}/platforms/win/win_timer.h
)

# 压缩相关实现（这些 .h/.c 文件会被 xxprofile 源码 include）
set(XXPROFILE_COMPRESS_SOURCES
    ${XXPROFILE_SRC_DIR}/compress/compress_zstd.lib.c
    ${XXPROFILE_SRC_DIR}/compress/compress_lzma_dec.lib.c
    ${XXPROFILE_SRC_DIR}/compress/compress_lzma_enc.lib.c
)

add_library(xxprofile STATIC
    ${XXPROFILE_SOURCES}
    ${XXPROFILE_PRIVATE_HEADERS}
    ${XXPROFILE_COMPRESS_SOURCES}
)

target_include_directories(xxprofile
    PUBLIC
        ${XXPROFILE_SRC_DIR}
        ${XXPROFILE_ROOT_DIR}/include
    PRIVATE
        ${XXPROFILE_ROOT_DIR}
        ${XXPROFILE_REPO_ROOT}
        ${XXPROFILE_LIBS_DIR}
)

# Apple 需要链接 Foundation 等系统框架
if(APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    target_link_libraries(xxprofile PRIVATE ${FOUNDATION_FRAMEWORK})
endif()

# Android 下和 NDK 构建保持一致，链接 log 库
if(ANDROID)
    find_library(ANDROID_LOG_LIB log)
    if(ANDROID_LOG_LIB)
        target_link_libraries(xxprofile PRIVATE ${ANDROID_LOG_LIB})
    endif()
endif()

set_target_properties(xxprofile PROPERTIES
    OUTPUT_NAME "xxprofile"
)


