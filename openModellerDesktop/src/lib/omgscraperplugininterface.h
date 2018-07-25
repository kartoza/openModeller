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

#ifndef OMGSCRAPERPLUGININTERFACE_H
#define OMGSCRAPERPLUGININTERFACE_H

#include "omglocality.h"
#include "omgpluginmessenger.h"

//QT Includes
#include <QString>

/** \ingroup library
 * \brief An abstract base class that provides the mandatory api for
 * all 'scraper' plugins. These are plugins that know how
 * to fetch locality data from online databases.
 */

class OMG_LIB_EXPORT OmgScraperPluginInterface 
{

public: 
  virtual ~OmgScraperPluginInterface();
  virtual bool search(QString theTaxonName, QString theFileName)=0;
  void setMinimumRecords(int theMinimum);
  virtual const QString getName()=0;
  virtual const QString getLicense()=0;
  /** Get a pointer to the plugin messenger instance associated with this plugin.
   * The pluginmessenger is used to relay signals between the application using the
   * plugin and the plugin itself. Ideally we would just make the plugin interface
   * inherit qobject and implement the signals directly in the interface, but this
   * approach only works on mac and linux at the moment so we resort to this slightly
   * more cumbersome appraoch.
   * @return OmgPluginMessenger *
  */
  virtual const OmgPluginMessenger * getMessenger() ;


protected:
  /** The default constructor is protected so that no besides inheriting classes
   * can use it! */ 
  OmgScraperPluginInterface() {};
  typedef QVector<OmgLocality> LocalityVector;
  LocalityVector mLocalityVector;
  QString mTaxonName;
  QString mFileName;
  int mMinimumRecords;

  void createShapefile(QString theShapefileName);
  /** Create an output text file.
  * @note Takes a shapefile name, strips the .shp off and saves a text file .txt
  */
  QString createTextFile(QString theShapefileName);
  /** The plugin messenger instance associated with this plugin */
  OmgPluginMessenger mMessenger;
};

// Declare plugin interfaces for Qt plugin loader
Q_DECLARE_INTERFACE(OmgScraperPluginInterface, "org.openmodeller.OmgScraperPluginInterface/1.0")

#endif //OMGSCRAPERPLUGININTERFACE
