#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"
readonly REPO_DIR="$( cd "$THIS_DIR/../.." ; pwd -P )"
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
    local var_archive_path=${var_tmp_path}/${var_productname}.xcarchive
    local var_archive_app=${var_archive_path}/Products/Applications/${var_productname}.app
    echo archiving ${param_config}_${param_sdk} ...
    guard rm -f -R ${var_tmp_path}
    guard mkdir -p ${var_tmp_path}
    #xcodebuild -list -project ${var_project} 
    guard xcodebuild \
        -project ${var_project} \
        -scheme ${var_scheme} \
        -destination generic/platform=${param_sdk} \
        -derivedDataPath ${var_tmp_path} \
        -archivePath ${var_archive_path} \
        -configuration ${param_config} \
        ENABLE_ADDRESS_SANITIZER=NO \
        archive \
        -quiet

    if [[ ! -d "${var_archive_app}" ]]; then
        echo "${var_productname}.app not found in archive: ${var_archive_path}" >&2
        return 1
    fi

    guard mkdir -p ${var_out_path}
    guard rm -f -R ${var_out_path}/${var_productname}.app
    guard cp -Rf ${var_archive_app} ${var_out_path}/
}

function build_Config() {
    local param_config=$1

    local var_out_path=${THIS_DIR}/build/libs/viewer_${param_config}
    local var_out_lib=${var_out_path}/${var_productname}
    guard rm -f -R ${var_out_path}
    guard mkdir -p ${var_out_path}
    build_Config_Sdk ${param_config} macOS

    guard pushd ${REPO_DIR} > /dev/null
        guard mkdir -p out
        guard rm -f -R out/${var_productname}.app
        guard cp -Rf ${var_out_lib}.app out/
    guard popd > /dev/null
}

pushd $THIS_DIR > /dev/null
build_Config Release
