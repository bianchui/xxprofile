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

function ife() {
  which -s "$1"
  if [[ "$?" -eq 0 ]]; then
    guard "$@"
  fi
}

function build_apple_lib() {
  echo "==== Building apple lib ===="
  guard pushd $THIS_DIR/xxprofile/proj.apple
    guard ./build_ios.sh
  guard popd
}

function build_apple_viewer() {
  echo "==== Building apple viewer ===="
  guard pushd $THIS_DIR/xxprofile/proj.apple
    guard ./build_viewer.sh
  guard popd
}

function cmake_build_target() {
  local BUILD_DIR="$1"
  local TARGET="$2"
  local CONFIG="${3:-Release}"

  guard cmake --build "$BUILD_DIR" --config "$CONFIG" --target "$TARGET" --parallel
}

function build_cmake_lib_mac() {
  echo "==== Building mac lib with cmake ===="
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-lib-mac"
  local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/mac"

  guard mkdir -p "$BUILD_DIR"
  guard cmake \
    -S "$PROJ_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_SYSROOT=macosx \
    -DXXPROFILE_DYNAMIC=OFF \
    -DXXPROFILE_BUILD_LOADER=OFF \
    -DXXPROFILE_BUILD_TESTS=OFF \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
    -DXXPROFILE_BUILD_VIEWER=OFF

  cmake_build_target "$BUILD_DIR" xxprofile Release

  local LIB_PATH="$BUILD_DIR/libxxprofile.a"
  if [[ ! -f "$LIB_PATH" && -f "$BUILD_DIR/Release/libxxprofile.a" ]]; then
    LIB_PATH="$BUILD_DIR/Release/libxxprofile.a"
  fi
  if [[ ! -f "$LIB_PATH" ]]; then
    echo "libxxprofile.a not found in $BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$OUT_DIR"
  guard cp "$LIB_PATH" "$OUT_DIR/libxxprofile.a"
}

function build_cmake_lib_ios_sdk() {
  local SDK="$1"
  local ARCHS="$2"
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-lib-ios-$SDK"
  local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/ios/$SDK"
  local CACHE_DIR="$BUILD_DIR/darwin-cache"
  local MODULE_CACHE_DIR="$BUILD_DIR/ModuleCache.noindex"

  guard rm -rf "$BUILD_DIR"
  guard mkdir -p "$BUILD_DIR"
  guard mkdir -p "$CACHE_DIR"
  guard mkdir -p "$MODULE_CACHE_DIR"
  guard env \
    DARWIN_USER_CACHE_DIR="$CACHE_DIR/" \
    CLANG_MODULE_CACHE_PATH="$MODULE_CACHE_DIR" \
    cmake \
    -S "$PROJ_DIR" \
    -B "$BUILD_DIR" \
    -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT="$SDK" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" \
    -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
    -DCMAKE_XCODE_ATTRIBUTE_CLANG_MODULE_CACHE_PATH="$BUILD_DIR/ModuleCache.noindex" \
    -DXXPROFILE_DYNAMIC=OFF \
    -DXXPROFILE_BUILD_LOADER=OFF \
    -DXXPROFILE_BUILD_TESTS=OFF \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
    -DXXPROFILE_BUILD_VIEWER=OFF

  guard env \
    DARWIN_USER_CACHE_DIR="$CACHE_DIR/" \
    CLANG_MODULE_CACHE_PATH="$MODULE_CACHE_DIR" \
    cmake --build "$BUILD_DIR" --config Release --target xxprofile --parallel

  local LIB_PATH="$BUILD_DIR/Release-$SDK/libxxprofile.a"
  if [[ ! -f "$LIB_PATH" && -f "$BUILD_DIR/Release/libxxprofile.a" ]]; then
    LIB_PATH="$BUILD_DIR/Release/libxxprofile.a"
  fi
  if [[ ! -f "$LIB_PATH" ]]; then
    echo "libxxprofile.a not found for $SDK in $BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$OUT_DIR"
  guard cp "$LIB_PATH" "$OUT_DIR/libxxprofile.a"
}

