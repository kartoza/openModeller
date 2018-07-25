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

//local includes
#include "omgsupportedfileformats.h"

//qt includes
#include <QSettings>

//GDAL
#include <gdal_priv.h>
#include <cpl_string.h>
#include <ogr_spatialref.h>


OmgSupportedFileFormats::OmgSupportedFileFormats(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);

  connect(pnbClose, SIGNAL(clicked()), this, SLOT( accept()));

  GDALAllRegister();
  //GDALDriverH		hDriver;
  int iDr;
  for( iDr = 0; iDr < GDALGetDriverCount(); iDr++ )
    {
        GDALDriverH hDriver = GDALGetDriver(iDr);
        
        // From gdal_translate.cpp
        /* if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) != NULL
            || GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATECOPY,
                                    NULL ) != NULL )
        {
            printf( "  %s: %s\n",
                    GDALGetDriverShortName( hDriver ),
                    GDALGetDriverLongName( hDriver ) );

        }*/
        lstSupportedFormats->addItem( QString(GDALGetDriverShortName( hDriver )) + ": " + QString(GDALGetDriverLongName( hDriver )) );
    }
}

OmgSupportedFileFormats::~OmgSupportedFileFormats()
{
}

void OmgSupportedFileFormats::accept()
{
  done(1);
}
