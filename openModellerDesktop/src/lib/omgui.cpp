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
#include "omgui.h"
#include "omggdal.h"
#include "omgmodel.h"
#include "omgmodellerplugininterface.h"
#include "omgscraperpluginregistry.h"
#include "omglocality.h"
#include "omglayerset.h"
#include <omgconfig.h> //software version no e.g. 1.0.6
#include <omgsvnversion.h>  //svn revision no e.g. 3002M

#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDirIterator>
#include <QFile>
#include <QMap>
#include <QPluginLoader>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>
#include <QColor>
#include <QUrl>
#include <QPainter>
#include <QRadialGradient>
#include <QDebug>
//
// OSX includes
// Important:  keep this include before qgis includes
// otherwise you will get a name conflix on Point
//
#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

//
// Qgis includes
//
#include <qgis.h> //defines GEOWKT
#include <qgsapplication.h> //search path for srs.db
#include <qgscoordinatereferencesystem.h> //needed for creating a srs
#include <qgsfeature.h> //we will need to pass a bunch of these for each rec
#include <qgsfield.h>
#include <qgsgeometry.h> //each feature needs a geometry
#include <qgsmapcanvas.h>
#include <qgsmaplayerregistry.h>
#include <qgspoint.h> //we will use point geometry
#include <qgsproject.h>
#include <qgsproviderregistry.h>
#include <qgsrasterlayer.h>
#include <qgssinglesymbolrenderer.h>
#include <qgssymbol.h>
#include <qgsuniquevaluerenderer.h>
#include <qgsvectordataprovider.h>
#include <qgsvectorfilewriter.h> //logic for writing shpfiles
#include <qgsvectorlayer.h> //defines QgsFieldMap 



/* 
 * Returns the path to the settings directory in user's home dir
 */
const QString Omgui::userSettingsDirPath()
{
  QSettings mySettings;
  QString myPath=
      mySettings.value("dataDirs/dataDir", QDir::homePath() + QDir::separator() + QString(".omgui")).toString();
  //  Make sure the users settings dir actually exists
  //qDebug("Omgui::userSettingsDirPath() = " + myPath.toLocal8Bit());
  QDir().mkpath(myPath);
  return myPath;
}
const QString Omgui::getModelOutputDir()
{
  QString myPath = userSettingsDirPath()+QDir::separator()+"modelOutputs"+QDir::separator();
  QDir().mkpath(myPath);
  return myPath;
}
const QString Omgui::userAlgorithmProfilesDirPath()
{
  //alg profiles are always saved in the users home dir under .omgui/
  QString myPath = QDir::homePath() + QDir::separator() + QString(".omgui") + 
    QDir::separator()+"algorithmProfiles"+QDir::separator();
  QDir().mkpath(myPath);
  return myPath;
}
const QString Omgui::fileSelectorCachePath()
{
  //cached fileSelector documents are always saved in the users home dir under .omgui/
  //and then in subdirectories based on the modeller adapter type
  QSettings mySettings;
  QString mySubDir = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  if (mySubDir=="Web Services Modeller Plugin")
  {
    QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
    QUrl myUrl(myWSUrl);
    mySubDir += QDir::separator() + myUrl.host();
  }
  mySubDir=mySubDir.replace(" ","");
  QString myPath = QDir::homePath() + QDir::separator() + QString(".omgui") + 
                   QDir::separator() +
                   "fileSelectorCache" +
                   QDir::separator() +
                   mySubDir +
                   QDir::separator();
  QDir().mkpath(myPath);
  return myPath;
}
const QString Omgui::userLayersetDirPath()
{
  //layersets  are always saved in the users home dir under .omgui/
  //and then in subdirectories based on the modeller adapter type
  QSettings mySettings;
  QString mySubDir = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  if (mySubDir=="Web Services Modeller Plugin")
  {
    QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
    QUrl myUrl(myWSUrl);
    mySubDir += QDir::separator() + myUrl.host();
  }
  mySubDir=mySubDir.replace(" ","");
  QString myPath = QDir::homePath() + QDir::separator() + QString(".omgui") + 
                   QDir::separator() +
                   "layersets" +
                   QDir::separator() +
                   mySubDir +
                   QDir::separator();
  QDir().mkpath(myPath);
  return myPath;
}


const QString Omgui::pluginDirPath()
{
#if defined(Q_OS_WIN)
  QDir myPluginsDir = QDir(qApp->applicationDirPath());
  myPluginsDir.cd("plugins");
  return myPluginsDir.absolutePath();
#elif defined(Q_OS_MAC)
  CFURLRef myPluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef myMacPath = CFURLCopyFileSystemPath(myPluginRef, kCFURLPOSIXPathStyle);
  const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
  CFRelease(myPluginRef);
  CFRelease(myMacPath);
  QString myPath(mypPathPtr);
  //do some magick so unit tests work - if the application is
  //not running in a bundle use a relative path to the build tree
  if (myPath.contains(".app"))
  {
    myPath += "/Contents/MacOS/lib/openModellerDesktop/";
    return myPath;
  }
  else //must be running from unit tests
  {
    QDir myPluginsDir = QDir(qApp->applicationDirPath());
    myPluginsDir.cd("lib");
    myPluginsDir.cd("plugins");
    myPath = myPluginsDir.absolutePath();
    qDebug("Running in unit test mode so Omgui::pluginDirPath() is using:");
    qDebug(myPath);
    return myPath;
  }
#else //linux
  if (!qApp->applicationDirPath().isEmpty())
  {
    QDir myPluginsDir = QDir(qApp->applicationDirPath());
    myPluginsDir.cd("..");
    myPluginsDir.cd("lib");
    myPluginsDir.cd("openModellerDesktop");
    return myPluginsDir.absolutePath();
  }
  else
  {
    //maybe we are using Wt??
    return "/var/www/wt/plugins/";
  }
#endif
}





