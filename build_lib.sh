#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

pushd $THIS_DIR/xxprofile/proj.apple
  ./build_ios.sh
  ./build_mac.sh
popd

pushd $THIS_DIR/xxprofile/proj.android
  ./build.sh
  cp ./prebuilt_Android.mk $THIS_DIR/out/prebuilt/android/Android.mk
popd

mkdir -p $THIS_DIR/out/include
cp $THIS_DIR/xxprofile/include/xxprofile/xxprofile.hpp $THIS_DIR/out/include
