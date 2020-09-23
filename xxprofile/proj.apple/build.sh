#!/bin/bash
var_libname=xxprofile_ios
var_scheme=xxprofile_ios
var_project=xxprofile.xcodeproj

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

function abspath() {
    python -c "import os.path; print os.path.abspath('$1')"
}

function move2trash() {
    local param_path="$( abspath $1 )"
    if [ -e ${param_path} ]; then
        osascript -e 'tell application "Finder" to move the POSIX file "'${param_path}'" to trash'
    fi
}

function build_Config_Sdk_Arch() {
    local param_config=$1
    local param_arch=$2
    local param_sdk=$3

    local var_tmp_path=./build/tmps/ios_${param_config}/${param_arch}
    local var_out_path=./build/libs/ios_${param_config}
    echo building ${param_config}_${param_arch} ...
    rm -f -R ${var_tmp_path}
    mkdir -p ${var_tmp_path}
    guard xcodebuild -project ${var_project} -sdk ${param_sdk} -arch ${param_arch} -scheme ${var_scheme} -derivedDataPath ${var_tmp_path} -configuration ${param_config} -quiet

    mkdir -p ${var_out_path}
    guard strip -S -x ${var_tmp_path}/Build/Products/${param_config}-${param_sdk}/lib${var_libname}.a -o ${var_out_path}/lib${var_libname}_${param_arch}.a

    #guard cp -R -n ${var_tmp_path}/Build/Products/${param_config}-${param_sdk}/usr/local/ ${var_out_path}
}

function build_Config() {
    local param_config=$1

    local var_out_path=./build/libs/ios_${param_config}
    local var_out_lib=${var_out_path}/lib${var_libname}
    rm -f -R ${var_out_path}
    mkdir -p ${var_out_path}
    build_Config_Sdk_Arch ${param_config} arm64 iphoneos
    build_Config_Sdk_Arch ${param_config} armv7 iphoneos
    build_Config_Sdk_Arch ${param_config} x86_64 iphonesimulator
    build_Config_Sdk_Arch ${param_config} i386 iphonesimulator

    guard lipo -output ${var_out_lib}.a -create ${var_out_lib}_arm64.a ${var_out_lib}_armv7.a ${var_out_lib}_x86_64.a ${var_out_lib}_i386.a

    guard rm -f ${var_out_lib}_arm64.a ${var_out_lib}_armv7.a ${var_out_lib}_x86_64.a ${var_out_lib}_i386.a
}

#move2trash ../src/egret-renderer-2d-wasm
build_Config Release