Omgui::LayerSetMap Omgui::getAvailableLayerSets()
{
  Omgui::LayerSetMap myMap;
  QDir myDirectory(userLayersetDirPath());
  myDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks );
  QFileInfoList myList = myDirectory.entryInfoList();
  for (unsigned int i = 0; i < static_cast<unsigned int>(myList.size()); ++i) 
  {
    QFileInfo myFileInfo = myList.at(i);
    //Ignore directories
    if(myFileInfo.fileName() == "." ||myFileInfo.fileName() == ".." ) 
    {
      continue;
    }
    //if the filename ends in .xml try to load it into our layerSets listing
    if(myFileInfo.completeSuffix()=="xml")
    {
      //qDebug("Loading layerset: " + myList.at(i).absoluteFilePath().toLocal8Bit());
      OmgLayerSet myLayerSet;
      myLayerSet.fromXmlFile(myFileInfo.absoluteFilePath());
      if (myLayerSet.name().isEmpty())
      {
        continue;
      }
      myMap[myLayerSet.name()]=myLayerSet;
    }
  }
  return myMap;
}

QStringList Omgui::sortList(QStringList theList)
{
    //sort the taxon list alpabetically descending order
    theList.sort(); //this sorts ascending!
    //flip the sort order
    QStringList mySortedList;
    QStringList::Iterator myIterator= theList.end();
    while( myIterator!=theList.begin() )
    {
      myIterator--;
      mySortedList << *myIterator;
    }
    return mySortedList;
}

QStringList Omgui::uniqueList(QStringList theList)
{
    //remove any duplicates from a sorted list
    QStringList myUniqueList;
    QString myLast = "";
    QStringListIterator myIterator( theList );
    while( myIterator.hasNext() )
    {
      QString myCurrent = myIterator.next();
      if (myCurrent!=myLast)
      {
        myUniqueList << myCurrent;
      }
      myLast=myCurrent;
    }
    return myUniqueList;
}


QString Omgui::getLayers(QString theBaseDir)
{
  // @TODO check theBaseDir exists
  
  QDomDocument myDocument("OmguiLayersTree");
  QDomElement myRoot = myDocument.createElement("AvailableLayers");
  myRoot.setAttribute("xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation","http://openmodeller.cria.org.br/xml/1.0 http://openmodeller.cria.org.br/xml/1.0/openModeller.xsd");
  myDocument.appendChild(myRoot);
  //append the start dir as a layerset
  QDomElement myElement = myDocument.createElement("LayersGroup");
  myElement.setAttribute("Id",theBaseDir);
  myElement.setAttribute("Type","DIR");
  //set the label
  QDomElement myLabelElement = myDocument.createElement("Label");
  QDomText myLabelText = myDocument.createTextNode(theBaseDir);
  myLabelElement.appendChild(myLabelText);
  myElement.appendChild(myLabelElement);
  //add to parent
  myRoot.appendChild(myElement);
  //now proceed to traverse all dirs below the base dir
  traverseDirectories(theBaseDir,myDocument,myElement);
  QString myString = myDocument.toString();
  return myString;
}

