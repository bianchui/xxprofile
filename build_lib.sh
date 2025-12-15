#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

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

function build_apple_lib() {
  guard pushd $THIS_DIR/xxprofile/proj.apple
    guard ./build_ios.sh
  guard popd
}

function build_apple_viewer() {
  guard pushd $THIS_DIR/xxprofile/proj.apple
    guard ./build_viewer.sh
  guard popd
}

function build_android_lib_ndk_build() {
  guard pushd $THIS_DIR/xxprofile/proj.android
    guard ./build.sh
    guard cp ./prebuilt_Android.mk $THIS_DIR/out/prebuilt/android/Android.mk
  guard popd
}

function build_android_lib_cmake() {
  local ANDROID_SDK_ROOT_DEFAULT="$HOME/Library/Android/sdk"
  local ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$ANDROID_SDK_ROOT_DEFAULT}"
  local ANDROID_NDK_ROOT_DEFAULT="$ANDROID_SDK_ROOT/ndk/20.0.5594570"
  local ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$ANDROID_NDK_ROOT_DEFAULT}"

  local TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake"
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"

  if [[ ! -f "$TOOLCHAIN_FILE" ]]; then
    echo "Android CMake toolchain file not found: $TOOLCHAIN_FILE" >&2
    return 1
  fi

  local ABIS=("arm64-v8a" "armeabi-v7a" "x86" "x86_64")

  for abi in "${ABIS[@]}"; do
    local BUILD_DIR="$THIS_DIR/xxprofile/build/build-android-$abi"
    guard mkdir -p "$BUILD_DIR"
    guard pushd "$BUILD_DIR" > /dev/null

    guard cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_WARN_DEPRECATED=OFF \
      -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
      -DANDROID_ABI="$abi" \
      -DANDROID_PLATFORM=android-21 \
      -DANDROID_STL=c++_static \
      -DXXPROFILE_DYNAMIC=ON \
      "$PROJ_DIR"

    guard cmake --build . --config Release

    local LIB_PATH="libxxprofile.so"
    if [[ ! -f "$LIB_PATH" && -f "Release/libxxprofile.so" ]]; then
      LIB_PATH="Release/libxxprofile.so"
    fi

    if [[ ! -f "$LIB_PATH" ]]; then
      echo "libxxprofile.so not found for ABI $abi in $BUILD_DIR" >&2
      popd > /dev/null
      return 1
    fi

    local OUT_DIR="$THIS_DIR/out/prebuilt/android_cmake/$abi"
    guard mkdir -p "$OUT_DIR"
    guard cp "$LIB_PATH" "$OUT_DIR/xxprofile.so"

    # strip symbols to reduce .so size, similar to NDK build
    local STRIP_BIN=""
    if [[ -x "$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-strip" ]]; then
      STRIP_BIN="$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-strip"
    elif command -v llvm-strip >/dev/null 2>&1; then
      STRIP_BIN="$(command -v llvm-strip)"
    fi
    if [[ -n "$STRIP_BIN" ]]; then
      "$STRIP_BIN" -s -x "$OUT_DIR/xxprofile.so" || true
    fi

    guard popd > /dev/null
  done
}

build_apple_lib
build_apple_viewer
build_android_lib_ndk_build
build_android_lib_cmake

guard mkdir -p $THIS_DIR/out/include
guard cp $THIS_DIR/xxprofile/include/xxprofile/xxprofile.hpp $THIS_DIR/out/include

guard pushd $THIS_DIR/out
  rm -f $THIS_DIR/xxprofile.zip
  guard zip -r $THIS_DIR/xxprofile.zip . -x **/.DS_Store
guard popd