function build_cmake_lib_ios() {
  echo "==== Building ios lib with cmake ===="
  build_cmake_lib_ios_sdk iphoneos arm64
  build_cmake_lib_ios_sdk iphonesimulator "arm64;x86_64"

  local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/ios"
  local XCFRAMEWORK="$OUT_DIR/libxxprofile.xcframework"
  guard rm -rf "$XCFRAMEWORK"
  guard xcodebuild -create-xcframework \
    -library "$OUT_DIR/iphoneos/libxxprofile.a" \
    -library "$OUT_DIR/iphonesimulator/libxxprofile.a" \
    -output "$XCFRAMEWORK"
}

function build_cmake_viewer() {
  echo "==== Building mac viewer with cmake ===="
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-viewer-mac"
  local OUT_DIR="$THIS_DIR/out"

  guard mkdir -p "$BUILD_DIR"
  guard cmake \
    -S "$PROJ_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_SYSROOT=macosx \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DXXPROFILE_BUILD_VIEWER=ON \
    -DXXPROFILE_BUILD_LOADER=ON \
    -DXXPROFILE_BUILD_TESTS=OFF \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF

  cmake_build_target "$BUILD_DIR" xxprofile_viewer Release

  local BIN_PATH="$BUILD_DIR/xxprofile_viewer"
  if [[ ! -f "$BIN_PATH" && -f "$BUILD_DIR/Release/xxprofile_viewer" ]]; then
    BIN_PATH="$BUILD_DIR/Release/xxprofile_viewer"
  fi
  if [[ ! -f "$BIN_PATH" ]]; then
    echo "xxprofile_viewer not found in $BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$OUT_DIR"
  guard cp "$BIN_PATH" "$OUT_DIR/xxprofile_viewer_cmake"
}

function build_imgui_sample_mac_metal() {
  echo "==== Building ImGui GLFW Metal sample with bundled GLFW 3.2.1 ===="
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local EXAMPLE_DIR="$THIS_DIR/libs/imgui/examples/example_glfw_metal"
  local TMP_DIR="$THIS_DIR/build/tmps/imgui_sample_mac_metal"
  local GLFW_BUILD_DIR="$TMP_DIR/glfw"
  local GLFW_LIB="$GLFW_BUILD_DIR/glfw/src/libglfw3.a"

  guard mkdir -p "$GLFW_BUILD_DIR"
  guard cmake \
    -S "$PROJ_DIR" \
    -B "$GLFW_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_SYSROOT=macosx \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DXXPROFILE_BUILD_VIEWER=ON \
    -DXXPROFILE_BUILD_LOADER=ON \
    -DXXPROFILE_BUILD_TESTS=OFF \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF

  cmake_build_target "$GLFW_BUILD_DIR" glfw Release

  if [[ ! -f "$GLFW_LIB" ]]; then
    echo "libglfw3.a not found in $GLFW_BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$TMP_DIR"
  guard make -C "$EXAMPLE_DIR" clean
  guard make -C "$TMP_DIR" -f "$EXAMPLE_DIR/Makefile" -B \
    EXE="$EXAMPLE_DIR/example_glfw_metal" \
    IMGUI_DIR="$THIS_DIR/libs/imgui" \
    VPATH="$EXAMPLE_DIR:$THIS_DIR/libs/imgui:$THIS_DIR/libs/imgui/backends" \
    CXXFLAGS="-std=c++11 -I$THIS_DIR/libs/imgui -I$THIS_DIR/libs/imgui/backends -I$THIS_DIR/libs/glfw-3.3.10/include -Wall -Wformat" \
    LIBS="-framework Metal -framework MetalKit -framework Cocoa -framework IOKit -framework CoreVideo -framework QuartzCore $GLFW_LIB"
}

