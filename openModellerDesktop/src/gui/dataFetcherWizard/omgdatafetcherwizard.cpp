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

#include <omgdatafetcherwizard.h>

OmgDataFetcherWizard::OmgDataFetcherWizard(QWidget* parent , Qt::WFlags fl ) :
  mpLicensePage(new OmgDataFetcherWizardLicensePage(this)),
  mpSearchPage(new OmgDataFetcherWizardSearchPage(this)),
  mpProgressPage(new OmgDataFetcherWizardProgressPage(this))
{
  addPage(mpLicensePage);
  addPage(mpSearchPage);
  addPage(mpProgressPage);
  mpProgressPage->setTaxonListWidget(mpSearchPage->getTaxonListWidget());
  connect(this, SIGNAL(currentIdChanged(int)), 
      this, SLOT(pageChanged(int)));
  connect(mpProgressPage, SIGNAL(shapefileCreated( QString )),
      this, SLOT(shapefileCreated( QString )));
};

OmgDataFetcherWizard::~OmgDataFetcherWizard()
{

};
void OmgDataFetcherWizard::pageChanged ( int theId ) 
{
  qDebug("Current Page Changed");
  if (currentPage() != 0 )
  {
    if (currentPage()->isFinalPage())
    {
      qDebug("calling search");
      mpProgressPage->search();
    }
  }
}

//received from wizard page when shp is done
void OmgDataFetcherWizard::shapefileCreated( QString theShapefileName )
{
  //for parent app e.g. QGIS to act apon
  emit shapefileReady( theShapefileName );
}