void Omgui::traverseDirectories(const QString theDirName, QDomDocument &theDocument, QDomElement &theParentElement)
{
  if (theDirName.isEmpty()) return;
  //std::cout << "Recursing into : " << theDirName << std::endl;
  QDir myDirectory(theDirName);
  myDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks );
  //std::cout << "Current directory is: " << theDirName.toAscii() << std::endl;
  QString myInvalidFileList;
  QString myInvalidFileProjList;
  QFileInfoList myList = myDirectory.entryInfoList();
  for (unsigned int i = 0; i < static_cast<unsigned int>(myList.size()); ++i) 
  {
    QFileInfo myFileInfo = myList.at(i);
    //qDebug("Traverse Directories Scanning : " + myFileInfo.fileName().toLocal8Bit());
    if(myFileInfo.fileName() == "." ||myFileInfo.fileName() == ".." ) 
    {
      continue;
    }
    //if this is an empty dir, just skip it...
    if (myFileInfo.absoluteDir().count() < 1)
    {
      continue;
    }
    //check to see if entry is a directory - if so iterate through it (recursive function)
    //a new tree node will be created each time
    bool myFileFlag=false;
    bool myProjFlag=false;
    myFileFlag=OmgGdal::isValidGdalFile(myFileInfo.absoluteFilePath());
    /*
    if (myFileFlag)
    {
      qDebug(myFileInfo.absoluteFilePath().toLocal8Bit() + " is a valid gdal file");
    }
    else
    {
      qDebug(myFileInfo.absoluteFilePath().toLocal8Bit() + " is NOT a valid gdal file");
    }
    */
    if (myFileFlag)
    {
      myProjFlag=OmgGdal::isValidGdalProj(myFileInfo.absoluteFilePath());
    }
    if(myFileInfo.isDir() && myFileInfo.isReadable() && !myFileFlag )
    {
      QDomElement myElement = theDocument.createElement("LayersGroup");
      myElement.setAttribute("Id",myFileInfo.absoluteFilePath());
      myElement.setAttribute("Type","DIR");
      //set the label
      QDomElement myLabelElement = theDocument.createElement("Label");
      QDomText myLabelText = theDocument.createTextNode(myFileInfo.baseName());
      myLabelElement.appendChild(myLabelText);
      myElement.appendChild(myLabelElement);
      //add to parent
      theParentElement.appendChild(myElement);
      traverseDirectories(myFileInfo.absoluteFilePath(),theDocument, myElement );
    }
    ///if its a directory AND a valid gdal layer then its AIG and we wont traverse it
    else if (myFileInfo.isDir() && myFileInfo.isReadable() && myFileFlag)
    {
      if ( myFileFlag && myProjFlag )
      {
        //GOOD FILE AND GOOD PROJ
        //std::cout <<myFileInfo->absoluteFilePath().ascii() << " is a valid GDAL file and contains projection info" << std::endl;
        QDomElement myElement = theDocument.createElement("Layer");
        myElement.setAttribute("Id",myFileInfo.absoluteFilePath());
        myElement.setAttribute("HasProjection","1");
        myElement.setAttribute("Type","AIG");
        //set the label
        QDomElement myLabelElement = theDocument.createElement("Label");
        QDomText myLabelText = theDocument.createTextNode(myFileInfo.baseName());
        myLabelElement.appendChild(myLabelText);
        myElement.appendChild(myLabelElement);
        //add to parent
        theParentElement.appendChild(myElement);
      }
      else if (myFileFlag && !myProjFlag)
      {
        //GOOD FILE AND BAD PROJ
        //std::cout <<myFileInfo->absoluteFilePath().ascii() << " is a valid GDAL file but contains no projection info" << std::endl;
        QDomElement myElement = theDocument.createElement("Layer");
        myElement.setAttribute("Id",myFileInfo.absoluteFilePath());
        myElement.setAttribute("HasProjection","0");
        myElement.setAttribute("Type","AIG");
        //set the label
        QDomElement myLabelElement = theDocument.createElement("Label");
        QDomText myLabelText = theDocument.createTextNode(myFileInfo.baseName());
        myLabelElement.appendChild(myLabelText);
        myElement.appendChild(myLabelElement);
        //add to parent
        theParentElement.appendChild(myElement);
      }
    }
    //check to see if entry is of the other required file types
    else if ((myFileInfo.suffix()=="tif") ||
            (myFileInfo.suffix()=="asc") ||
            (myFileInfo.suffix()=="bil") ||
            (myFileInfo.suffix()=="img") ||
            (myFileInfo.suffix()=="jpg")   )
    {      
      
      //test whether the file is GDAL compatible
      if ( myFileFlag && myProjFlag )
      {
        //GOOD FILE AND GOOD PROJ
        //std::cout <<myFileInfo->absoluteFilePath().ascii() << " is a valid GDAL file and contains projection info" << std::endl;
        QDomElement myElement = theDocument.createElement("Layer");
        myElement.setAttribute("Id",myFileInfo.absoluteFilePath());
        myElement.setAttribute("HasProjection","1");
        myElement.setAttribute("Type",myFileInfo.suffix());
        //set the label
        QDomElement myLabelElement = theDocument.createElement("Label");
        QDomText myLabelText = theDocument.createTextNode(myFileInfo.fileName());
        myLabelElement.appendChild(myLabelText);
        myElement.appendChild(myLabelElement);
        //add to parent
        theParentElement.appendChild(myElement);
      }
      else if (myFileFlag && !myProjFlag)
      {
        //GOOD FILE AND BAD PROJ
        //std::cout <<myFileInfo->absoluteFilePath().ascii() << " is a valid GDAL file but contains no projection info" << std::endl;
        QDomElement myElement = theDocument.createElement("Layer");
        myElement.setAttribute("Id",myFileInfo.absoluteFilePath());
        myElement.setAttribute("HasProjection","0");
        myElement.setAttribute("Type",myFileInfo.suffix());
        //set the label
        QDomElement myLabelElement = theDocument.createElement("Label");
        QDomText myLabelText = theDocument.createTextNode(myFileInfo.fileName());
        myLabelElement.appendChild(myLabelText);
        myElement.appendChild(myLabelElement);
        //add to parent
        theParentElement.appendChild(myElement);
      }
      else 
      {
        //BAD FILE AND/OR BAD PROJ
        //do nothing
      } 
    }  
  }
}   

//return a string list of all the experiment files
QStringList Omgui::getExperimentsList()
{
  QStringList myExperimentList;
  QSettings mySettings;
  QString myWorkDir = mySettings.value("dataDirs/dataDir",
      QDir::homePath() + QDir::separator() + ".omgui").toString() + 
                     QDir::separator() + "modelOutputs";
  QDir myDirectory(myWorkDir);
  if (!myDirectory.exists())
  {
    qDebug("Error the experiment working directory does not exist");
    qDebug("Directory we tried to use is : " +  myWorkDir.toAscii());
    return QStringList();
  }
  myDirectory.setFilter( QDir::Dirs | QDir::NoDotAndDotDot );

  QStringList myList = myDirectory.entryList();
  for (unsigned int i = 0; i < static_cast<unsigned int>(myList.size()); ++i) 
  {
    QString mySubDirectoryName = myWorkDir + QDir::separator() + myList.at(i);
    //qDebug("Get ExperimentsList Checking: " + mySubDirectoryName.toLocal8Bit());
    //Now look in each dir under the workdir for the xml experiment file
    //It should be the first one found since there should only be
    //one experiment per directory.
    //the experiment file should be named <long guid>.xml
    QDir mySubDirectory( mySubDirectoryName );
    mySubDirectory.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    QStringList myFilters;
    myFilters << "*.xml";
    mySubDirectory.setNameFilters(myFilters);
    //qDebug ("Current directory is: " +  myWorkDir.toAscii());
    QStringList myExperimentFileList = mySubDirectory.entryList();
    if (myExperimentFileList.count() < 1)
    {
      continue;
    }
    QString myFileName = mySubDirectoryName + QDir::separator() +  myExperimentFileList.at(0);

    //qDebug("Get ExperimentsList Checking: " + myFileName.toLocal8Bit());
    if (QFile::exists(myFileName))
    {
      myExperimentList << myFileName;
      //qDebug("Get ExperimentsList Adding: " + myFileName.toLocal8Bit());
    }
  }
  return myExperimentList;
}