function build_cmake_test_mac() {
  echo "==== Building and running mac test with cmake ===="
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-test-mac"
  local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/test_mac"

  guard mkdir -p "$BUILD_DIR"
  guard cmake \
    -S "$PROJ_DIR" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_SYSROOT=macosx \
    -DXXPROFILE_DYNAMIC=OFF \
    -DXXPROFILE_BUILD_LOADER=OFF \
    -DXXPROFILE_BUILD_TESTS=ON \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
    -DXXPROFILE_BUILD_VIEWER=OFF

  cmake_build_target "$BUILD_DIR" xxprofile_test Release

  local TEST_PATH="$BUILD_DIR/xxprofile_test"
  if [[ ! -f "$TEST_PATH" && -f "$BUILD_DIR/Release/xxprofile_test" ]]; then
    TEST_PATH="$BUILD_DIR/Release/xxprofile_test"
  fi
  if [[ ! -f "$TEST_PATH" ]]; then
    echo "xxprofile_test not found in $BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$OUT_DIR"
  guard cp "$TEST_PATH" "$OUT_DIR/xxprofile_test"
  guard pushd "$BUILD_DIR" > /dev/null
    guard "$TEST_PATH"
  guard popd > /dev/null
}

function build_cmake_test_ios_sdk() {
  local SDK="$1"
  local ARCHS="$2"
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-test-ios-$SDK"
  local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/test_ios/$SDK"
  local CACHE_DIR="$BUILD_DIR/darwin-cache"
  local MODULE_CACHE_DIR="$BUILD_DIR/ModuleCache.noindex"

  guard rm -rf "$BUILD_DIR"
  guard mkdir -p "$BUILD_DIR"
  guard mkdir -p "$CACHE_DIR"
  guard mkdir -p "$MODULE_CACHE_DIR"
  guard env \
    DARWIN_USER_CACHE_DIR="$CACHE_DIR/" \
    CLANG_MODULE_CACHE_PATH="$MODULE_CACHE_DIR" \
    cmake \
    -S "$PROJ_DIR" \
    -B "$BUILD_DIR" \
    -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_SYSROOT="$SDK" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHS" \
    -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
    -DCMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER=com.xxprofile.xxprofile-test \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO \
    -DCMAKE_XCODE_ATTRIBUTE_CLANG_MODULE_CACHE_PATH="$MODULE_CACHE_DIR" \
    -DXXPROFILE_DYNAMIC=OFF \
    -DXXPROFILE_BUILD_LOADER=OFF \
    -DXXPROFILE_BUILD_TESTS=ON \
    -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
    -DXXPROFILE_BUILD_VIEWER=OFF

  guard env \
    DARWIN_USER_CACHE_DIR="$CACHE_DIR/" \
    CLANG_MODULE_CACHE_PATH="$MODULE_CACHE_DIR" \
    cmake --build "$BUILD_DIR" --config Release --target xxprofile_test --parallel

  local TEST_PATH="$(find "$BUILD_DIR" -path '*Release-*' -type f -name 'xxprofile_test' | head -n 1)"
  if [[ -z "$TEST_PATH" || ! -f "$TEST_PATH" ]]; then
    echo "xxprofile_test not found for $SDK in $BUILD_DIR" >&2
    return 1
  fi

  guard mkdir -p "$OUT_DIR"
  guard cp "$TEST_PATH" "$OUT_DIR/xxprofile_test"
}

function build_cmake_test_ios() {
  echo "==== Building ios test with cmake ===="
  build_cmake_test_ios_sdk iphoneos arm64
  build_cmake_test_ios_sdk iphonesimulator "arm64;x86_64"
}

