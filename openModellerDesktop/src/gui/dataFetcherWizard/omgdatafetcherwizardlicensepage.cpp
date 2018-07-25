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

#include <omgdatafetcherwizardlicensepage.h>
#include <omgdatafetcherwizardlicensewidget.h>
//
// QT includes
//
#include <QVBoxLayout>
  OmgDataFetcherWizardLicensePage::OmgDataFetcherWizardLicensePage(QWidget *parent)
: QWizardPage(parent)
{
  setTitle(tr("License Agreement"));
  setSubTitle(tr("The data obtained using this wizard is "
        "available under different license agreements. You "
        "must agree to the licenses below before you can "
        "continue to search for occurrence data."));
  OmgDataFetcherWizardLicenseWidget * mypLicenseWidget = 
    new OmgDataFetcherWizardLicenseWidget();
  QVBoxLayout *mypLayout = new QVBoxLayout;
  mypLayout->addWidget(mypLicenseWidget);
  setLayout(mypLayout);

  registerField("cbxLicenseAccepted*", mypLicenseWidget->licenseCheckBox());
}

