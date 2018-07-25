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

#include <omgui.h> //ancilliary helper functions
#include <omgdatafetcherwizardprogresswidget.h>
#include <QSettings>
#include <QStringList>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
OmgDataFetcherWizardProgressWidget::OmgDataFetcherWizardProgressWidget(QWidget* parent , Qt::WFlags fl )
{
  setupUi(this);
}
OmgDataFetcherWizardProgressWidget::~OmgDataFetcherWizardProgressWidget()
{

}
QTextEdit * OmgDataFetcherWizardProgressWidget::getTeResults() 
{ 
  return teResults;
}
QProgressBar * OmgDataFetcherWizardProgressWidget::getProgressBar() 
{ 
  return progressBar; 
}
QLabel * OmgDataFetcherWizardProgressWidget::getLblStatus() 
{ 
  return lblStatus; 
}


