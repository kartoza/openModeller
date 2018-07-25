/***************************************************************************
 *  OmgModellerPluginRegistry.cpp  -  Singleton class for tracking modeller plugins.
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
/* $Id: omgmodellerpluginregistry.cpp 4024 2008-02-11 19:19:34Z timlinux $ */

#include "omgmodellerpluginregistry.h"
#include "omgui.h"

#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QSettings>

//
// Static calls to enforce singleton behaviour
//
OmgModellerPluginRegistry *OmgModellerPluginRegistry::mpInstance = 0;
OmgModellerPluginRegistry *OmgModellerPluginRegistry::instance()
{
  if (mpInstance == 0)
  {
    mpInstance = new OmgModellerPluginRegistry();
  }
  return mpInstance;
}

//
// Main class begins now...
//

OmgModellerPluginRegistry::OmgModellerPluginRegistry(QObject *parent) : QObject(parent) 
{
  //now populate the registry since this is the first time its being used
  QString myPluginsPath=Omgui::pluginDirPath() + QDir::separator() + "modeller";
  //if we are running unit tests we need to be able to load directly
  //from the build tree so we check the above path exists and if
  //it doesnt assume we are running in the build tree...
  if (!QFile::exists(myPluginsPath))
  {
    myPluginsPath=Omgui::pluginDirPath() + QDir::separator() + "modellerplugins";
  }
  //qDebug("Searching for plugins in : " + myPluginsPath.toLocal8Bit());
  QDir myPluginsDir(myPluginsPath);
  foreach (QString myFileName, myPluginsDir.entryList(QDir::Files))
  {
    //check if the filename matches the users chose modeller plugin
    //qDebug("Plugin registry trying to load : " + myFileName.toLocal8Bit());
    QPluginLoader myLoader(myPluginsDir.absoluteFilePath(myFileName));
    //qDebug("Plugin registry: loaded : " + myFileName.toLocal8Bit());
    QObject *mypPlugin = myLoader.instance();
    //qDebug("Plugin to registry: cast to qobject : " + myFileName.toLocal8Bit());
    if (mypPlugin)
    {
      //try to cast to a modeller plugin
      OmgModellerPluginInterface * mypModellerPlugin = qobject_cast<OmgModellerPluginInterface *>(mypPlugin);
      //qDebug("Plugin to registry: cast to modellerplugin : " + myFileName.toLocal8Bit());
      if (!mypModellerPlugin->getName().isEmpty())
      {
        mPluginsMap.insert(mypModellerPlugin->getName(),mypModellerPlugin);
        //qDebug("Added modeller plugin to registry : " + mypModellerPlugin->getName().toLocal8Bit());
      }
      else
      {
        myLoader.unload();
        delete mypModellerPlugin;
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
  //qDebug("OmgModellerPluginRegistry created!");
}
const int OmgModellerPluginRegistry::count()
{
  return mPluginsMap.count();
}

OmgModellerPluginInterface * OmgModellerPluginRegistry::getPlugin(QString thePluginName)
{
  //if no explicit plugin name was given return the default one
  if (thePluginName.isEmpty())
  {
    //find out which plugin should be active
    QSettings mySettings;
    thePluginName=
      mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  }
  //qDebug("Getting the instance of: " + thePluginName.toLocal8Bit());
  //look for the named plugin - if its not registered return 0
  return mPluginsMap.value(thePluginName,0);
}

QStringList OmgModellerPluginRegistry::names()
{
  QStringList myList = (QStringList) mPluginsMap.keys();
  return myList;
}

