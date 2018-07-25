/***************************************************************************
                          omgguid.h  -  description
                             -------------------
    begin                : April 2007
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

#ifndef OMGGRADIENTFACTORY_H
#define OMGGRADIENTFACTORY_H

#include <QLinearGradient>
#include <QList>
#include <QColor>
/** \ingroup library
  * \brief A factory class for creating QGradients
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgGradientFactory 
{
  public:
    /** Constructor . */
    OmgGradientFactory();
    /** Desctructor . */
    virtual ~OmgGradientFactory();
  private:
};

#endif //OMGGRADIENTFACTORY_H

