/***************************************************************************
                          omgscraperpluginregistry.h    
           Singleton class for keeping track of loaded scraper plugins
                             -------------------
    begin                : Tues Dec 26 2006
    copyright            : (C) 2006 by Tim Sutton
    email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /* $Id: omgscraperpluginregistry.h 3512 2007-08-14 14:01:10Z timlinux $ */
 
#ifndef OMGSCRAPERPLUGINREGISTRY_H
#define OMGSCRAPERPLUGINREGISTRY_H

#include "omgscraperplugininterface.h"
#include <QMap>

class QString;
class QStringList;
/** \ingroup library
* \brief This class keeps a list of available scraper plugins and
* can return a pointer to one on demand.
*/
class OMG_LIB_EXPORT OmgScraperPluginRegistry : public QObject
{
   Q_OBJECT;

public:

 //! Returns the instance pointer, creating the object on the first call
 static OmgScraperPluginRegistry * instance();
/*! Return the number of registered scraper plugins.
 *
 * */
 const int count();
 
 /** Retrieve a pointer to a loaded plugin by id
  * @Note The client class should never delete the pointer returned by
  * this function.
  * @param theName of the plugin to return. The parameter is optional. If
  * ommitted the default plugin will be returned.
  * @return A OmgScraperPluginInterface instance.
  */
 OmgScraperPluginInterface* getPlugin(QString thePluginName="");
 /** Get the available plugins names.
  * @return QStringList containing all the registered plugins names.
  */
 QStringList names();
 /** Get the licenses for available scraper plugins
   * @return a QMap<QString,QString> where the key is the plugin name
   * and the value is the license text for that plugin.
   */
 QMap<QString,QString> licenses();
signals:


protected:

 //! protected constructor
 OmgScraperPluginRegistry( QObject * parent = 0 );

private:

 static OmgScraperPluginRegistry* mpInstance;

 QMap<QString,OmgScraperPluginInterface*> mPluginsMap;


}; // class OmgScraperPluginRegistry

#endif //OMGSCRAPERPLUGINREGISTRY_H

