#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
var_libname=xxprofile
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

function build_Config_Sdk() {
    local param_config=$1
    local param_sdk=$2

    local var_tmp_path=$THIS_DIR/build/tmps/ios_${param_config}/${param_sdk}
    local var_out_path=$THIS_DIR/build/libs/ios_${param_config}
    local var_PRODUCTS_DIR=${var_tmp_path}/Build/Products
    local var_TMP_DIR=${var_tmp_path}/Build/Intermediates.noindex
    echo building ${param_config}_${param_sdk} ...
    rm -f -R ${var_tmp_path}
    mkdir -p ${var_tmp_path}
    # -showBuildSettings>ios_${param_sdk}.txt
    guard xcodebuild -project ${var_project} -sdk ${param_sdk} -scheme ${var_scheme} -destination generic/platform=${param_sdk} -derivedDataPath ${var_tmp_path} -configuration ${param_config} BUILD_DIR=$var_PRODUCTS_DIR BUILD_ROOT=$var_PRODUCTS_DIR OBJROOT=$var_TMP_DIR -quiet

    mkdir -p ${var_out_path}
    guard strip -S -x $var_PRODUCTS_DIR/${param_config}-${param_sdk}/lib${var_scheme}.a -o ${var_out_path}/lib${var_libname}_${param_sdk}.a

    #guard cp -R -n ${var_tmp_path}/Build/Products/${param_config}-${param_sdk}/usr/local/ ${var_out_path}
}

function build_Config() {
    local param_config=$1

    local var_out_path=./build/libs/ios_${param_config}
    local var_out_lib=${var_out_path}/lib${var_libname}
    rm -f -R ${var_out_path}
    mkdir -p ${var_out_path}
    build_Config_Sdk ${param_config} iphoneos
    build_Config_Sdk ${param_config} iphonesimulator

    guard xcodebuild -create-xcframework -library ${var_out_lib}_iphoneos.a -library ${var_out_lib}_iphonesimulator.a -output ${var_out_lib}.xcframework

    guard rm -f ${var_out_lib}_iphoneos.a ${var_out_lib}_iphonesimulator.a

    mkdir -p ../../out/prebuilt/ios/
    guard cp -rf ${var_out_lib}.xcframework ../../out/prebuilt/ios/
}

pushd $THIS_DIR > /dev/null
build_Config Release
