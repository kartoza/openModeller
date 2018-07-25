; defines added by tim to streamline / softcode install process
; Make sure the the following two vars are correct for your system
!define PRODUCT_VERSION "1.1.0"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "openModeller Desktop"
!define PRODUCT_PUBLISHER "openModeller.sf.net"
!define PRODUCT_WEB_SITE "http://openModeller.sf.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\openModellerDesktop.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

!define BUILD_DIR "C:\dev\omte\openModellerDesktop\bld"
!define INSTALL_DIR "$PROGRAMFILES\${PRODUCT_NAME} ${PRODUCT_VERSION}"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "openModellerDesktopSetup${PRODUCT_VERSION}.exe"
InstallDir "${INSTALL_DIR}"
# If this next lineis uncommented the installer will try to install to
# the same dir as any prefvious install of omdesktop
# With it commented it will try to used INSTALL_DIR as defined above
#InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
SetCompressor zlib
; Added by Tim for setting env vars (see this file on disk)
!include WriteEnvStr.nsh
; MUI 1.67 compatible ------
!include "MUI.nsh"
;Added by Tim for a macro that will recursively delete the files in the install dir
!include RecursiveDelete.nsh
;Added by Tim to support unzipping downloaded sample data automatically
!include ZipDLL.nsh

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "..\src\gui\openmodeller64x64.ico"
!define MUI_UNICON "..\src\gui\openmodeller64x64.ico"
; Added by Tim for side image
!define MUI_WELCOMEFINISHPAGE_BITMAP "om_logo.bmp"
; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "..\LICENSE.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "openModeller Desktop"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\openModellerDesktop.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "PortugueseBR" 
;!insertmacro MUI_LANGUAGE "French" 
;!insertmacro MUI_LANGUAGE "German" 
;!insertmacro MUI_LANGUAGE "SimpChinese" 
;!insertmacro MUI_LANGUAGE "Japanese" 
;!insertmacro MUI_LANGUAGE "Italian" 
;!insertmacro MUI_LANGUAGE "Swedish" 
;!insertmacro MUI_LANGUAGE "Russian" 
;!insertmacro MUI_LANGUAGE "Portuguese" 
;!insertmacro MUI_LANGUAGE "Polish" 
;!insertmacro MUI_LANGUAGE "Czech" 
;!insertmacro MUI_LANGUAGE "Slovak" 
;!insertmacro MUI_LANGUAGE "Latvian" 
;!insertmacro MUI_LANGUAGE "Indonesian" 

; Initialize language
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd


; MUI end ------



Section "Application" SEC01
  ;this section is mandatory
  SectionIn RO
  ;Added by Tim to install for all users not just the logged in user..
  ;make sure this is at the top of the section
  SetShellVarContext all
  
  
  SetOutPath "$INSTDIR\"
  File "${BUILD_DIR}\openModellerDesktop.exe"
  
  SetOutPath "$INSTDIR\lib\"
  File /r "${BUILD_DIR}\lib\*"
  
  SetOutPath "$INSTDIR\share"
  File /r "${BUILD_DIR}\share\*"
  
  SetOutPath "$INSTDIR\"
  File "${BUILD_DIR}\*.dll"
  
  SetOutPath "$INSTDIR\"
  File "${BUILD_DIR}\bin\*.dll"
;------- gdal related
  SetOutPath "$INSTDIR\data"
  File "${BUILD_DIR}\data\*"
  
  SetOutPath "$INSTDIR\nad"
  File "${BUILD_DIR}\nad\*"
  
  SetOutPath "$INSTDIR\algs"
  File "${BUILD_DIR}\algs\*.dll"
  
  SetOutPath "$INSTDIR\i18n"
  File "${BUILD_DIR}\i18n\*.qm"
  
  SetOutPath "$INSTDIR\plugins\modeller"
  File "${BUILD_DIR}\plugins\modeller\*.dll"
  
  SetOutPath "$INSTDIR\plugins\scraper"
  File "${BUILD_DIR}\plugins\scraper\*.dll"
; Shortcuts
; Next line is important - added by Tim
; if its not there the application working dir will be the last used
;outpath and libom wont be able to find its alg
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\openModeller Desktop v${PRODUCT_VERSION}.lnk" "$INSTDIR\openModellerDesktop.exe"
  CreateShortCut "$DESKTOP\openModeller Desktop ${PRODUCT_VERSION}.lnk" "$INSTDIR\openModellerDesktop.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - CRU CL2 Present" SEC03 
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/crucl2.zip crucl2.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\crucl2.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - based on Hadley 2050 A1F" SEC04 
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/hadley2050A1F.zip hadley2050A1F.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\hadley2050A1F.zip" "$INSTDIR\SampleData\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - South America" SEC05
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/installer_sample_data/SouthAmerica.zip SouthAmerica.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\SouthAmerica.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

