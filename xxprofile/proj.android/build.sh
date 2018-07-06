#!/bin/bash
ANDROID_SDK_ROOT=/Library/Java/android-sdk-macosx
ANDROID_NDK_ROOT=/Library/Java/android-ndk-r12b

PROJECT_PATH="$( cd "$(dirname "$0")" ; pwd -P )"

NDK_BUILD_COMMAND=${ANDROID_NDK_ROOT}/ndk-build

function guard() {
    $*
    local exit_code=$?
    if [[ "${exit_code}" -ne 0 ]]; then
        echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: **Error:${exit_code}** when executing command:" >&2
        echo "**:$*" >&2
        exit ${exit_code}
    fi
}

function build_native_libs() {
    echo "Begin of buidling Native libraries"
    local cpu_num=`sysctl -n hw.ncpu`
    local NDK_BUILD_OPTIONS="NDK_DEBUG=0 -j${cpu_num}"
    guard ${NDK_BUILD_COMMAND} ${NDK_BUILD_OPTIONS}
}

build_native_libs $1
