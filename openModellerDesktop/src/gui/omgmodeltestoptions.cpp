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
#include "omgmodeltestoptions.h"

//qt includes
#include <QSettings>


OmgModelTestOptions::OmgModelTestOptions(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);

  // on_cbxLowestPrecenseThreshold_cliked
  connect(cbxLowestPrecenseThreshold, SIGNAL( stateChanged ( int )), this, SLOT( on_cbxLowestPrecenseThreshold_cliked( int )));

  // Get last user values
  QSettings mySettings;
  // dsbConfusionMatrixThrehold
  dsbConfusionMatrixThrehold->setValue(mySettings.value("ModelTestOptions/dsbConfusionMatrixThrehold", 50).toDouble());
  // cbxLowestPrecenseThreshold
  cbxLowestPrecenseThreshold->setChecked(mySettings.value("ModelTestOptions/cbxLowestPrecenseThreshold", false).toBool());
  // sbxBackgroundPoints
  sbxBackgroundPoints->setValue(mySettings.value("ModelTestOptions/sbxBackgroundPoints", 10000).toInt());
  // dsbMaximumOmission
  dsbMaximumOmission->setValue(mySettings.value("ModelTestOptions/dsbMaximumOmission", 100).toDouble());
}

OmgModelTestOptions::~OmgModelTestOptions()
{
}

void OmgModelTestOptions::accept()
{
  // Save user values
  QSettings mySettings;
  // dsbConfusionMatrixThrehold
  mySettings.setValue("ModelTestOptions/dsbConfusionMatrixThrehold",dsbConfusionMatrixThrehold->value());
  // cbxLowestPrecenseThreshold
  mySettings.setValue("ModelTestOptions/cbxLowestPrecenseThreshold",cbxLowestPrecenseThreshold->isChecked());
  // sbxBackgroundPoints
  mySettings.setValue("ModelTestOptions/sbxBackgroundPoints",sbxBackgroundPoints->value());
  // dsbMaximumOmission
  mySettings.setValue("ModelTestOptions/dsbMaximumOmission",dsbMaximumOmission->value());

  done(1);
}

void OmgModelTestOptions::on_cbxLowestPrecenseThreshold_cliked( int state )
{
  /* from qt4 documentation
  Qt::Unchecked         0 The item is unchecked.
  Qt::PartiallyChecked  1 The item is partially checked. Items in hierarchical models
                          may be partially checked if some, but not all, of their 
                          children are checked.
  Qt::Checked           2 The item is checked.
  */
  if( state == Qt::Checked)
  {
    lblThershold->setEnabled( false );
    dsbConfusionMatrixThrehold->setEnabled( false );
    lblThresholdPercent->setEnabled( false );
  }
  else if( state == Qt::Unchecked)
  {
    lblThershold->setEnabled( true );
    dsbConfusionMatrixThrehold->setEnabled( true );
    lblThresholdPercent->setEnabled( true );
  }
}


// get methods
double OmgModelTestOptions::getThreshold()
{
  return dsbConfusionMatrixThrehold->value();
}

bool OmgModelTestOptions::getUseLowestThreshold()
{
  return cbxLowestPrecenseThreshold->isChecked();
}

int OmgModelTestOptions::getNumBackgroundPoints()
{
  return sbxBackgroundPoints->value();
}

double OmgModelTestOptions::getMaxOmission()
{
  return dsbMaximumOmission->value();
}

void OmgModelTestOptions::hideLowestPrecenseThreshold()
{
  cbxLowestPrecenseThreshold->setChecked( false );
  cbxLowestPrecenseThreshold->hide();
}
