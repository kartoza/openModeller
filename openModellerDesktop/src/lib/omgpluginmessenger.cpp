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


//QT Includes
#include "omgpluginmessenger.h"
//needed to be able to do qDebug() << 
#include <QtDebug>
#include <QCoreApplication>


OmgPluginMessenger::OmgPluginMessenger(QObject *parent)
{

}
OmgPluginMessenger::~OmgPluginMessenger()
{

}

void OmgPluginMessenger::emitError(QString theError)
{
  //qDebug() << "OmgPluginMessenger::emitError " 
  //         << theError;
  emit error(theError); 
}
void OmgPluginMessenger::emitMessage(QString theMessage)
{
  //qDebug() << "OmgPluginMessenger::emitMessage " 
  //         << theMessage;
  emit message(theMessage);
}
void OmgPluginMessenger::emitModelError(QString theModelGuid,QString theMessage)
{
  //qDebug() << "OmgPluginMessenger::emitModelError " 
  //         << theModelGuid << " " << theMessage;
  emit modelError(theModelGuid,theMessage);
}
void OmgPluginMessenger::emitModelMessage(QString theModelGuid,QString theMessage)
{
  //qDebug() << "OmgPluginMessenger::emitModelMessage " 
  //         << theModelGuid << " " << theMessage;
  emit modelMessage(theModelGuid,theMessage);
  QCoreApplication::processEvents();
}
void OmgPluginMessenger::emitModelDone(QString theModelGuid)
{
  //qDebug() << "OmgPluginMessenger::emitModelDone " 
  //         << theModelGuid ;
  emit modelDone(theModelGuid);
}
void OmgPluginMessenger::emitModelCreationProgress(QString theModelGuid,int theProgress)
{
  //qDebug() << "OmgPluginMessenger::emitModelCreationProgress " 
  //         << theModelGuid << " " << theProgress;
  emit modelCreationProgress(theModelGuid,theProgress);
}
void OmgPluginMessenger::emitModelProjectionProgress(QString theModelGuid,int theProgress)
{
  //qDebug() << "OmgPluginMessenger::emitModelProjectionProgress " 
  //         << theModelGuid << " " << theProgress;
  emit modelProjectionProgress(theModelGuid,theProgress);
}
void OmgPluginMessenger::emitRefresh()
{
  //qDebug() << "OmgPluginMessenger::emitRefresh" ;
  emit refresh();

}

// Next methods are plugin messenger related

void OmgPluginMessenger::emitFileWritten(QString theShapeFile, QString theTextFile,QString theTaxonName,int theCount)
{
  emit fileWritten(theShapeFile,theTextFile,theTaxonName,theCount);
}
void OmgPluginMessenger::emitFileNotWritten(QString theTaxonName)
{
  emit fileNotWritten(theTaxonName);
}