function build_cmake_test_android() {
  echo "==== Building android test with cmake ===="
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
    local BUILD_DIR="$THIS_DIR/xxprofile/build/cmake-test-android-$abi"
    local OUT_DIR="$THIS_DIR/out/prebuilt/cmake/test_android/$abi"
    guard mkdir -p "$BUILD_DIR"

    guard cmake \
      -S "$PROJ_DIR" \
      -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_WARN_DEPRECATED=OFF \
      -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
      -DANDROID_ABI="$abi" \
      -DANDROID_PLATFORM=android-21 \
      -DANDROID_STL=c++_static \
      -DXXPROFILE_DYNAMIC=OFF \
      -DXXPROFILE_BUILD_LOADER=OFF \
      -DXXPROFILE_BUILD_TESTS=ON \
      -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
      -DXXPROFILE_BUILD_VIEWER=OFF

    cmake_build_target "$BUILD_DIR" xxprofile_test Release

    local TEST_PATH="$BUILD_DIR/xxprofile_test"
    if [[ ! -f "$TEST_PATH" && -f "$BUILD_DIR/Release/xxprofile_test" ]]; then
      TEST_PATH="$BUILD_DIR/Release/xxprofile_test"
    fi
    if [[ ! -f "$TEST_PATH" ]]; then
      TEST_PATH="$(find "$BUILD_DIR" -type f -name 'xxprofile_test' | head -n 1)"
    fi
    if [[ -z "$TEST_PATH" || ! -f "$TEST_PATH" ]]; then
      echo "xxprofile_test not found for ABI $abi in $BUILD_DIR" >&2
      return 1
    fi

    guard mkdir -p "$OUT_DIR"
    guard cp "$TEST_PATH" "$OUT_DIR/xxprofile_test"
  done
}

function build_android_lib_ndk_build() {
  echo "==== Building android lib with ndk-build ===="
  guard pushd $THIS_DIR/xxprofile/proj.android
    guard ./build.sh
    guard cp ./prebuilt_Android.mk $THIS_DIR/out/prebuilt/android/Android.mk
  guard popd
}

