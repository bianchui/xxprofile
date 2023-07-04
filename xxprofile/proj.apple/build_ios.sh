#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
var_libname=xxprofile
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
    local param_type=$1
    local param_scheme=$2
    local param_config=$3
    local param_sdk=$4

    local var_tmp_path=$THIS_DIR/build/tmps/${param_type}_${param_config}/${param_sdk}
    local var_out_path=$THIS_DIR/build/libs/${param_type}_${param_config}
    local var_PRODUCTS_DIR=${var_tmp_path}/Build/Products
    local var_TMP_DIR=${var_tmp_path}/Build/Intermediates.noindex
    echo ==== building ${param_type} ${param_scheme} ${param_config}_${param_sdk} ...
    rm -f -R ${var_tmp_path}
    mkdir -p ${var_tmp_path}
    # -showBuildSettings>ios_${param_sdk}.txt
    guard xcodebuild \
        -project ${var_project} \
        -scheme ${param_scheme} \
        -destination generic/platform=${param_sdk} \
        -derivedDataPath ${var_tmp_path} \
        -configuration ${param_config} BUILD_DIR=$var_PRODUCTS_DIR BUILD_ROOT=$var_PRODUCTS_DIR OBJROOT=$var_TMP_DIR CONFIGURATION_BUILD_DIR=$var_PRODUCTS_DIR/${param_config}-${param_sdk} \
        -quiet

    mkdir -p ${var_out_path}/${param_sdk}
    if [ "${param_type}" == "static" ]; then
        guard strip -S -x $var_PRODUCTS_DIR/${param_config}-${param_sdk}/lib${param_scheme}.a -o ${var_out_path}/${param_sdk}/lib${var_libname}.a
    elif [ "${param_type}" == "framework" ]; then
        guard cp -R -n $var_PRODUCTS_DIR/${param_config}-${param_sdk}/${param_scheme}.framework ${var_out_path}/${param_sdk}/
    fi

    #guard cp -R -n ${var_tmp_path}/Build/Products/${param_config}-${param_sdk}/usr/local/ ${var_out_path}
}

function build_static_Config() {
    local param_config=$1

    local var_out_path=./build/libs/static_${param_config}
    local var_out_lib=${var_out_path}/lib${var_libname}
    rm -f -R ${var_out_path}
    mkdir -p ${var_out_path}
    build_Config_Sdk static xxprofile_ios ${param_config} iphoneos
    build_Config_Sdk static xxprofile_ios ${param_config} iphonesimulator
    build_Config_Sdk static xxprofile_mac ${param_config} macOS

    guard xcodebuild -create-xcframework \
        -library ${var_out_path}/iphoneos/lib${var_libname}.a \
        -library ${var_out_path}/iphonesimulator/lib${var_libname}.a \
        -library ${var_out_path}/macOS/lib${var_libname}.a \
        -output ${var_out_lib}.xcframework

    guard rm -rf ${var_out_path}/iphoneos
    guard rm -rf ${var_out_path}/iphonesimulator
    guard rm -rf ${var_out_path}/macOS

    mkdir -p ../../out/prebuilt/
    guard cp -rf ${var_out_lib}.xcframework ../../out/prebuilt/
}

function build_framework_Config() {
    local param_config=$1

    local var_out_path=./build/libs/framework_${param_config}
    local var_out_lib=${var_out_path}/${var_libname}
    rm -f -R ${var_out_path}
    mkdir -p ${var_out_path}

    build_Config_Sdk framework xxprofile ${param_config} iphoneos
    build_Config_Sdk framework xxprofile ${param_config} iphonesimulator
    build_Config_Sdk framework xxprofile ${param_config} macOS

    guard xcodebuild -create-xcframework \
        -framework ${var_out_path}/iphoneos/${var_libname}.framework \
        -framework ${var_out_path}/iphonesimulator/${var_libname}.framework \
        -framework ${var_out_path}/macOS/${var_libname}.framework \
        -output ${var_out_lib}.xcframework

    guard rm -f ${var_out_path}/iphoneos/${var_libname}.framework
    guard rm -f ${var_out_path}/iphonesimulator/${var_libname}.framework
    guard rm -f ${var_out_path}/macOS/${var_libname}.framework

    mkdir -p ../../out/prebuilt/
    guard cp -rf ${var_out_lib}.xcframework ../../out/prebuilt/
}

pushd $THIS_DIR > /dev/null
build_static_Config Release
build_framework_Config Release
