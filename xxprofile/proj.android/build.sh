#!/bin/bash
readonly ANDROID_SDK_ROOT=/Library/Java/android-sdk-macosx
readonly ANDROID_NDK_ROOT=/Library/Java/android-ndk-r12b
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

readonly NDK_BUILD_COMMAND=${ANDROID_NDK_ROOT}/ndk-build

function guard() {
    "$@"
    local exit_code=$?
    if [[ "${exit_code}" -ne 0 ]]; then
        local func="${FUNCNAME[1]}"
        [ x$func = x ] && func=MAIN
        local linen="${BASH_LINENO[0]}"
        local src="$THIS_DIR/$(basename "${BASH_SOURCE[0]}")"
        echo "[$(date +'%Y-%m-%dT%H:%M:%S%z')]: **Error:${exit_code}** when executing command:" >&2
        echo "**:$@" >&2
        echo "**   pwd: $PWD" >&2
        echo "**   src: $src($linen)" >&2
        echo "**   fun: $func" >&2
        exit ${exit_code}
    fi
}

function build_native_libs() {
    guard pushd $THIS_DIR > /dev/null
    echo "Begin of building Native libraries"
    local cpu_num=`sysctl -n hw.ncpu`
    local NDK_BUILD_OPTIONS="NDK_DEBUG=0 -j${cpu_num}"
    guard ${NDK_BUILD_COMMAND} ${NDK_BUILD_OPTIONS}
    popd > /dev/null
}

function build_test_app() {
    guard pushd $THIS_DIR/test > /dev/null
    echo "Begin of building test app"
    local cpu_num=`sysctl -n hw.ncpu`
    local NDK_BUILD_OPTIONS="NDK_DEBUG=0 -j${cpu_num}"
    guard ${NDK_BUILD_COMMAND} ${NDK_BUILD_OPTIONS} NDK_PROJECT_PATH=. NDK_APPLICATION_MK=Application.mk APP_BUILD_SCRIPT=Android.mk
    popd > /dev/null
}

build_native_libs $1
build_test_app $1
