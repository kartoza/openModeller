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

#include "omgscraperplugininterface.h"
#include "omgui.h"

//QT Includes
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>

OmgScraperPluginInterface::~OmgScraperPluginInterface()
{
}

const OmgPluginMessenger * OmgScraperPluginInterface::getMessenger() 
{
  //qDebug("Getting messenger"); 
  return &mMessenger;
}
void OmgScraperPluginInterface::setMinimumRecords(int theMinimum)
{
  mMinimumRecords=theMinimum;
  if (mMinimumRecords < 1)
  {
    mMinimumRecords = 1;
  }
}

QString OmgScraperPluginInterface::createTextFile(QString theFileName)
{
  if (mLocalityVector.size() < mMinimumRecords)
  {
    return QString();
  }
  // need to strip off the .shp and replace with .txt
  QFileInfo myFileInfo(theFileName);
  QString myBaseString = myFileInfo.dir().path()+QString("/")+myFileInfo.baseName();  // excludes any extension
  //create the txt file
  QString myFileName(myBaseString+".txt");
  QFile myFile( myFileName );
  if ( myFile.open( QIODevice::WriteOnly ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << "#id\ttaxon\tlon\tlat\nabundance\r\n";
    for (int myIteratorInt = 0; myIteratorInt < mLocalityVector.size(); ++myIteratorInt)
    {
      OmgLocality myLocality = mLocalityVector[myIteratorInt];
      myQTextStream << myLocality.id() << "\t" << myLocality.label() << "\t" << myLocality.longitude() << "\t" << myLocality.latitude() << "\r\n";
      //qDebug("Writing " +
      //        QString::number(myLocality.longitude).toLocal8Bit() + " " +
      //        QString::number(myLocality.latitude).toLocal8Bit() + " to "
      //        + myFileName.toLocal8Bit());
    }
  }
  else
  {
    QString myWarning(QObject::tr("Unable to open ") + myFileName.toLocal8Bit() + QObject::tr(" for writing!"));
    mMessenger.emitError(myWarning);
  }
  myFile.close();
  return QString(myFileName);
}

void OmgScraperPluginInterface::createShapefile(QString theShapefileName)
{
  if (mLocalityVector.size() < mMinimumRecords)
  {
    return;
  }
  Omgui::localitiesToShapefile(theShapefileName,mLocalityVector);
}
