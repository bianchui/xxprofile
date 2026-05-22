cmake_minimum_required(VERSION 3.10)

project(xxprofile LANGUAGES C CXX)

if(APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

option(XXPROFILE_HAS_DECOMPRESS "Enable decompression support in xxprofile" OFF)
option(XXPROFILE_DYNAMIC "Build xxprofile as a shared library with XX_DLL_EXPORT=1" OFF)
option(XXPROFILE_BUILD_LOADER "Build xxprofile trace loader library" ON)
option(XXPROFILE_BUILD_TESTS "Build xxprofile runtime test executable" ON)
option(XXPROFILE_BUILD_TEST_COMPRESS "Build compression benchmark/test executable" OFF)
option(XXPROFILE_BUILD_VIEWER "Build desktop ImGui/GLFW viewer executable" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(XXPROFILE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(XXPROFILE_REPO_ROOT ${XXPROFILE_ROOT_DIR}/..)
set(XXPROFILE_SRC_DIR ${XXPROFILE_ROOT_DIR}/src)
set(XXPROFILE_LOADER_DIR ${XXPROFILE_ROOT_DIR}/loader)
set(XXPROFILE_VIEWER_DIR ${XXPROFILE_ROOT_DIR}/viewer)
set(XXPROFILE_LIBS_DIR ${XXPROFILE_REPO_ROOT}/libs)

function(xxprofile_apply_common_options target_name)
    target_include_directories(${target_name}
        PUBLIC
            ${XXPROFILE_SRC_DIR}
            ${XXPROFILE_ROOT_DIR}/include
        PRIVATE
            ${XXPROFILE_ROOT_DIR}
            ${XXPROFILE_REPO_ROOT}
            ${XXPROFILE_LIBS_DIR}
    )

    if(MSVC)
        target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        target_compile_options(${target_name} PRIVATE
            -fno-rtti
            -fno-exceptions
            -fvisibility=hidden
            -fvisibility-inlines-hidden
            -fno-stack-protector
            -Os
        )
    endif()
endfunction()

function(xxprofile_apply_platform target_name)
    if(WIN32)
        target_compile_definitions(${target_name} PUBLIC XX_PLATFORM_WINDOWS=1)
    elseif(ANDROID)
        target_compile_definitions(${target_name} PUBLIC XX_PLATFORM_ANDROID=1)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        target_compile_definitions(${target_name} PUBLIC XX_PLATFORM_EMSCRIPTEN=1)
    elseif(APPLE)
        target_compile_definitions(${target_name} PUBLIC XX_PLATFORM_APPLE=1)
    else()
        target_compile_definitions(${target_name} PUBLIC XX_PLATFORM_POSIX=1)
    endif()
endfunction()

set(XXPROFILE_SOURCES
    ${XXPROFILE_SRC_DIR}/xxprofile.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_name.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_tls.cpp
    ${XXPROFILE_SRC_DIR}/xxprofile_archive.cpp
    ${XXPROFILE_SRC_DIR}/platforms/platform_base.cpp
    ${XXPROFILE_SRC_DIR}/compress/compress_zstd.lib.c
)

if(ANDROID)
    list(APPEND XXPROFILE_SOURCES ${XXPROFILE_SRC_DIR}/platforms/platform_android.cpp)
endif()

if(APPLE)
    list(APPEND XXPROFILE_SOURCES ${XXPROFILE_SRC_DIR}/platforms/platform_apple.mm)
endif()

set(XXPROFILE_LIBRARY_TYPE STATIC)
if(XXPROFILE_DYNAMIC)
    set(XXPROFILE_LIBRARY_TYPE SHARED)
endif()

add_library(xxprofile ${XXPROFILE_LIBRARY_TYPE} ${XXPROFILE_SOURCES})
add_library(xxprofile::xxprofile ALIAS xxprofile)
xxprofile_apply_common_options(xxprofile)
xxprofile_apply_platform(xxprofile)
target_compile_definitions(xxprofile PRIVATE
    ZSTDLIB_VISIBLE=
    ZSTDERRORLIB_VISIBILITY=
    ZSTD_DISABLE_ASM
)

if(XXPROFILE_DYNAMIC)
    target_compile_definitions(xxprofile PUBLIC XX_DLL_EXPORT=1)
endif()

if(XXPROFILE_HAS_DECOMPRESS OR XXPROFILE_BUILD_LOADER OR XXPROFILE_BUILD_VIEWER OR XXPROFILE_BUILD_TEST_COMPRESS)
    if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        message(FATAL_ERROR "XXPROFILE_HAS_DECOMPRESS is not supported in emscripten")
    endif()
    target_compile_definitions(xxprofile PUBLIC XXPROFILE_HAS_DECOMPRESS=1)
endif()

if(APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    target_link_libraries(xxprofile PRIVATE ${FOUNDATION_FRAMEWORK})
endif()

if(ANDROID)
    find_library(ANDROID_LOG_LIB log)
    if(ANDROID_LOG_LIB)
        target_link_libraries(xxprofile PRIVATE ${ANDROID_LOG_LIB})
    endif()
endif()

set_target_properties(xxprofile PROPERTIES OUTPUT_NAME "xxprofile")

set(XXPROFILE_NEEDS_COMPRESSION_DEPS OFF)
if(XXPROFILE_BUILD_LOADER OR XXPROFILE_BUILD_TEST_COMPRESS OR XXPROFILE_BUILD_VIEWER)
    set(XXPROFILE_NEEDS_COMPRESSION_DEPS ON)
endif()

if(XXPROFILE_NEEDS_COMPRESSION_DEPS)
    set(XXPROFILE_COMPRESSION_DEP_SOURCES
        ${XXPROFILE_LIBS_DIR}/zlib/src/adler32.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/compress.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/crc32.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/deflate.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/infback.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/inffast.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/inflate.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/inftrees.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/trees.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/uncompr.c
        ${XXPROFILE_LIBS_DIR}/zlib/src/zutil.c
        ${XXPROFILE_LIBS_DIR}/lz4-1.9.4/lib/lz4.c
        ${XXPROFILE_LIBS_DIR}/lz4-1.9.4/lib/lz4hc.c
        ${XXPROFILE_SRC_DIR}/compress/compress_lzma_dec.lib.c
        ${XXPROFILE_SRC_DIR}/compress/compress_lzma_enc.lib.c
    )

    add_library(xxprofile_compression_deps STATIC ${XXPROFILE_COMPRESSION_DEP_SOURCES})
    target_include_directories(xxprofile_compression_deps
        PUBLIC
            ${XXPROFILE_LIBS_DIR}/zlib/src
            ${XXPROFILE_LIBS_DIR}/lzma
            ${XXPROFILE_LIBS_DIR}/lz4-1.9.4/lib
            ${XXPROFILE_LIBS_DIR}/lzo-2.10/include
    )
    target_compile_definitions(xxprofile_compression_deps PRIVATE
        ZLIB_CONST
        ZSTDLIB_VISIBLE=
        ZSTDERRORLIB_VISIBILITY=
        ZSTD_DISABLE_ASM
    )
    if(NOT MSVC)
        target_compile_options(xxprofile_compression_deps PRIVATE -fvisibility=hidden -Os)
    endif()
endif()

if(XXPROFILE_BUILD_LOADER)
    add_library(xxprofile_loader STATIC
        ${XXPROFILE_LOADER_DIR}/xxprofile_loader.cpp
        ${XXPROFILE_LOADER_DIR}/xxprofile_CppNameDecoder.cpp
    )
    add_library(xxprofile::loader ALIAS xxprofile_loader)
    xxprofile_apply_common_options(xxprofile_loader)
    xxprofile_apply_platform(xxprofile_loader)
    target_link_libraries(xxprofile_loader
        PUBLIC
            xxprofile
        PRIVATE
            xxprofile_compression_deps
    )
endif()

if(XXPROFILE_BUILD_TESTS)
    add_executable(xxprofile_test ${XXPROFILE_ROOT_DIR}/test/main.cpp)
    xxprofile_apply_common_options(xxprofile_test)
    xxprofile_apply_platform(xxprofile_test)
    target_link_libraries(xxprofile_test PRIVATE xxprofile)
    if(THREADS_PREFER_PTHREAD_FLAG OR NOT WIN32)
        find_package(Threads REQUIRED)
        target_link_libraries(xxprofile_test PRIVATE Threads::Threads)
    endif()
endif()

if(XXPROFILE_BUILD_TEST_COMPRESS)
    add_executable(xxprofile_test_compress
        ${XXPROFILE_ROOT_DIR}/test_compress/main.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_zlib.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_lzo.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_lz4.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_zstd.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_lzma.cpp
        ${XXPROFILE_ROOT_DIR}/test_compress/test_memcpy.cpp
    )
    xxprofile_apply_common_options(xxprofile_test_compress)
    xxprofile_apply_platform(xxprofile_test_compress)
    target_compile_definitions(xxprofile_test_compress PRIVATE XXPROFILE_HAS_DECOMPRESS=1)
    target_link_libraries(xxprofile_test_compress PRIVATE xxprofile xxprofile_compression_deps)
endif()

if(XXPROFILE_BUILD_VIEWER)
    if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        message(FATAL_ERROR "XXPROFILE_BUILD_VIEWER currently supports desktop platforms only")
    endif()
    if(NOT TARGET xxprofile_loader)
        message(FATAL_ERROR "XXPROFILE_BUILD_VIEWER requires XXPROFILE_BUILD_LOADER=ON")
    endif()

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    add_subdirectory(${XXPROFILE_LIBS_DIR}/glfw-3.3.10 ${CMAKE_BINARY_DIR}/glfw)

    set(XXPROFILE_IMGUI_BACKEND_SOURCES
        ${XXPROFILE_LIBS_DIR}/imgui/backends/imgui_impl_glfw.cpp
    )
    if(APPLE)
        list(APPEND XXPROFILE_IMGUI_BACKEND_SOURCES
            ${XXPROFILE_LIBS_DIR}/imgui/backends/imgui_impl_metal.mm
        )
    else()
        list(APPEND XXPROFILE_IMGUI_BACKEND_SOURCES
            ${XXPROFILE_LIBS_DIR}/imgui/backends/imgui_impl_opengl2.cpp
        )
    endif()

    set(XXPROFILE_VIEWER_BACKEND_SOURCE ${XXPROFILE_VIEWER_DIR}/mainloop_opengl2.cpp)
    if(APPLE)
        set(XXPROFILE_VIEWER_BACKEND_SOURCE ${XXPROFILE_VIEWER_DIR}/mainloop_metal.mm)
        set_source_files_properties(
            ${XXPROFILE_LIBS_DIR}/imgui/backends/imgui_impl_metal.mm
            ${XXPROFILE_VIEWER_DIR}/mainloop_metal.mm
            PROPERTIES COMPILE_FLAGS "-fobjc-arc"
        )
    endif()

    add_library(xxprofile_imgui STATIC
        ${XXPROFILE_LIBS_DIR}/imgui/imgui.cpp
        ${XXPROFILE_LIBS_DIR}/imgui/imgui_draw.cpp
        ${XXPROFILE_LIBS_DIR}/imgui/imgui_tables.cpp
        ${XXPROFILE_LIBS_DIR}/imgui/imgui_widgets.cpp
        ${XXPROFILE_IMGUI_BACKEND_SOURCES}
        ${XXPROFILE_VIEWER_DIR}/imgui/imgui_custom.cpp
    )
    target_include_directories(xxprofile_imgui
        PUBLIC
            ${XXPROFILE_LIBS_DIR}/imgui
            ${XXPROFILE_LIBS_DIR}/imgui/backends
            ${XXPROFILE_LIBS_DIR}/glfw-3.3.10/include
            ${XXPROFILE_VIEWER_DIR}/imgui
            ${XXPROFILE_REPO_ROOT}
    )
    target_compile_definitions(xxprofile_imgui PUBLIC
        IMGUI_USER_CONFIG=\"${XXPROFILE_VIEWER_DIR}/imgui/imgui_config.h\"
    )
    target_link_libraries(xxprofile_imgui PUBLIC glfw)
    if(NOT MSVC)
        target_compile_options(xxprofile_imgui PRIVATE -fvisibility=hidden -Os)
    endif()

    add_executable(xxprofile_viewer
        ${XXPROFILE_VIEWER_DIR}/main.c
        ${XXPROFILE_VIEWER_DIR}/mainloop.cpp
        ${XXPROFILE_VIEWER_BACKEND_SOURCE}
        ${XXPROFILE_VIEWER_DIR}/mainwin.cpp
        ${XXPROFILE_VIEWER_DIR}/view/FramesLineView.cpp
        ${XXPROFILE_VIEWER_DIR}/view/FrameView.cpp
        ${XXPROFILE_VIEWER_DIR}/view/TimeLineView.cpp
    )
    xxprofile_apply_common_options(xxprofile_viewer)
    xxprofile_apply_platform(xxprofile_viewer)
    target_compile_definitions(xxprofile_viewer PRIVATE
        XXPROFILE_HAS_DECOMPRESS=1
        IMGUI_USER_CONFIG=\"${XXPROFILE_VIEWER_DIR}/imgui/imgui_config.h\"
    )
    target_include_directories(xxprofile_viewer PRIVATE
        ${XXPROFILE_VIEWER_DIR}
        ${XXPROFILE_LIBS_DIR}/imgui
        ${XXPROFILE_LIBS_DIR}/imgui/backends
        ${XXPROFILE_LIBS_DIR}/glfw-3.3.10/include
    )
    target_link_libraries(xxprofile_viewer PRIVATE
        xxprofile_loader
        xxprofile_imgui
        xxprofile_compression_deps
    )

    if(APPLE)
        find_library(METAL_FRAMEWORK Metal REQUIRED)
        find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
        target_link_libraries(xxprofile_viewer PRIVATE
            ${METAL_FRAMEWORK}
            ${QUARTZCORE_FRAMEWORK}
        )
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(xxprofile_viewer PRIVATE OpenGL::GL)
    endif()

    if(WIN32)
        target_sources(xxprofile_viewer PRIVATE ${XXPROFILE_ROOT_DIR}/proj.win/xxprofileViewer/xxprofileViewer.rc)
        set_target_properties(xxprofile_viewer PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()
