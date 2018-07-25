#!/bin/bash
export LD_LIBRARY_PATH=`pwd`/omgui1-release/lib:$HOME/apps/lib/
export PATH=`pwd`/omgui1-release/bin:$PATH
#ldd `pwd`/omgui1-release/bin/omgui1-release
#ldd /usr/local/lib/libopenmodeller.so
#make qt show extra debug info
#export QT_DEBUG_PLUGINS=1
#gdb omgui1-release core
openModellerDesktop