QStringList Omgui::getPreAnalysisList()
{
  QStringList myPreAnalysisList;
  QSettings mySettings;
  QString myWorkDir = mySettings.value("dataDirs/preDataDir",
      QDir::homePath() + QDir::separator() + ".omgui").toString() + 
                     QDir::separator() + "preOutputs";
  QDir myDirectory(myWorkDir);
  if (!myDirectory.exists())
  {
    qDebug("Error the pre-analysis working directory does not exist");
    qDebug("Directory we tried to use is : " +  myWorkDir.toAscii());
    return QStringList();
  }
  myDirectory.setFilter( QDir::Dirs | QDir::NoDotAndDotDot );

  QStringList myList = myDirectory.entryList();
  for (unsigned int i = 0; i < static_cast<unsigned int>(myList.size()); ++i) 
  {
    QString mySubDirectoryName = myWorkDir + QDir::separator() + myList.at(i);
    // qDebug("Get PreAnalysisList Checking: " + mySubDirectoryName.toLocal8Bit());
    //Now look in each dir under the workdir for the xml experiment file
    //It should be the first one found since there should only be
    //one experiment per directory.
    //the experiment file should be named <long guid>.xml
    QDir mySubDirectory( mySubDirectoryName );
    mySubDirectory.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    QStringList myFilters;
    myFilters << "*.xml";
    mySubDirectory.setNameFilters(myFilters);
    //qDebug ("Current directory is: " +  myWorkDir.toAscii());
    QStringList myPreAnalysisFileList = mySubDirectory.entryList();
    if (myPreAnalysisFileList.count() < 1)
    {
      continue;
    }
    QString myFileName = mySubDirectoryName + QDir::separator() +  myPreAnalysisFileList.at(0);

    // qDebug("Get PreAnalysisList Checking: " + myFileName.toLocal8Bit());
    if (QFile::exists(myFileName))
    {
      myPreAnalysisList << myFileName;
      // qDebug("Get PreAnalysisList Adding: " + myFileName.toLocal8Bit());
    }
  }
  return myPreAnalysisList;
}

QHash<QString,QString> Omgui::getOutputFormats()
{
  //later this should be something provided by the modeller adapter
  QHash<QString,QString> myHash;
  myHash.insert("GreyTiff",QObject::tr("Tagged Image Format (Integer)"  ));
  myHash.insert("GreyTiff100",QObject::tr("Tagged Image Format 100 (Integer)"  ));
  myHash.insert("FloatingTiff",QObject::tr("Tagged Image Format (Floating Point)" ));
  myHash.insert("GreyBMP",QObject::tr("Bitmap (Integer)" ));
  myHash.insert("FloatingHFA",QObject::tr("Erdas Imagine (Floating Point)" ));
  myHash.insert("ByteHFA",QObject::tr("Erdas Imagine (Integer)" ));
  myHash.insert("ByteASC",QObject::tr("Arc/Info ASCII Grid (Integer)" ));
  myHash.insert("FloatingASC",QObject::tr("Arc/Info ASCII (Floating Point)" ));
  return myHash;

}
QHash<QString,QString> Omgui::getOutputFormatNotes()
{
  //later this should be something provided by the modeller adapter
  QHash<QString,QString> myHash;
  myHash.insert("GreyTiff",QObject::tr("Tagged Image Format (Integer), Range: 0-254, Nodata: 255"  ));
  myHash.insert("GreyTiff100",QObject::tr("Tagged Image Format (Integer), Range: 0-100, Nodata: 127"  ));
  myHash.insert("FloatingTiff",QObject::tr("Tagged Image Format (Floating Point), Range:0-1, Nodata: -1" ));
  myHash.insert("GreyBMP",QObject::tr("Bitmap (Integer), Range: 1-255, Nodata: 0" ));
  myHash.insert("FloatingHFA",QObject::tr("Erdas Imagine (Floating Point), Range: 0-1, Nodata: -1" ));
  myHash.insert("ByteHFA",QObject::tr("Erdas Imagine (Integer), Range: 0-100, Nodata: 101" ));
  myHash.insert("ByteASC",QObject::tr("Arc/Info ASCII Grid (Integer), Range: 0-100, Nodata: 101" ));
  myHash.insert("FloatingASC",QObject::tr("Arc/Info ASCII Grid (Floating Point), Range: 0-1, Nodata: -9999" ));
  return myHash;

}

QHash<QString,QPair<double,double> > Omgui::getOutputFormatRanges()
{
  //later this should be something provided by the modeller adapter
  QHash<QString,QPair<double,double> > myHash;
  myHash.insert("GreyTiff",qMakePair(0.0,254.0));
  myHash.insert("GreyTiff100",qMakePair(0.0,100.0));
  myHash.insert("FloatingTiff",qMakePair(0.0,1.0));
  myHash.insert("GreyBMP",qMakePair(1.0,255.0));
  myHash.insert("FloatingHFA",qMakePair(0.0,1.0));
  myHash.insert("ByteHFA",qMakePair(0.0,100.0));
  myHash.insert("ByteASC",qMakePair(0.0,100.0));
  return myHash;
}

QHash<QString,float> Omgui::getOutputFormatNoData()
{
  //later this should be something provided by the modeller adapter
  QHash<QString,float> myHash;
  myHash.insert("GreyTiff",255);
  myHash.insert("GreyTiff100",127);
  myHash.insert("FloatingTiff",-1);
  myHash.insert("GreyBMP",0);
  myHash.insert("FloatingHFA",-1);
  myHash.insert("ByteHFA",101);
  myHash.insert("ByteASC",101);
  return myHash;
}

QString Omgui::getOutputFormatExtension()
{
  QSettings mySettings;
  QString myFormat = mySettings.value("outputFormat","GreyTiff").toString();
  if (myFormat.contains("Tiff")) return "tif";
  if (myFormat.contains("HFA")) return "img";
  if (myFormat.contains("BMP")) return "bmp";
  if (myFormat.contains("ASC")) return "asc";
  //qDebug("Omgui::getOutputFormatExtension Warning unknown extension!!!");
  //qDebug("Defaulting to img!!!");
  return "img";
}

