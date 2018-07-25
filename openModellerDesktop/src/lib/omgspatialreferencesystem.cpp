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
#include "omgspatialreferencesystem.h"
#include <QFile>
#include <QTextStream>


OmgSpatialReferenceSystem::OmgSpatialReferenceSystem()
{

}

OmgSpatialReferenceSystem::~OmgSpatialReferenceSystem()
{

}



OmgSpatialReferenceSystem::WktMap OmgSpatialReferenceSystem::getWktMap()
{
  WktMap myWktMap;
  //first lat long hard coded options, rest of WKTs we will try to get from our resource file
  myWktMap["Lat/Long WGS84"] = "GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9108\"]],AXIS[\"Lat\",NORTH],AXIS[\"Long\",EAST],AUTHORITY[\"EPSG\",\"4326\"]]";


  // The wkt list is in the qt reources file (see reources dir and qt docs on how resource bundles work)
  QFile myQFile(":/wkt_defs.txt" );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the file
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
    while ( !myQTextStream.atEnd() )
    {
      // line of text excluding '\n'
      // lines are in pairs
      // first line in pair has descriptive name for wkt
      // second line actual wkt
      myCurrentLine= myQTextStream.readLine();
      if (myCurrentLine.left(4)!="PROJ")
      {
        myWktMap[myCurrentLine]=myQTextStream.readLine();
      }
    }
    myQFile.close();
  }
  else
  {
    qDebug("Opening wkt file failed");
  }
  //qDebug("Opening wkt completed");
  return myWktMap;
}

