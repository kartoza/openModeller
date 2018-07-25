#!/bin/bash

#
# A script to profile omgui1
#

#
# Visit http://kcachegrind.sourceforge.net/cgi-bin/show.cgi/KcacheGrindUsage
#                          for more information
#

CURDIR=`pwd`
WORKDIR=/tmp/callgrindoutput$$
mkdir ${WORKDIR}
cd ${WORKDIR}
callgrind -v --dump-every-bb=10000000 openModellerDesktop
kcachegrind
cd ${CURDIR}
#rm -rf ${WORKDIR}


