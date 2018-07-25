/***************************************************************************
 *  OmgScraperPluginRegistry.cpp  -  Singleton class for tracking scraper plugins.
 *                         -------------------
 * begin                : Tues 26 Dec 2007
 * copyright            : (C) 2007 by Tim Sutton
 * email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: omgscraperpluginregistry.cpp 3110 2007-05-29 14:29:49Z timlinux $ */

#include "omgscraperpluginregistry.h"
#include "omgui.h"

#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QSettings>

//
// Static calls to enforce singleton behaviour
//
OmgScraperPluginRegistry *OmgScraperPluginRegistry::mpInstance = 0;
OmgScraperPluginRegistry *OmgScraperPluginRegistry::instance()
{
  if (mpInstance == 0)
  {
    mpInstance = new OmgScraperPluginRegistry();
  }
  return mpInstance;
}

//
// Main class begins now...
//

OmgScraperPluginRegistry::OmgScraperPluginRegistry(QObject *parent) : QObject(parent) 
{
  //now populate the registry since this is the first time its being used
  QString myPluginsPath=Omgui::pluginDirPath() + QDir::separator() + "scraper";
  //if we are running unit tests we need to be able to load directly
  //from the build tree so we check the above path exists and if
  //it doesnt assume we are running in the build tree...
  if (!QFile::exists(myPluginsPath))
  {
    myPluginsPath=Omgui::pluginDirPath() + QDir::separator() + "scraperplugins";
  }
  //qDebug("Searching for plugins in : " + myPluginsPath.toLocal8Bit());
  QDir myPluginsDir(myPluginsPath);
  foreach (QString myFileName, myPluginsDir.entryList(QDir::Files))
  {
    //check if the filename matches the users chose scraper plugin
    //qDebug("Plugin registry trying to load : " + myFileName.toLocal8Bit());
    QPluginLoader myLoader(myPluginsDir.absoluteFilePath(myFileName));
    //qDebug("Plugin registry: loaded : " + myFileName.toLocal8Bit());
    QObject *mypPlugin = myLoader.instance();
    //qDebug("Plugin to registry: cast to qobject : " + myFileName.toLocal8Bit());
    if (mypPlugin)
    {
      //try to cast to a scraper plugin
      OmgScraperPluginInterface * mypScraperPlugin = qobject_cast<OmgScraperPluginInterface *>(mypPlugin);
      //qDebug("Plugin to registry: cast to scraperplugin : " + myFileName.toLocal8Bit());
      if (!mypScraperPlugin->getName().isEmpty())
      {
        mPluginsMap.insert(mypScraperPlugin->getName(),mypScraperPlugin);
        //qDebug("Added scraper plugin to registry : " + mypScraperPlugin->getName().toLocal8Bit());
      }
      else
      {
        myLoader.unload();
        delete mypScraperPlugin;
        //qDebug("...plugin name was invalid");
      }
    }
    else
    {
      myLoader.unload();
      delete mypPlugin;
      //qDebug("...plugin could not be loaded");
    }
  }
  //qDebug("OmgScraperPluginRegistry created!");
}
const int OmgScraperPluginRegistry::count()
{
  return mPluginsMap.count();
}

OmgScraperPluginInterface * OmgScraperPluginRegistry::getPlugin(QString thePluginName)
{
  //if no explicit plugin name was given return the default one
  if (thePluginName.isEmpty())
  {
    //find out which plugin should be active
    QSettings mySettings;
    thePluginName=
      mySettings.value("locScraper/scraperPluginName", "GBIF REST Web Service Plugin" ).toString();
  }
  //qDebug("Getting the instance of: " + thePluginName.toLocal8Bit());
  //look for the named plugin - if its not registered return 0
  return (OmgScraperPluginInterface*) mPluginsMap.value(thePluginName,0);
}

QStringList OmgScraperPluginRegistry::names()
{
  QStringList myList = (QStringList) mPluginsMap.keys();
  return myList;
}

QMap<QString,QString> OmgScraperPluginRegistry::licenses()
{
  QMap<QString,QString> myMap;

  foreach (QString myName, names()) 
  {
    myMap[myName] = mPluginsMap.value(myName)->getLicense();
  }
  return myMap;
}
