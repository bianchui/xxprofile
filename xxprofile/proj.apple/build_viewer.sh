#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
var_productname=xxprofileViewer
var_scheme=xxprofileViewer
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

    local var_tmp_path=$THIS_DIR/build/tmps/viewer_${param_config}/${param_sdk}
    local var_out_path=$THIS_DIR/build/libs/viewer_${param_config}
    local var_PRODUCTS_DIR=${var_tmp_path}/Build/Products
    local var_TMP_DIR=${var_tmp_path}/Build/Intermediates.noindex
    echo building ${param_config}_${param_sdk} ...
    rm -f -R ${var_tmp_path}
    mkdir -p ${var_tmp_path}
    #xcodebuild -list -project ${var_project} 
    guard xcodebuild -project ${var_project} -scheme ${var_scheme} -destination generic/platform=${param_sdk} -derivedDataPath ${var_tmp_path} -configuration ${param_config} BUILD_DIR=$var_PRODUCTS_DIR BUILD_ROOT=$var_PRODUCTS_DIR OBJROOT=$var_TMP_DIR -quiet

    mkdir -p ${var_out_path}
    guard cp -Rf $var_PRODUCTS_DIR/${param_config}/${var_productname}.app ${var_out_path}/
    guard strip -S -x ${var_out_path}/${var_productname}.app/Contents/MacOS/${var_productname}
    guard codesign -f -s - --deep ${var_out_path}/${var_productname}.app
}

function build_Config() {
    local param_config=$1

    local var_out_path=./build/libs/viewer_${param_config}
    local var_out_lib=${var_out_path}/${var_productname}
    rm -f -R ${var_out_path}
    mkdir -p ${var_out_path}
    build_Config_Sdk ${param_config} macOS

    mkdir -p ../../out/
    guard cp -Rf ${var_out_lib}.app ../../out/
}

pushd $THIS_DIR > /dev/null
build_Config Release
