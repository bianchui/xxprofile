cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Werror")
add_definitions("-DANDROID_NDK -DDISABLE_IMPORTGL")

add_library(sanangeles SHARED
    src/main/cpp/app-android.c
    src/main/cpp/demo.cpp
    src/main/cpp/importgl.c
)

target_include_directories(sanangeles PRIVATE
    src/main/cpp/
    ../../../src/
)

add_library(xxprofile SHARED IMPORTED)
set_target_properties(
    xxprofile
    PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../../libs/${ANDROID_ABI}/xxprofile.so
)

# Include libraries needed for sanangeles lib
target_link_libraries(
    sanangeles
    xxprofile
    android
    dl
    GLESv1_CM
    log
    m
    z
)