; /o means unchecked by default
Section /o "Sample Data - Aquamaps" SEC06
 SetOutPath "$INSTDIR\SampleData\"
 NSISdl::download http://openmodeller.cria.org.br/download/marine2.zip marine2.zip
 !insertmacro ZIPDLL_EXTRACT "$INSTDIR\SampleData\marine2.zip" "$INSTDIR\SampleData\EnvironmentLayers\" "<ALL>"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  ;Added by Tim to install for all users not just the logged in user..
  SetShellVarContext all
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  ; its more consistant to let user remove the app from add/remove progs in control panel
  ;CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst-release.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\openModellerDesktop-uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\openModellerDesktop.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\openModellerDesktop-uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\openModellerDesktop.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Main application files - you really need this!"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Sample environment data. Global coverage derived from CRU CL2 present day scenario. About 41mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Sample environment data. Global coverage derived from Hadley 2050 A1f scenario. About 4mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Sample environment data. South America. About 1mb data will be downloaded from the internet."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} "Sample environment data. Aquamaps. About 1.4mb data will be downloaded from the internet."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall

  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\openModellerDesktop-uninstall.exe"
  Delete "$INSTDIR\*.dll"
;------------------- Sample Data
  Delete "$INSTDIR\SampleData\Points\SampleLegumes.txt"
;---------- Gdal Requirements
  Delete "$INSTDIR\data\projop_wparm.csv"
  Delete "$INSTDIR\data\prime_meridian.csv"
  Delete "$INSTDIR\data\pcs.csv"
  Delete "$INSTDIR\data\unit_of_measure.csv"
  Delete "$INSTDIR\data\stateplane.csv"
  Delete "$INSTDIR\data\seed_3d.dgn"
  Delete "$INSTDIR\data\s57objectclasses_iw.csv"
  Delete "$INSTDIR\data\s57objectclasses_aml.csv"
  Delete "$INSTDIR\data\s57objectclasses.csv"
  Delete "$INSTDIR\data\s57expectedinput.csv"
  Delete "$INSTDIR\data\s57attributes_iw.csv"
  Delete "$INSTDIR\data\s57attributes_aml.csv"
  Delete "$INSTDIR\data\s57attributes.csv"
  Delete "$INSTDIR\data\s57agencies.csv"
  Delete "$INSTDIR\data\gdal_datum.csv"
  Delete "$INSTDIR\data\ellipsoid.csv"
  Delete "$INSTDIR\data\gcs.csv"
  Delete "$INSTDIR\data\esri_extra.wkt"
  Delete "$INSTDIR\data\epsg.wkt"
  Delete "$INSTDIR\data\ecw_cs.dat"
  Delete "$INSTDIR\data\cubewerx_extra.wkt"
  Delete "$INSTDIR\nad\FL.lla"
  Delete "$INSTDIR\nad\MD.lla"
  Delete "$INSTDIR\nad\TN.lla"
  Delete "$INSTDIR\nad\WI.lla"
  Delete "$INSTDIR\nad\WO.lla"
  Delete "$INSTDIR\nad\alaska.lla"
  Delete "$INSTDIR\nad\conus.lla"
  Delete "$INSTDIR\nad\epsg"
  Delete "$INSTDIR\nad\esri"
  Delete "$INSTDIR\nad\hawaii.lla"
  Delete "$INSTDIR\nad\nad27"
  Delete "$INSTDIR\nad\nad83"
  Delete "$INSTDIR\nad\ntv1_can.dat"
  Delete "$INSTDIR\nad\prvi.lla"
  Delete "$INSTDIR\nad\stgeorge.lla"
  Delete "$INSTDIR\nad\stlrnc.lla"
  Delete "$INSTDIR\nad\stpaul.lla"
  Delete "$INSTDIR\nad\world"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\share"
;---------- openModeller Requirements
  Delete "$INSTDIR\pluginpath.cfg"
  Delete "$INSTDIR\libopenmodeller.a"
  Delete "$INSTDIR\algs\*.dll"
  Delete "$INSTDIR\data\aquamaps.db"
;---------------- translations

  Delete "$INSTDIR\*.qm"
;---------------- openModeller desktop related
  Delete "$INSTDIR\openModellerDesktop.exe"
  Delete "$INSTDIR\plugins\modeller\*"
  Delete "$INSTDIR\plugins\scraper\*"

  RMDir /r "$INSTDIR\SampleData\Points"
  RMDir /r "$INSTDIR\SampleData\EnvironmentLayers\"
  RMDir /r "$INSTDIR\SampleData\"
  RMDir /r "$INSTDIR\i18n"
  RMDir /r "$INSTDIR\algs"
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\nad"
  RMDir /r "$INSTDIR\plugins\modeller"
  RMDir /r "$INSTDIR\plugins\scraper"
  RMDir /r "$INSTDIR\plugins"
  RMDir /r "$INSTDIR"
;----------------- The application dir gets zapped next ...  
  ;I added this recursive delte implementation because
  ; RM -R wasnt working properly
  Push "$INSTDIR"
  !insertmacro RemoveFilesAndSubDirs "$INSTDIR\"
  RMDir "$INSTDIR"


;----------------- icons and shortcuts
  ;Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  ;Added by Tim to uninstall for all users not just the logged in user..
  SetShellVarContext all
  Delete "$DESKTOP\openModeller Desktop.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\openModeller Desktop.lnk"
  RMDir "$SMPROGRAMS\$ICONS_GROUP"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
