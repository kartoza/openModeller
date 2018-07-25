#!/bin/bash
#set -x
if [ -d `pwd`/src ]
then
  #src exists so we are prolly in the right dir
  echo "Good we are in openModellerDesktop checkout dir!"
else
  echo "You must run this from the top level openModellerDesktop checkout dir!"
  exit 1
fi

if [ -d `pwd`/debian ]
then
  #src exists so we are prolly in the right dir
  echo "Good the debian directory exists!"
else
  echo "Debian directory is missing!"
  exit 1
fi

export DEBFULLNAME="Tim Sutton"
export DEBEMAIL=tim@linfiniti.com
cp install_manifest.txt debian/openmodellerdesktop.install
#dch -v 1.0.6+svn`date +%Y%m%d`
dch -v 1.0.7

fakeroot dpkg-buildpackage -kAA4D3BA997626237
