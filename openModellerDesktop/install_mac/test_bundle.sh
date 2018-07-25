#!/bin/bash

# A simple test to see if the mac bundle is behaving
# It contains hard coded paths for now...

sudo mv /Library/Frameworks /Library/Frameworks_
sudo mv /usr/local/lib /usr/local/lib_
~/apps/openModellerDesktop.app/Contents/MacOS/openModellerDesktop
sudo mv /Library/Frameworks_ /Library/Frameworks
sudo mv /usr/local/lib_ /usr/local/lib

