ren lupdate src\gui\gui.pro
ren lupdate src\lib\omglib.pro
ren lupdate src\lib\modellerplugins\localplugin.pro
ren lupdate src\lib\modellerplugins\webservicesplugin.pro
ren lupdate src\widgets\widgets.pro

echo Creating qmake project file
qmake -project -o omgui_ts.pro
echo Updating translation files
lupdate -verbose omgui_ts.pro
lrelease -verbose omgui_ts.pro
echo Removing qmake project file
del omgui_ts.pro
copy i18n\*.qm omgui1-release\i18n\
