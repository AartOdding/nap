#!/usr/bin/env bash
nap_root=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )/../..

thirdparty="$nap_root/../thirdparty"
if [ ! -d $thirdparty ]; then
    echo "Error: The third party repository ('thirdparty') needs to be cloned alongside the main repository."
    echo
    echo "Once thirdparty is in place run check_build_environment.sh first."
    exit 1
fi

if [ "$(uname)" == "Darwin" ]; then
    python="$thirdparty/python/osx/install/bin/python3"
else
    python="$thirdparty/python/linux/install/bin/python3"
fi

unset PYTHONHOME
unset PYTHONPATH
$python $nap_root/build_tools/project_and_module_updater/project_and_module_updater.py "$@"
