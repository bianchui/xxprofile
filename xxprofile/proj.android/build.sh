#!/bin/bash
readonly ANDROID_SDK_ROOT=$HOME/Library/Android/sdk
readonly ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk/20.0.5594570
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

function get_build_var_for_abi() {
    # current dir is a vaild ndk-build dir
    # with:
    #   jni/Android.mk
    #   jni/Application.mk
    NDK_PROJECT_PATH=.  make --no-print-dir -f $ANDROID_NDK_ROOT/build/core/build-local.mk DUMP_$1 APP_ABI=$2
}

function get_tool_for_abi() {
    local TOOLCHAIN_PREFIX=`get_build_var_for_abi TOOLCHAIN_PREFIX $1`
    which ${TOOLCHAIN_PREFIX}$2
}

function cp_abis_a() {
    while [ "$1" != "" ]; do
        guard mkdir -p ../../out/prebuilt/android/$1
        guard cp obj/local/$1/xxprofile.a ../../out/prebuilt/android/$1/
        local STRIP=`get_tool_for_abi $1 strip`
        $STRIP -d ../../out/prebuilt/android/$1/xxprofile.a
        shift
    done
}

function cp_abis_so() {
    while [ "$1" != "" ]; do
        guard mkdir -p ../../out/prebuilt/android/$1
        guard cp libs/$1/xxprofile.so ../../out/prebuilt/android/$1/
        local STRIP=`get_tool_for_abi $1 strip`
        $STRIP -s -x ../../out/prebuilt/android/$1/xxprofile.so
        shift
    done
}

function build_native_libs() {
    guard pushd $THIS_DIR > /dev/null
    echo "Begin of building xxprofile libraries"
    local cpu_num=`sysctl -n hw.ncpu`
    local NDK_BUILD_OPTIONS="NDK_DEBUG=0 -j${cpu_num}"
    guard ${NDK_BUILD_COMMAND} ${NDK_BUILD_OPTIONS} NDK_PROJECT_PATH=. NDK_APPLICATION_MK=jni/Application.mk APP_BUILD_SCRIPT=jni/Android.mk
    cp_abis_a arm64-v8a armeabi-v7a x86 x86_64
    guard ${NDK_BUILD_COMMAND} ${NDK_BUILD_OPTIONS} NDK_PROJECT_PATH=. NDK_APPLICATION_MK=jni/Application.mk APP_BUILD_SCRIPT=jni/Android_so.mk
    cp_abis_so arm64-v8a armeabi-v7a x86 x86_64
    local NM=`get_tool_for_abi arm64-v8a nm`
    $NM -D -C ../../out/prebuilt/android/arm64-v8a/xxprofile.so | grep " T "
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
