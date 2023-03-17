#!/bin/bash
readonly THIS_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

sudo xattr -rd com.apple.quarantine $THIS_DIR/xxprofileViewer.app
rm -f $THIS_DIR/first_run.command
