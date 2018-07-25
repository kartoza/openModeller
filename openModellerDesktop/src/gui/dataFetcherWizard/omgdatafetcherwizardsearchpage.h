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

#ifndef OMGDATAFETCHERWIZARDSEARCHPAGE_H
#define OMGDATAFETCHERWIZARDSEARCHPAGE_H
#include <omgdatafetcherwizardsearchwidget.h>
//
// QT includes
//
#include <QWizard>
/** \ingroup gui
 * \brief The second page of a wizard that allows the 
 * user to find occurrence data from online databases.
 * This page asks the user for search parameters..
 * @author Tim Sutton
 * */
class OmgDataFetcherWizardSearchPage : public QWizardPage
{
  Q_OBJECT;
  public:
  OmgDataFetcherWizardSearchPage(QWidget *parent);
  QListWidget * getTaxonListWidget();
  public slots:
  void checkComplete();
  //reimplement from wizard page base class
  bool isComplete() const;
  private:
    OmgDataFetcherWizardSearchWidget * mpSearchWidget;
};
#endif

