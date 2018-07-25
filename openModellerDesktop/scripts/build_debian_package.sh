#!/bin/bash
set -x
if [ -d `pwd`/src ]
then
  #src exists so we are prolly in the right dir
  echo "good we are in  omgui1 checkout dir!"
else
  echo "You must run this from the top level omgui1 checkout dir!"
  exit 1
fi

export DEBFULLNAME="Tim Sutton"
export DEBEMAIL=tim@linfiniti.com
dch -v 1.1.1+cvs`date +%Y%m%d`
fakeroot dpkg-buildpackage
