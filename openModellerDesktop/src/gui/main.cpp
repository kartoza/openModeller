/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "omgmainwindow.h"
#include "omgtermsandconditions.h"
#include "omgui.h"
#include <stdio.h>
#include <stdlib.h>
//qt includes
#include <QApplication>
#include <QBitmap>
#include <QDir>
#include <QPixmap>
#include <QPlastiqueStyle>
#include <QSettings>
#include <QString>
#include <QStyle>
#include <QSettings>
#include <QTextCodec>
#include <QLocale>
#include <QTranslator>
#include <QImageReader>

#include <cpl_conv.h> // for setting gdal options


#ifdef WITH_QGIS
//
// QGIS Includes
//
#include <qgsapplication.h>
#endif

#if STATIC_BUILD
 #include <QtCore/QtPlugin>
 Q_IMPORT_PLUGIN(qjpeg)
 Q_IMPORT_PLUGIN(qsvg)
#endif

#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif
//std includes

int main(int argc, char *argv[])
{
#ifdef WITH_QGIS
  QgsApplication myApp(argc,argv,true);

#else
  QApplication  myApp(argc,argv);
#endif

  //NOTE: make sure these lines stay after myApp init above
  QCoreApplication::setOrganizationName("openModeller");
  QCoreApplication::setOrganizationDomain("openmodeller.sf.net");
  QCoreApplication::setApplicationName("OpenModellerGui");

  // For non static builds on mac and win
  // we need to be sure we can find the qt image
  // plugins. In mac be sure to look in the
  // application bundle...
#ifdef Q_WS_WIN
  QApplication::addLibraryPath( QApplication::applicationDirPath() 
      + QDir::separator() + "plugins" );
#endif
#ifdef Q_OS_MACX
  qDebug("Adding qt image plugins to plugin search path...");
  CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
  const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
  CFRelease(myBundleRef);
  CFRelease(myMacPath);
  QString myPath(mypPathPtr);
  // if we are not in a bundle assume that the app is built
  // as a non bundle app and that image plugins will be
  // in system Qt frameworks. If the app is a bundle
  // lets try to set the qt plugin search path...
  if (myPath.contains(".app"))
  {
    QString myPluginPath = myPath + "/Contents/plugins";
    QApplication::addLibraryPath( myPluginPath );
    qDebug( "Added %s to plugin search path", qPrintable( myPluginPath ) );
    //
    // Set the search path for Gdal CSV data files
    //
    QString myGdalDataPath = myPath + "/Contents/MacOS/share";
    CPLSetConfigOption("GDAL_DATA", myGdalDataPath.toLocal8Bit()); 
    // verify ...
    if( CPLGetConfigOption("GDAL_DATA",NULL) != NULL )
    {
      myGdalDataPath = QString(CPLGetConfigOption("GDAL_DATA",NULL));
      qDebug("GDAL_DATA set to :" + myGdalDataPath.toLocal8Bit());
    }
    else
    {
      qDebug("Warning GDAL_DATA is not set and thus projections");
      qDebug("may not work properly...");
    }
  }
#endif



  //
  // QGis plugin paths
  //
#ifdef Q_WS_WIN
  myApp.setPluginPath(myApp.applicationDirPath() + QDir::separator() + 
      "lib" + QDir::separator() + "qgis");
  myApp.setPkgDataPath(myApp.applicationDirPath() + QDir::separator() + 
      "share" + QDir::separator() + "qgis");
  //qDebug("plugin path:" + myApp.pluginPath().toLocal8Bit());
  //qDebug("srs path:" + myApp.srsDbFilePath().toLocal8Bit());
   
    //
    // Ensure gdal looks for its data in the correct place
    //
    QString myGdalDataPath = myApp.applicationDirPath() + 
      QDir::separator() + "data";
    CPLSetConfigOption("GDAL_DATA", myGdalDataPath.toLocal8Bit()); 
    // verify ...
    if( CPLGetConfigOption("GDAL_DATA",NULL) != NULL )
    {
      myGdalDataPath = QString(CPLGetConfigOption("GDAL_DATA",NULL));
      qDebug("GDAL_DATA set to :" + myGdalDataPath.toLocal8Bit());
    }
    else
    {
      qDebug("Warning GDAL_DATA is not set and thus projections");
      qDebug("may not work properly...");
    }
#endif

  QImageReader::supportedImageFormats(); // will check for plugins
#ifdef Q_WS_WIN
  //for windows lets use plastique syle!
  QApplication::setStyle(new QPlastiqueStyle);
#endif
  //make the library search path include the application dir on windows
  //this is so the plugins can find the dlls they are linked to at run time
  QApplication::addLibraryPath(QApplication::applicationDirPath());

  //
  // Set up the stylesheet
  // gradieted colour for header views
  //QString myStyle = "QHeaderView::section { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565); color: white; padding-left: 4px; border: 1px solid #6c6c6c; }";
  //myApp.setStyleSheet(myStyle);

  //
  // Set up translations
  //
  QSettings mySettings;
  QString mySystemLocale = QLocale::languageToString(QLocale::system().language());
  QString myUserLocale = mySettings.value("locale/userLocale", "").toString();
  bool myLocaleOverrideFlag = mySettings.value("locale/overrideFlag",false).toBool();
  QString myI18nPath = Omgui::i18nPath();
  QString myLocale;
  if (!myLocaleOverrideFlag || myUserLocale.isEmpty())
  {
    myLocale = QTextCodec::locale();
  }
  else
  {
    myLocale = myUserLocale;
  }
  //qDebug( "Setting translation to "
  //+ myI18nPath.toLocal8Bit()  + "/openModellerDesktop_" +
  //myLocale.toLocal8Bit());

  /* Translation file for Qt.
   * The strings from the QMenuBar context section are used by Qt/Mac to shift
   * the About, Preferences and Quit items to the Mac Application menu.
   * These items must be translated identically in both qt_ and qgis_ files.
   */
  QTranslator myQtTranslator(0);
  if (myQtTranslator.load(QString("qt_") + myLocale, myI18nPath))
  {
    myApp.installTranslator(&myQtTranslator);
    //qDebug("openModellerDesktop_" + myLocale.toLocal8Bit() + " locale loaded for Qt");
  }

  /* Translation file for openModeller Desktop.
  */
  QTranslator myOmgTranslator(0);
  if (myOmgTranslator.load(QString("openModellerDesktop_") + myLocale, myI18nPath))
  {
    myApp.installTranslator(&myOmgTranslator);
    //qDebug("openModellerDesktop_" + myLocale.toLocal8Bit() + " locale loaded for openModeller");
  }

  //
  // Now set up the main window and lauch the app
  //
  OmgMainWindow * mypOmg = new OmgMainWindow();
  if (mySettings.value("licensing/termsAgreedFlag",false).toBool()!=true)
  {
    OmgTermsAndConditions myTerms;
    if (myTerms.exec()!=QDialog::Accepted)
    {
      return false;
    }
  }
  mypOmg->show();
  // note if the widget does not inherit qdialog
  // (as in the case of our main window)
  // you must call app exec!
  return myApp.exec();

}

