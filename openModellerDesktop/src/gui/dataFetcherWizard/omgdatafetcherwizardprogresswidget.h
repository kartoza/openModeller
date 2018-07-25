/***************************************************************************
 *   Copyright (C) 2007 by Tim Sutton tim@linfiniti.com   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02222-2307, USA.             *
 ***************************************************************************/

#ifndef OMGDATAFETCHERWIZARDPROGRESSWIDGET_H
#define OMGDATAFETCHERWIZARDPROGRESSWIDGET_H

#include <omgui.h> //ancilliary helper functions
#include <ui_omgdatafetcherwizardprogresswidgetbase.h>
#include <QSettings>
#include <QStringList>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
/** \ingroup gui
 * \brief A dialog to display information about the application.
 */
class OmgDataFetcherWizardProgressWidget : public QWidget, private Ui::OmgDataFetcherWizardProgressWidgetBase
{
  Q_OBJECT;

  public:
  OmgDataFetcherWizardProgressWidget(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgDataFetcherWizardProgressWidget();
  QTextEdit * getTeResults();
  QProgressBar * getProgressBar();
  QLabel * getLblStatus();
};

#endif

