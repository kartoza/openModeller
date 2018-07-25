/***************************************************************************
                          omgmodellerpluginregistry.h    
           Singleton class for keeping track of loaded modeller plugins
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
 /* $Id: omgmodellerpluginregistry.h 3512 2007-08-14 14:01:10Z timlinux $ */
 
#ifndef OMGMODELLERPLUGINREGISTRY_H
#define OMGMODELLERPLUGINREGISTRY_H

#include "omgmodellerplugininterface.h"
#include <QMap>

class QString;
class QStringList;
/** \ingroup library
* \brief This class keeps a list of available modeller plugins and
* can return a pointer to one on demand.
*/
class OMG_LIB_EXPORT OmgModellerPluginRegistry : public QObject
{
   Q_OBJECT;

public:

 //! Returns the instance pointer, creating the object on the first call
 static OmgModellerPluginRegistry * instance();
/*! Return the number of registered modeller plugins.
 *
 * */
 const int count();
 
 /** Retrieve a pointer to a loaded plugin by id
  * @Note The client class should never delete the pointer returned by
  * this function.
  * @param theName of the plugin to return. The parameter is optional. If
  * ommitted the default plugin will be returned.
  * @return A OmgModellerPluginInterface instance.
  */
 OmgModellerPluginInterface* getPlugin(QString thePluginName="");
 /** Get the available plugins names.
  * @return QStringList containing all the registered plugins names.
  */
 QStringList names();

signals:


protected:

 //! protected constructor
 OmgModellerPluginRegistry( QObject * parent = 0 );

private:

 static OmgModellerPluginRegistry* mpInstance;

 QMap<QString,OmgModellerPluginInterface*> mPluginsMap;


}; // class OmgModellerPluginRegistry

#endif //OMGMODELLERPLUGINREGISTRY_H

