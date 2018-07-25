#!/bin/bash

# Check operating system
OS=`uname -s`

if [ $OS = FreeBSD ]; then
   QMAKE=qmake-qt4
   LUPDATE=lupdate-qt4
  else
   QMAKE=qmake
   LUPDATE=lupdate
fi

while (( $# > 0 )); do
  arg=$1
  shift

  if [ "$arg" = "-a" ]; then
    arg=$1
    shift
    if [ -f "i18n/openModellerDesktop_$arg.ts" ]; then
      echo "cannot add existing translation $arg"
      exit 1
    else
      add="$add $arg"
    fi
  fi
done

echo Creating qmake project file
$QMAKE -project -o omgui_ts.pro

if [ -n "$add" ]; then
  for i in $add; do
    echo "Adding translation for $i"
    echo "TRANSLATIONS += i18n/openModellerDesktop_$i.ts" >> omgui_ts.pro
  done
fi

echo Updating translation files
$LUPDATE -verbose omgui_ts.pro

if [ -n "$add" ]; then
  for i in $add; do
    if [ -f i18n/openModellerDesktop_$i.ts ]; then
      svn add i18n/openModellerDesktop_$i.ts
    else
      echo "Translation for $i was not added"
      exit 1
    fi
  done
fi

echo Removing qmake project file
rm omgui_ts.pro
