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

guard pushd $THIS_DIR/xxprofile/proj.apple
  guard ./build_ios.sh
  guard ./build_mac.sh
  guard ./build_viewer.sh
guard popd

guard pushd $THIS_DIR/xxprofile/proj.android
  guard ./build.sh
  guard cp ./prebuilt_Android.mk $THIS_DIR/out/prebuilt/android/Android.mk
guard popd

guard mkdir -p $THIS_DIR/out/include
guard cp $THIS_DIR/xxprofile/include/xxprofile/xxprofile.hpp $THIS_DIR/out/include

guard pushd $THIS_DIR/out
  rm -f $THIS_DIR/xxprofile.zip
  guard zip -r $THIS_DIR/xxprofile.zip . -x **/.DS_Store
guard popd
