/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     * 
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
#ifndef OMGSPATIALREFERENCESYSTEM_H
#define OMGSPATIALREFERENCESYSTEM_H

//qt includes
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

/** \ingroup library
 * \brief A representation of the supported spatial reference
 * systems in openModeller Desktop. 
 */
class OMG_LIB_EXPORT OmgSpatialReferenceSystem 
{
  public:
    OmgSpatialReferenceSystem();
    ~OmgSpatialReferenceSystem();


    typedef QMap<QString, QString> WktMap; 

    WktMap getWktMap();


};

#endif //OMSPATIALREFERENCESYSTEM_H
