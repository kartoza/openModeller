#!/bin/bash
#
# A script by Tim Sutton to automate running multiple QtTests
#   and produce a summary report of the results.
# (c) T. Sutton 2006
#
#set -x
export LD_LIBRARY_PATH=`pwd`/omgui1-release/lib:`pwd`/omgui1-release/bin/plugins/modeller/
export PATH=`pwd`/omgui1-release/bin:$PATH
PREFIX=`pwd`/omgui1-release/bin/tests
$1 -v2
