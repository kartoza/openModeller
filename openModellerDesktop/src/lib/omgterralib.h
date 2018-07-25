/***************************************************************************
                          omgterralib.h  -  description
                             -------------------
    begin                : Feb 2007
    copyright            : (C) 2007 by Tim Sutton
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

#ifndef OMGTERRALIB_H
#define OMGTERRALIB_H

#include <QString>
/** \ingroup library
  * \brief A utility class to manage communication with a terralib database instance.
  * @author Tim Sutton
  */

#include <TeDatabaseFactory.h>
#include <TeSharedPtr.h>

class OMG_LIB_EXPORT OmgTerralib 
{
public:
  /** Constructor . */
  OmgTerralib();
  /** Desctructor . */
  virtual ~OmgTerralib();
private:
TeSharedPtr< TeDatabase > db_;};

#endif //OMGTERRALIB_H

