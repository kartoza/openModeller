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

//local includes
#include "omgpublishtocatalogue.h"
#include <omgmodel.h>
//qt includes
#include <QSettings>
OmgPublishToCatalogue::OmgPublishToCatalogue(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
}

OmgPublishToCatalogue::~OmgPublishToCatalogue()
{}




void OmgPublishToCatalogue::setModel(OmgModel * thepModel)
{
  mpModel = thepModel;
  lblName->setText(mpModel->taxonName());
  //QPixmap myPixmap(mpModel->workDir() + mpModel->thumbnailFileName());
  //lblImage->setPixmap(myPixmap);

}

void OmgPublishToCatalogue::accept()
{
}

