/**************************************************************************
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
#include <omggdal.h>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QDebug>
int main(int argc, char *argv[])
{
  printf("Started with : %s\n",argv[1]);

  QString myInputFileName(argv[1]);
  //save a nice looking png of the image to disk
  OmgGdal myGdal;
  QFileInfo myFileInfo(myInputFileName);
  //get the raw wild name but without its extension
  QString myBaseName=myFileInfo.dir().absolutePath()+QDir::separator()+myFileInfo.baseName();
  QString myOutputFileName (myBaseName+".png");
  //convert tif generated by om to pseudocolor png
  qDebug("Calling omggdal::writeImage(" +  myInputFileName.toLocal8Bit() + ","
      + myOutputFileName.toLocal8Bit()+")");
  myGdal.writeImage(myInputFileName,myOutputFileName);


  return EXIT_SUCCESS;
}

