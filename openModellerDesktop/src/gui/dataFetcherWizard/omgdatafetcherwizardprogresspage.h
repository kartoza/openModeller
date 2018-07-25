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

#ifndef OMGDATAFETCHERWIZARDPROGRESSPAGE_H
#define OMGDATAFETCHERWIZARDPROGRESSPAGE_H

#include <omgdatafetcherwizardprogresswidget.h>
//
// QT includes
//
#include <QWizard>
#include <QListWidget>
/** \ingroup gui
 * \brief The third page of a wizard that allows the 
 * user to find occurrence data from online databases.
 * This page shows the user the progress of the search.
 * @author Tim Sutton
 * */
class OmgDataFetcherWizardProgressPage : public QWizardPage
{
  Q_OBJECT;
  public:
  OmgDataFetcherWizardProgressPage(QWidget *parent);
  void setTaxonListWidget (QListWidget * thepWidget);

  signals:
  void shapefileCreated( QString theShapefileName );

  public slots:
  void search() ;

  private slots:
  void fileWritten(QString theShapeFile, QString theTextFile,QString theTaxonName,int theCount);
  void fileNotWritten(QString theTaxonName);
  void showPoints(QString theFileName);
  void setStatus(QString theStatus) ;
  void setError(QString theError);
  private:
  OmgDataFetcherWizardProgressWidget * mpProgressWidget;  
  int mRunningCount;
  int mNamesWithDataCount;
  QListWidget * mpTaxonListWidget;
};
#endif
