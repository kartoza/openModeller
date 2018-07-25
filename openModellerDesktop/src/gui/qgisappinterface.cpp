/***************************************************************************
                          qgisappinterface.cpp
                          Interface class for accessing exposed functions
                          in QgisApp
                             -------------------
    copyright            : (C) 2002 by Gary E.Sherman
    email                : sherman at mrcc dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgisappinterface.cpp 7114 2007-07-30 07:58:57Z mhugent $ */

#include <iostream>
#include <QString>
#include <QMenu>

#include "qgisappinterface.h"
#include "omgmainwindow.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"
#include "qgsmapcanvas.h"
//#include "qgslegend.h"

QgisAppInterface::QgisAppInterface ( OmgMainWindow * thepOmgMainWindow )
    : mpOmgMainWindow ( thepOmgMainWindow )
{
  // connect signals
  //connect ( mpOmgMainWindow->legend(), SIGNAL(currentLayerChanged(QgsMapLayer *)),
  //           this, SIGNAL(currentLayerChanged(QgsMapLayer *)) );

}

QgisAppInterface::~QgisAppInterface()
{
}

void QgisAppInterface::zoomFull()
{
  //mpOmgMainWindow->zoomFull();
}

void QgisAppInterface::zoomToPrevious()
{
  //mpOmgMainWindow->zoomPrevious();
}

void QgisAppInterface::zoomToActiveLayer()
{
  //mpOmgMainWindow->zoomToLayerExtent();
}

QgsVectorLayer * QgisAppInterface::addVectorLayer ( QString vectorLayerPath, QString baseName, QString providerKey )
{
  //mpOmgMainWindow->addVectorLayer(vectorLayerPath, baseName, providerKey);
  //TODO fix this so it returns something meaningfull
  return 0;
}

QgsRasterLayer *  QgisAppInterface::addRasterLayer ( QString rasterLayerPath, QString baseName )
{
  return 0;//mpOmgMainWindow->addRasterLayer( QStringList(rasterLayerPath) );
}

QgsRasterLayer * QgisAppInterface::addRasterLayer ( QgsRasterLayer * theRasterLayer, bool theForceRenderFlag )
{
  return 0;//mpOmgMainWindow->addRasterLayer(theRasterLayer, theForceRenderFlag);
}

bool QgisAppInterface::addProject ( QString theProjectName )
{
  return false;//mpOmgMainWindow->addProject(theProjectName);
}

void QgisAppInterface::newProject ( bool thePromptToSaveFlag )
{
  //mpOmgMainWindow->fileNew(thePromptToSaveFlag);
}

QgsMapLayer *QgisAppInterface::activeLayer()
{
  return NULL;//mpOmgMainWindow->activeLayer();
}

void QgisAppInterface::addPluginToMenu ( QString name, QAction* action )
{
  //mpOmgMainWindow->addPluginMenu(name, action);
}

void QgisAppInterface::removePluginMenu ( QString name, QAction* action )
{
  //mpOmgMainWindow->removePluginMenu(name, action);
}

int QgisAppInterface::addToolBarIcon ( QAction * qAction )
{
  // add the menu to the master Plugins menu
  return 0;//mpOmgMainWindow->addPluginToolBarIcon(qAction);
}

void QgisAppInterface::removeToolBarIcon ( QAction *qAction )
{
  //mpOmgMainWindow->removePluginToolBarIcon(qAction);
}

int QgisAppInterface::addRasterToolBarIcon ( QAction * qAction )
{
  // add the menu to the master Plugins menu
  return 0;//mpOmgMainWindow->addPluginToolBarIcon(qAction);
}

void QgisAppInterface::removeRasterToolBarIcon ( QAction *qAction )
{
  //mpOmgMainWindow->removePluginToolBarIcon(qAction);
}

int QgisAppInterface::addVectorToolBarIcon ( QAction * qAction )
{
  // add the menu to the master Plugins menu
  return 0;//mpOmgMainWindow->addPluginToolBarIcon(qAction);
}

void QgisAppInterface::removeVectorToolBarIcon ( QAction *qAction )
{
  //mpOmgMainWindow->removePluginToolBarIcon(qAction);
}

int QgisAppInterface::addDatabaseToolBarIcon ( QAction * qAction )
{
  // add the menu to the master Plugins menu
  return 0;//mpOmgMainWindow->addPluginToolBarIcon(qAction);
}

void QgisAppInterface::removeDatabaseToolBarIcon ( QAction *qAction )
{
  //mpOmgMainWindow->removePluginToolBarIcon(qAction);
}

int QgisAppInterface::addWebToolBarIcon ( QAction * qAction )
{
  // add the menu to the master Plugins menu
  return 0;//mpOmgMainWindow->addPluginToolBarIcon(qAction);
}

void QgisAppInterface::removeWebToolBarIcon ( QAction *qAction )
{
  //mpOmgMainWindow->removePluginToolBarIcon(qAction);
}

QToolBar* QgisAppInterface::addToolBar ( QString name )
{
  return NULL; //mpOmgMainWindow->addToolBar(name);
}

void QgisAppInterface::openURL ( QString url, bool useQgisDocDirectory )
{
  //mpOmgMainWindow->openURL(url, useQgisDocDirectory);
}

std::map<QString, int> QgisAppInterface::menuMapByName()
{

  std::map<QString, int> myMap;
  return myMap; //mpOmgMainWindow->menuMapByName();
}

std::map<int, QString> QgisAppInterface::menuMapById()
{
  std::map<int, QString> myMap;
  return myMap;//mpOmgMainWindow->menuMapById();
}

QgsMapCanvas * QgisAppInterface::mapCanvas()
{
  return mpOmgMainWindow->mpMapCanvas;
}

QWidget * QgisAppInterface::mainWindow()
{
  return mpOmgMainWindow;
}
