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

#include <omgdatafetcherwizardsearchpage.h>
#include <omgpluginmessenger.h>
#include <omgui.h> //ancilliary helper functions
//qt includes
#include <QSettings>

//
// QT includes
//
#include <QWizard>
#include <QVBoxLayout>
#include <QFile>


OmgDataFetcherWizardSearchPage::OmgDataFetcherWizardSearchPage(QWidget *parent)
: QWizardPage(parent),
  mpSearchWidget(new OmgDataFetcherWizardSearchWidget())
{
  setTitle(tr("Search Criteria"));
  setSubTitle(tr("Use this page to define the search criteria."));
  QVBoxLayout *mypLayout = new QVBoxLayout;
  mypLayout->addWidget(mpSearchWidget);
  setLayout(mypLayout);
  //*after name means its compulsory
  registerField("leOutputPath", mpSearchWidget->getLeOutputPath());
  connect(mpSearchWidget, SIGNAL(dataChanged()), 
      this, SLOT(checkComplete()));
}
QListWidget * OmgDataFetcherWizardSearchPage::getTaxonListWidget()
{
  return mpSearchWidget->getLstSearch();
}
void OmgDataFetcherWizardSearchPage::checkComplete() 
{
  //this is a QWizardPage specified signal...
  emit completeChanged();
}
//reimplement from wizard page basW class
bool OmgDataFetcherWizardSearchPage::isComplete() const
{
  qDebug("isComplete called");
  if (mpSearchWidget->getLstSearch()->count() > 0 &&
      QFile::exists(mpSearchWidget->getLeOutputPath()->text()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

