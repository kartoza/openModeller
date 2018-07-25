#!/bin/bash
export LD_LIBRARY_PATH=`pwd`/omgui1-debug/lib:$HOME/apps/lib
export PATH=`pwd`/omgui1-debug/bin:$PATH
#make qt show extra debug info
export QT_DEBUG_PLUGINS=1
omgui1-debug
#gdb omgui1-debug

