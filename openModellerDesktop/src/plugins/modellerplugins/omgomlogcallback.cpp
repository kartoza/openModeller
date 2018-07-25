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

#include <omgomlogcallback.h>

OmgOmLogCallback::OmgOmLogCallback() : 
      QObject(),
      LogCallback()
{

}

OmgOmLogCallback::OmgOmLogCallback(QObject * parent) : 
      QObject (parent), 
      LogCallback()
{

}

OmgOmLogCallback::~OmgOmLogCallback( )
{

}

//see header for explanation of why parameter is unused
//do not try to use the text stream!
OmgOmLogCallback::OmgOmLogCallback( QTextStream& theTextStream ) : 
     QObject(), 
     LogCallback( )
{

}


void OmgOmLogCallback::operator()(Log::Level theLevel, const std::string& theLogMessage )
{
  //qDebug (mModelGuid.toLocal8Bit() + " :: " + QString(theLogMessage.c_str()).toLocal8Bit());
  if (theLevel==Log::Error)
  {
    emit omLogError(mModelGuid, QString(theLogMessage.c_str()).trimmed());
  }
  else
  {
    //qDebug(theLogMessage.c_str());
    emit omLogMessage(mModelGuid, QString(theLogMessage.c_str()).trimmed());
  }
}

void OmgOmLogCallback::setModelGuid(QString theModelGuid)
{
  mModelGuid = theModelGuid;
}
