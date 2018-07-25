#!/bin/bash
##
## Script to automate install of OMGUI1 designer plugins
##
##          Tim Sutton 2006
##

export QTDIR=/usr

sudo cp lib/plugins/designer/*.so ${QTDIR}/lib/qt4/plugins/designer/