function build_android_lib_cmake() {
  echo "==== Building android lib with cmake ===="
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
      -DXXPROFILE_BUILD_LOADER=OFF \
      -DXXPROFILE_BUILD_TESTS=OFF \
      -DXXPROFILE_BUILD_TEST_COMPRESS=OFF \
      -DXXPROFILE_BUILD_VIEWER=OFF \
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

function build_wasm_lib() {
  echo "==== Building wasm lib ===="
  local PROJ_DIR="$THIS_DIR/xxprofile/proj.cmake"
  local BUILD_DIR="$THIS_DIR/xxprofile/build/build-wasm"

  guard mkdir -p "$BUILD_DIR"
  guard pushd "$BUILD_DIR" > /dev/null

  if command -v emcmake >/dev/null 2>&1; then
    # Use emcmake to configure CMake for Emscripten toolchain
    guard emcmake cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DXXPROFILE_DYNAMIC=OFF \
      "$PROJ_DIR"
  else
    echo "emcmake not found in PATH, please activate emscripten environment." >&2
    popd > /dev/null
    return 1
  fi

  guard cmake --build . --config Release

  local LIB_PATH="libxxprofile.a"
  if [[ ! -f "$LIB_PATH" && -f "Release/libxxprofile.a" ]]; then
    LIB_PATH="Release/libxxprofile.a"
  fi

  if [[ ! -f "$LIB_PATH" ]]; then
    echo "libxxprofile.a (wasm) not found in $BUILD_DIR" >&2
    popd > /dev/null
    return 1
  fi

  local OUT_DIR="$THIS_DIR/out/prebuilt/wasm"
  guard mkdir -p "$OUT_DIR"
  guard cp "$LIB_PATH" "$OUT_DIR/libxxprofile.a"

  guard popd > /dev/null
}

function build_all() {
  build_apple_lib
  build_apple_viewer
  build_cmake_lib_mac
  build_cmake_lib_ios
  build_cmake_viewer
  build_android_lib_ndk_build
  build_android_lib_cmake
  build_wasm_lib
}

function copy_headers() {
  echo "==== Copying headers ===="
  guard mkdir -p $THIS_DIR/out/include
  guard cp $THIS_DIR/xxprofile/include/xxprofile/xxprofile.hpp $THIS_DIR/out/include
}

function zip_out() {
  echo "==== Zipping out files ===="
  guard pushd $THIS_DIR/out
    rm -f $THIS_DIR/xxprofile.zip
    guard zip -r $THIS_DIR/xxprofile.zip . -x **/.DS_Store
  guard popd
}

function cleanup_all() {
  echo "==== Cleaning up all ===="
  guard rm -rf $THIS_DIR/out/prebuilt
  guard rm -rf $THIS_DIR/out/include
  guard rm -rf $THIS_DIR/out/xxprofileViewer.app

  # android
  guard rm -rf $THIS_DIR/xxprofile/proj.android/libs/
  guard rm -rf $THIS_DIR/xxprofile/proj.android/obj/
  guard rm -rf $THIS_DIR/xxprofile/proj.android/test/libs/
  guard rm -rf $THIS_DIR/xxprofile/proj.android/test/obj/

  # apple
  guard rm -rf $THIS_DIR/xxprofile/proj.apple/build/

  # cmake
  guard rm -rf $THIS_DIR/xxprofile/build/
}

function usage() {
  echo "$0 commands"
  echo "commands:"
  echo "------------ seprate build commands ---------------"
  echo "  apple                 : build apple lib and viewer"
  echo "  android               : build android lib"
  echo "  wasm                  : build wasm lib"
  echo "  headers               : copy headers"
  echo "  zip                   : zip out files"
  echo "------------- cmake build commands ----------------"
  echo "  cmake_lib_mac         : build mac static lib with cmake"
  echo "  cmake_lib_ios         : build ios static xcframework with cmake"
  echo "  cmake_apple           : build cmake mac lib, ios lib, and mac viewer"
  echo "  cmake_viewer          : build mac viewer executable with cmake"
  echo "-------------- all in one commands ----------------"
  echo "  build                 : build all"
  echo "  clean                 : clean all"
  echo "----------- cmake build test commands -------------"
  echo "  cmake_test_mac        : build and run mac test with cmake"
  echo "  cmake_test_ios        : build ios test executable with cmake"
  echo "  cmake_test_android    : build android test executable with cmake"
  echo "---------- imgui sample build commands ------------"
  echo "  imgui_sample_mac_metal: build ImGui GLFW Metal sample with bundled GLFW 3.2.1"

}

function parse_arguments() {
  while [ "$1" != "" ]; do
    local PARAM=`echo $1 | awk -F= '{print $1}'`
    local VALUE=`echo $1 | awk -F= '{print $2}'`
    case $PARAM in
      apple)
        build_apple_lib
        build_apple_viewer
        ;;

      cmake_lib_mac)
        build_cmake_lib_mac
        ;;

      cmake_lib_ios)
        build_cmake_lib_ios
        ;;

      cmake_viewer)
        build_cmake_viewer
        ;;

      imgui_sample_mac_metal)
        build_imgui_sample_mac_metal
        ;;

      cmake_apple)
        build_cmake_lib_mac
        build_cmake_lib_ios
        build_cmake_viewer
        ;;

      cmake_test_mac)
        build_cmake_test_mac
        ;;

      cmake_test_ios)
        build_cmake_test_ios
        ;;

      cmake_test_android)
        build_cmake_test_android
        ;;

      test)
        build_cmake_test_mac
        build_cmake_test_ios
        build_cmake_test_android
        ;;

      android)
        build_android_lib_ndk_build
        build_android_lib_cmake
        ;;

      wasm)
        build_wasm_lib
        ;;

      headers)
        copy_headers
        ;;

      zip)
        zip_out
        ;;

      build)
        build_all
        copy_headers
        zip_out
        ;;

      clean)
        cleanup_all
        ;;

      *)
        echo "ERROR: unknown parameter \"$PARAM\""
        usage
        exit 1
        ;;
    esac
    shift
  done
}

if [ $# == 0 ]; then
  usage
  exit
fi

parse_arguments "$@"
