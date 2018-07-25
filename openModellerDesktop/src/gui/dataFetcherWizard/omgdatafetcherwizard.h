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

#ifndef OMGDATAFETCHERWIZARD_H
#define OMGDATAFETCHERWIZARD_H

#include <omgdatafetcherwizardlicensepage.h>
#include <omgdatafetcherwizardsearchpage.h>
#include <omgdatafetcherwizardprogresspage.h>

//
// QT includes
//
#include <QWizard>


/** \ingroup gui
 * \brief A wizard that allows the user to find occurrence data from online databases.
 * @author Tim Sutton
 * */

class OmgDataFetcherWizard : public QWizard
{
  Q_OBJECT;

  public:
  OmgDataFetcherWizard(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgDataFetcherWizard();

  signals:
  void shapefileReady( QString theShapefileName );

  public slots:
  void pageChanged ( int theId ); 
  void shapefileCreated( QString theShapefileName );

  private:
    OmgDataFetcherWizardLicensePage * mpLicensePage;
    OmgDataFetcherWizardSearchPage * mpSearchPage;
    OmgDataFetcherWizardProgressPage * mpProgressPage;
};

#endif

