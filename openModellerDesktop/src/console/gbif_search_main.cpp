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
#include <omggbifconsolesearch.h>
#include <QCoreApplication>
#include <QObject>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QString>

int main(int argc, char *argv[])
{
  QCoreApplication myApp(argc,argv);
  //NOTE: make sure these lines stay after myApp init above
  QCoreApplication::setOrganizationName("Linux GIS UK");
  QCoreApplication::setOrganizationDomain("linux.gis.co.uk");
  QCoreApplication::setApplicationName("OpenModeller Gui");
  printf("%s started with %i arguements\n", argv[0], argc);
  OmgGbifConsoleSearch myGbif;
  QObject::connect(&myGbif, SIGNAL(searchDone()), &myApp, SLOT(quit()));
  myGbif.search(QString(argv[1]));
  return myApp.exec();
}
