Notes on the useage of the OMGUI custom designer plugins

                 Tim Sutton, 2006
--------------------------------------------------------

Introduction:

The purpose of the OMGUI designer plugins is to provide reuseable
QT4 based components that can be used with minimal programming.
The idea being that you use the standard Qt4 Designer GUI design tool 
to create your user interface and then add omgui widgets
from the toolbox of widgets in designer
- with OMGUI having added its own group of custom widgets there.
The OMGUI custom widgets can then be graphically 'programmed' by
setting widget properties and using interactive signal/slot 
connectors.

Plugin Paths:

There are two options for having Qt4 Designer find your
plugins at startup:

1) copy the plugin from {OMGUI Install Prefix}/lib/qgis/designer
   into the standard Qt4 designer plugin directory at 
   $QTDIR/plugins/designer/

2) export the environment variable QT_PLUGIN_PATH with all the
   places designer should look for your plugins in. Separate
   each entry with a colon. So for example:
   
   export QT_PLUGIN_PATH={OMGUI Install Prefix}/lib/plugins

   Note that the 'designer' directory is ommitted from the path.

   Also libomgwidgets.so.1 needs to be findable in your library search 
   path. Here is how I get the designer widgets to appear in designer on
   my system:

   export QT_PLUGIN_PATH=/home/timlinux/dev/cpp/omgui1/omgui1-release/lib/plugins
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/timlinux/dev/cpp/omgui1/omgui1-release/lib/
   designer

   Its probably a good idea to add the above export clause to 
   your ~/.bash_profile or ~/.bashrc if you plan to use the 
   designer plugins frequently.


Additional Notes:

If you built Qt4 in debug mode then the designer plugins must also 
be build in debug mode or they will be ignored. The converse is also
true.

NOTE NOTE NOTE Read the above paragraph again. If your plugin wont
work in designer, edit settings.pro and make sure the OMG_DEBUGMODE=false !!!

If you have set QT_PLUGIN_PATH in your bashrc and the plugin doesnt load when 
you start designer from the Kde menu (or Alt-f2 or gnome menu), try starting
designer from the command line.

You can use strace to check if your plugin dir is being searched when 
designer starts etc. e.g.:

strace designer >/tmp/strace.txt 2>&1