bool Omgui::createTextFile(QString theFileName, QString theData, bool theAppendFlag )
{
  //create the txt file
  QFile myFile( theFileName );
  QIODevice::OpenMode myFlags;
  if ( theAppendFlag )
  {
    myFlags = QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text;
  }
  else
  {
    myFlags = QIODevice::WriteOnly | QIODevice::Text;
  }
  if ( myFile.open( myFlags ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << theData;
  }
  else
  {
    return false;
  }
  myFile.close();
  return true ;
}

QString Omgui::readTextFile(QString theFileName)
{
  QString myString;
  QFile myFile( theFileName );
  if ( myFile.open( QIODevice::ReadOnly ) )
  {
    myString=myFile.readAll();
    myFile.close();
  }
  return myString;
}

QString Omgui::xmlEncodeAmpersands(QString theString)
{
  theString.replace("&","&amp;");
  return theString;
}

QString Omgui::xmlEncode(QString theString)
{
  theString.replace("<","&lt;");
  theString.replace(">","&gt;");
  theString.replace("&","&amp;");
  return theString;
}

void Omgui::xmlEncodeInPlace(QString &theString)
{
  theString.replace("<","&lt;");
  theString.replace(">","&gt;");
  theString.replace("&","&amp;");
}

QString Omgui::xmlDecode(QString theString)
{
  theString.replace("&lt;","<");
  theString.replace("&gt;",">");
  theString.replace("&amp;","&");
  return theString;
}

void Omgui::xmlDecodeInPlace(QString &theString)
{
  theString.replace("&lt;","<");
  theString.replace("&gt;",">");
  theString.replace("&amp;","&");
}

QString Omgui::i18nPath()
{
  QString myI18nPath;
#ifdef Q_OS_MACX 
  CFURLRef myBundleRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef myMacPath = CFURLCopyFileSystemPath(myBundleRef, kCFURLPOSIXPathStyle);
  const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
  CFRelease(myBundleRef);
  CFRelease(myMacPath);
  QString myPath(mypPathPtr);
  //do some magick so that we can still find i18n dir if
  //openModellerDesktop was not built as a bundle
  if (myPath.contains(".app"))
  {
    myI18nPath = myPath + "/Contents/MacOS/share/openModellerDesktop/i18n";
  }
  else //must be running from a non .app bundle
  {
    QString myI18nPath = QApplication::applicationDirPath()+"/../share/openModellerDesktop/i18n";
  }
#elif WIN32  
  myI18nPath = QApplication::applicationDirPath()+"/i18n";
#else
  myI18nPath = QApplication::applicationDirPath()+"/../share/openModellerDesktop/i18n";
#endif
  
  return myI18nPath;
}

QStringList Omgui::i18nList()
{
  QStringList myList;
  myList << "en_US"; //there is no qm file for this so we add it manually
  QString myI18nPath = i18nPath();
  QDirIterator myIterator(myI18nPath, QDirIterator::Subdirectories);
  while (myIterator.hasNext()) 
  {
    QString myEntry = myIterator.next();
    QFileInfo myFileInfo(myEntry);
    if (!myFileInfo.isFile()) continue;
    QString myFileName = myFileInfo.fileName();
    myList << myFileName.replace("openModellerDesktop_","").replace(".qm","");
  }
  return myList;
}

QColor Omgui::randomColour()
{
  //get a random record from the resultset
  srand(QTime::currentTime().msec());
  int myRand = rand();
  int myValue1 = static_cast<int> (myRand % 256); //range 0-255
  myRand = rand();
  int myValue2 = static_cast<int> (myRand % 256); //range 0-255
  myRand = rand();
  int myValue3 = static_cast<int> (myRand % 256); //range 0-255
  //QString myString = "Random colour: " + 
  //  QString::number(myValue1) +
  //  "," + QString::number(myValue2) +
  //  "," + QString::number(myValue3);
  //qDebug(myString);
  QColor myColour(myValue1,myValue2,myValue3,190);
  return myColour;
}

QString Omgui::defaultStyleSheet()
{
  QString myStyle;
  myStyle += ".glossy{ background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565);"
    "color: white;"
    "padding-left: 4px;"
    "padding-top: 20px;"
    "padding-bottom: 8px;"
    "border: 1px solid #6c6c6c;"
    "}";
  myStyle += ".glossyBlue{ background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3087d3, stop: 0.5 #3794e2, stop: 0.6 #43a6f9, stop:1 #2f87d1);"
    "color: white;"
    "padding-left: 4px;"
    "padding-top: 20px;"
    "padding-bottom: 8px;"
    "border: 1px solid #44a7fb;"
    "}";
  myStyle += "h1 {font-size : 16pt; }";
  myStyle += "h2 {font-size : 14pt; }";
  myStyle += "h3 {font-size : 12pt; }";
  myStyle += "body {font-size : 10pt; }";
  myStyle += ".glossyh3{ "
    "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #616161, stop: 0.5 #505050, stop: 0.6 #434343, stop:1 #656565);"
    "color: white; "
    "padding-left: 4px; "
    "padding-top: 20px;"
    "padding-bottom: 8px;  "
    "border: 1px solid #6c6c6c; }";
  myStyle += ".headerCell, th {color:#466aa5; "
    "font-size : 12pt; "
    "font-weight: bold; "
    "width: 100%;"
    "}";
  myStyle += ".parameterHeader {font-weight: bold;}";
  myStyle += ".largeCell {color:#000000; font-size : 12pt;}";
  myStyle += ".alternateCell {font-weight: bold;}";
  myStyle += ".rocTable "
  "{"
  "  border-width: 1px 1px 1px 1px;"
  "  border-spacing: 2px;"
  "  border-style: solid solid solid solid;" //unsupported
  "  border-color: black black black black;" //unsupported
  "  border-collapse: separate;"
  "  background-color: white;"
  "}";
  return myStyle;
}


QString Omgui::getHtmlHeader()
{
 // Use xhtml transitional
 QString myHtml = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"";
  myHtml += "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
  myHtml += "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\">";
  myHtml += "<head>";
  myHtml += " <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />";
  myHtml += "</head>";
  myHtml += "<body>";
  return myHtml;
}

QString Omgui::getHtmlFooter()
{
 QString myHtml = "</body>";
    myHtml += "</html>";
 return myHtml;
}


bool Omgui::deleteShapeFile(QString theFileName)
{
  theFileName.replace(".shp","");
  QFileInfo myInfo(theFileName + ".shp");
  if (myInfo.exists())
  {
    if(!QFile::remove(theFileName + ".shp"))
    {
      qDebug("Removing file failed : " + theFileName.toLocal8Bit() + ".shp");
      return false;
    }
  }
  myInfo.setFile(theFileName + ".shx");
  if (myInfo.exists())
  {
    if(!QFile::remove(theFileName + ".shx"))
    {
      qDebug("Removing file failed : " + theFileName.toLocal8Bit() + ".shx");
      return false;
    }
  }
  myInfo.setFile(theFileName + ".dbf");
  if (myInfo.exists())
  {
    if(!QFile::remove(theFileName + ".dbf"))
    {
      qDebug("Removing file failed : " + theFileName.toLocal8Bit() + ".dbf");
      return false;
    }
  }
  myInfo.setFile(theFileName + ".prj");
  if (myInfo.exists())
  {
    if(!QFile::remove(theFileName + ".prj"))
    {
      qDebug("Removing file failed : " + theFileName.toLocal8Bit() + ".prj");
      return false;
    }
  }
  return true;
}

bool Omgui::localitiesToShapefile(QString theShapefileName,
    OmgLocalities const &theLocalities) 
{
  if (theLocalities.size() < 1)
  {
    qDebug("Writing " + theShapefileName.toLocal8Bit() + " failed (no locs)");
    return false;
  }
  //
  //    Note: if you get a message like 
  //    ERROR 1: /tmp/foo.shp is not a directory.
  //    It is caused by the /tmp/testshp.* files already existing
  //    (the ERROR comes from OGR and is not very intuitive)
  QString myEncoding = "UTF-8";
  QgsVectorFileWriter::WriterError myError;
  QgsField myIdField("accession",QVariant::String,"String",10,0,"Accession code");
  QgsField myTaxonField("taxon",QVariant::String,"String",255,0,"Taxon name");
  QgsField myLonField("lon",QVariant::Double,"Double",10,6,"longitude");
  QgsField myLatField("lat",QVariant::Double,"Double",10,6,"latitude");
  QgsField myStatusField("status",QVariant::String,"String",10,0,"Present or absent");
  QgsFieldMap myFields;
  myFields.insert(0, myIdField);
  myFields.insert(1, myTaxonField);
  myFields.insert(2, myLonField);
  myFields.insert(3, myLatField);
  myFields.insert(4, myStatusField);
  QgsCoordinateReferenceSystem mySRS = QgsCoordinateReferenceSystem(GEOWkt);

  Omgui::deleteShapeFile(theShapefileName);
  QgsVectorFileWriter myWriter (theShapefileName,
      myEncoding,
      myFields,
      QGis::WKBPoint,
      &mySRS);
  for (int myIteratorInt = 0; myIteratorInt < theLocalities.size(); ++myIteratorInt)
  {
    OmgLocality myLocality = theLocalities[myIteratorInt];
    QgsPoint myPoint(static_cast<double>(myLocality.longitude()), 
        static_cast<double>(myLocality.latitude()));
    //
    // Create a feature
    //
    QgsFeature myFeature;
    myFeature.setGeometry(QgsGeometry::fromPoint(myPoint));
    myFeature.addAttribute(0,myLocality.id());
    myFeature.addAttribute(1,myLocality.label());
    myFeature.addAttribute(2,static_cast<double>(myLocality.longitude()));
    myFeature.addAttribute(3,static_cast<double>(myLocality.latitude()));
    if (myLocality.abundance() > 0)
    {
      if (myLocality.samples().size() == 0)
      {
        myFeature.addAttribute(3,"Present - Not Used");
      }
      else //was actually used in a model
      {
        myFeature.addAttribute(4,"Present");
      }
    }
    else
    {
      if (myLocality.samples().size() == 0)
      {
        myFeature.addAttribute(4,"Absent - Not Used");
      }
      else //was actually used in a model
      {
        myFeature.addAttribute(4,"Absent");
      }
    }
    qDebug() << "Writing feature now";
    qDebug() << myLocality.label();
    qDebug() << myLocality.longitude();
    qDebug() << myLocality.latitude();
    //
    // Write the feature to the filewriter
    // and check for errors
    //
    if (!myWriter.addFeature(myFeature))
    {
      return false;
    }
    myError = myWriter.hasError();
    if(myError==QgsVectorFileWriter::ErrDriverNotFound)
    {
      qDebug("Driver not found error");
      return false;
    }
    else if (myError==QgsVectorFileWriter::ErrCreateDataSource)
    {
      qDebug("Create data source error");
      return false;
    }
    else if (myError==QgsVectorFileWriter::ErrCreateLayer)
    {
      qDebug("Create layer error");
      return false;
    }
  }
  return true;
}

QString Omgui::version()
{
  QString myOmgVersion;
#ifdef OMGMAJORVERSION
  myOmgVersion = 
    QString::number(OMGMAJORVERSION) + "." +
    QString::number(OMGMINORVERSION) + "." +
    QString::number(OMGRELEASEVERSION);

#endif

#ifdef OMGSVNVERSION
  QString myVersionString = QObject::tr("Version: ") + 
    myOmgVersion + " (" + 
    OMGSVNVERSION + ")";  
#else
  QString myVersionString = QObject::tr("Version: ") + 
    myOmgVersion ;  
#endif
  return myVersionString;
}

void Omgui::createThumbnail(QString theSourceImageFileName,
                            QString theOutputFileName,
                            QString theTopLabel,
                            QString theBottomLabel,
                            QString theBottomLabel2)
{
  
  //  +-------------------+
  //  |                   | <- vertical space 1
  //  |     Top Label     |
  //  |                   | <- vertical space 2
  //  | +---------------+ |
  //  | |               | |
  //  | |     IMAGE     | |
  //  | |               | |
  //  | |               | |
  //  | +---------------+ |
  //  |                   | <- vertical space 3
  //  |   Bottom Label    |
  //  |                   | <- vertical space 4
  //  |  Bottom  Label2   |
  //  |                   | <- vertical space 5
  //  +-------------------+
  //   ^                 ^
  //   +-----------------+---- horizontal spaces 1 and 2

  
  int myX = 160;
  int myY = 180;
  int mySpacing = 10; //px
  QFont myFont("arial", 10, QFont::Normal);
  QFontMetrics myMetrics(myFont);
  int myResizeX = myX - (2*mySpacing); //leave space on each side
  int myResizeY = myY - ((5*mySpacing)+(3*myMetrics.height())); //spaces above and below each label
 
  QImage myImage(myX, myY, QImage::Format_ARGB32);
  myImage.fill(qRgba(0,0,0,0)); 
  QPainter myPainter(&myImage);
  myPainter.setRenderHint(QPainter::Antialiasing);
  QPen myPen;
  
  //
  // draw the drop shadow first
  //
  myPen.setWidth(0);
  myPen.setColor(Qt::transparent);
  myPainter.setPen(myPen);
  //gradient bavkground shadow for the thumbnail
  //radial gradient starting in top left corner
  QRadialGradient myGradient(QPointF(0,0), myX/2);
  myGradient.setColorAt(0, Qt::darkGray);
  myGradient.setColorAt(0.99, Qt::darkGray);
  myGradient.setColorAt(1, Qt::gray);
  myPainter.setBrush(myGradient);
  myPainter.drawRoundRect ( QRect(1,1,myX,myY),25,25 );
  //
  // Now draw the rounded rect - it will overpaint
  // most of the above and just leave a little
  // shadow sticking out below and right
  //

  //make a glossygradient for the thumbnail background
  QGradientStops myStops;
  myStops << QGradientStop(0.0,QColor("#3087d3"));
  myStops << QGradientStop(0.5,QColor("#3794e2"));
  myStops << QGradientStop(0.6,QColor("#43a6f9"));
  myStops << QGradientStop(1.0,QColor("#2f87d1"));
  QLinearGradient myGlossyGradientTop(QPointF(0,0), QPointF(0,myY/2));
  myGlossyGradientTop.setStops(myStops);
  QLinearGradient myGlossyGradientBottom(QPointF(0,myY/2), QPointF(0,0));
  myGlossyGradientBottom.setStops(myStops);
  
  myPen.setWidth(2);
  myPen.setColor("#43a6f9");
  myPainter.setPen(myPen);
  myPainter.setBrush(myGlossyGradientTop);
  myPainter.drawRoundRect ( QRect(1,1,myX-4,myY-4),25,25 );
  //myPainter.setBrush(myGlossyGradientBottom);
  //myPainter.drawRoundRect ( QRect(1,1,myX-4,myY-4),25,25 );
  //
  //now paint the label for the top
  //
  myPainter.setFont(myFont);
  int myWidth = myMetrics.width(theTopLabel); //width of text
  if (myWidth > (myX - (2*mySpacing)))
  {
    myWidth = myX - (2*mySpacing);
  }
  int myHeight = myMetrics.height();
  myPen.setColor(Qt::black);
  myPainter.setPen(myPen);
  QPointF myTopLeftPoint =QPointF(  mySpacing , mySpacing);
  QPointF myBottomRightPoint = QPointF( myX-mySpacing , mySpacing+myHeight );
  myPainter.drawText(QRectF(myTopLeftPoint,myBottomRightPoint),
               Qt::AlignCenter || Qt::AlignVCenter,theTopLabel);
  //
  // now get a thumbnail of the image and paste it into our
  // painter
  QFileInfo myFileInfo(theSourceImageFileName);
  QImage mySourceImage;
  if (!myFileInfo.exists())
  {
    //image will be blank
  }
  else 
  {
    mySourceImage.load(theSourceImageFileName);
  }
  QImage myThumbImage = mySourceImage.scaled(
        myResizeX,myResizeY,Qt::KeepAspectRatio,Qt::FastTransformation );
  myTopLeftPoint =QPointF(  mySpacing , (mySpacing*2)+myHeight);
  myBottomRightPoint = QPointF( myX-mySpacing , (mySpacing*2)+myHeight+myResizeY );
  myPainter.drawImage(QRectF(myTopLeftPoint,myBottomRightPoint),
               myThumbImage);
  //
  //now paint the bottom label
  //
  myWidth = myMetrics.width(theBottomLabel); //width of text
  if (myWidth > (myX - (2*mySpacing)))
  {
    myWidth = myX - (2*mySpacing);
  }
  myPen.setColor(Qt::white);
  myPainter.setPen(myPen);
  myTopLeftPoint =QPointF(  mySpacing , (mySpacing*3)+myHeight+myResizeY);
  myBottomRightPoint = QPointF( myX-mySpacing ,(mySpacing*3)+(myHeight*2)+myResizeY );
  myPainter.drawText(QRectF(myTopLeftPoint,myBottomRightPoint),
               Qt::AlignCenter || Qt::AlignVCenter,theBottomLabel);
  //
  //now paint the second label for the bottom
  //
  myPainter.setFont(myFont);
  myWidth = myMetrics.width(theBottomLabel2); //width of text
  if (myWidth > (myX - (2*mySpacing)))
  {
    myWidth = myX - (2*mySpacing);
  }
  myHeight = myMetrics.height();
  myPen.setColor(Qt::white);
  myPainter.setPen(myPen);
  myTopLeftPoint =QPointF(  mySpacing , (mySpacing*5)+myHeight+myResizeY);
  myBottomRightPoint = QPointF( myX-mySpacing ,(mySpacing*5)+(myHeight*2)+myResizeY );
  myPainter.drawText(QRectF(myTopLeftPoint,myBottomRightPoint),
               Qt::AlignCenter || Qt::AlignVCenter,theBottomLabel2);
  //
  //finish up by saving our swish looking thumbnail to disk
  //
  myPainter.end();
  myImage.save(theOutputFileName,"PNG");
}

bool Omgui::loadModelOccurrencesLayer( OmgModel * thepModel )
{
  QSettings mySettings;
  //try to create and add the vectore layer of points for occurrences
  //if the provider is not found or some other problem occurred
  //we continue with just the raster
  QString myShapeFile (thepModel->workDir() + thepModel->shapefileName());
  qDebug("Trying to load: " + thepModel->shapefileName().toLocal8Bit());
  if (!QFile::exists(myShapeFile))
  {
    qDebug("Vector layer does not exist");
    return false;
  }
  else
  {
    QString myProviderName = "ogr";
    QFileInfo myVectorFileInfo(myShapeFile);
    QgsVectorLayer * mypVectorLayer = new QgsVectorLayer(myVectorFileInfo.filePath(), 
        myVectorFileInfo.completeBaseName(), myProviderName);
    if (mypVectorLayer->isValid())
    {
      //qDebug("Vector Layer is valid");
      //
      // For backwards compatibility we need to check
      // if this layer has a status field. If not
      // we display as a single symble, otherwise we
      // use a unique value renderer
      //
      QgsVectorDataProvider *mypProvider = 
        dynamic_cast<QgsVectorDataProvider *>(mypVectorLayer->dataProvider());    
      if (mypProvider)
      { 
        double mySymbolSize(mySettings.value("mapping/localitySymbolSize",3.0).toDouble());
        QgsFieldMap myFields = mypProvider->fields();
        bool myNewFormatFlag = false;
        for (int i = 0; i < myFields.size(); i++ )
        {
          if (myFields[i].name()=="status")
          {
            myNewFormatFlag=true;
            break;
          }
        }
        if (myNewFormatFlag) // uniquevalue renderer
        {
          QColor myPresenceColor(mySettings.value("mapping/presenceColour",QColor(Qt::green).name()).toString());
          QColor myAbsenceColor(mySettings.value("mapping/absenceColour",QColor(Qt::red).name()).toString());
          //
          // Presence with samples symbol
          //
          QgsSymbol *  mypPresenceSymbol = new QgsSymbol(mypVectorLayer->geometryType(),"Present","");
          mypPresenceSymbol->setNamedPointSymbol("hard:circle");
          mypPresenceSymbol->setFillColor(myPresenceColor);
          mypPresenceSymbol->setColor(Qt::black); //outline
          mypPresenceSymbol->setPointSize(mySymbolSize);
          mypPresenceSymbol->setFillStyle(Qt::SolidPattern);

          //
          // Presence with NO samples symbol
          //
          QgsSymbol *  mypPresence2Symbol = new QgsSymbol(mypVectorLayer->geometryType(),"Present - Not Used","");
          mypPresence2Symbol->setNamedPointSymbol("hard:triangle");
          mypPresence2Symbol->setFillColor(myPresenceColor);
          mypPresence2Symbol->setColor(Qt::black); //outline
          mypPresence2Symbol->setPointSize(mySymbolSize);
          mypPresence2Symbol->setFillStyle(Qt::SolidPattern);

          // 
          // Absence with samples symbol
          //
          QgsSymbol *  mypAbsenceSymbol = new QgsSymbol(mypVectorLayer->geometryType(),"Absent","");
          mypAbsenceSymbol->setNamedPointSymbol("hard:circle");
          mypAbsenceSymbol->setFillColor(myAbsenceColor);
          mypAbsenceSymbol->setColor(Qt::black); //outline
          mypAbsenceSymbol->setPointSize(mySymbolSize);
          mypAbsenceSymbol->setFillStyle(Qt::SolidPattern);

          // 
          // Absence with NO samples symbol
          //
          QgsSymbol *  mypAbsence2Symbol = new QgsSymbol(mypVectorLayer->geometryType(),"Absent","");
          mypAbsence2Symbol->setNamedPointSymbol("hard:triangle");
          mypAbsence2Symbol->setFillColor(myAbsenceColor);
          mypAbsence2Symbol->setColor(Qt::black); //outline
          mypAbsence2Symbol->setPointSize(mySymbolSize);
          mypAbsence2Symbol->setFillStyle(Qt::SolidPattern);

          //create renderer
          QgsUniqueValueRenderer *mypRenderer = new QgsUniqueValueRenderer(mypVectorLayer->geometryType());
          qDebug("Provider ok...getting status field");
          int myFieldIndex = mypProvider->fieldNameIndex("status");
          if(myFieldIndex > 0) 
          {
            qDebug("Status field found");
            mypRenderer->setClassificationField(myFieldIndex);
            mypRenderer->insertValue("Present",mypPresenceSymbol);
            mypRenderer->insertValue("Present - Not Used",mypPresence2Symbol);
            mypRenderer->insertValue("Absent",mypAbsenceSymbol);
            mypRenderer->insertValue("Absent - Not Used",mypAbsence2Symbol);
            mypVectorLayer->setRenderer(mypRenderer);
            // Add the layer to the Layer Registry
            QgsMapLayerRegistry::instance()->addMapLayer(mypVectorLayer, TRUE);
          } // field index
        }
        else // old format so use single symbol renderer
        {
          // old format did not support absence
          QgsSymbol *  mypSymbol = new QgsSymbol(mypVectorLayer->geometryType());
          QColor myPresenceColor(mySettings.value("mapping/presenceColour",QColor(Qt::green).name()).toString());
          mypSymbol->setFillColor(myPresenceColor);
          mypSymbol->setColor(Qt::black); //outline
          mypSymbol->setPointSize(mySymbolSize);
          mypSymbol->setFillStyle(Qt::SolidPattern);
          QgsSingleSymbolRenderer *mypRenderer = new QgsSingleSymbolRenderer(mypVectorLayer->geometryType());
          mypRenderer->addSymbol(mypSymbol);
          mypVectorLayer->setRenderer(mypRenderer);
          // Add the layer to the Layer Registry
          QgsMapLayerRegistry::instance()->addMapLayer(mypVectorLayer, TRUE);
        } //old format
      } //provider valid
      else
      {
        return false;
      }
    } //vector layer valid
    else
    {
      //qDebug("Vector Layer is NOT valid, and Not Loaded");
      return false;
    }
  }
  return true;
}
