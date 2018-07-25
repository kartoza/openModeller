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

#include <omglocalitiesoptions.h>
//qt includes
#include <QSettings>
#include <QString>
#include "qgsgenericprojectionselector.h"
#include "qgscoordinatereferencesystem.h"
/** A options form for the user to decide how they want to work with localities. */

OmgLocalitiesOptions::OmgLocalitiesOptions(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);

  QSettings mySettings;
  QString myLocalitiesOption = mySettings.value("localities/loaderType","FILE").toString();
  if (myLocalitiesOption ==  "FILE")
  {
      radSingleFile->setChecked(true);
  }
  else
  {
      radFilesInDirectory->setChecked(true);
  }

  QString myFilterType = 
    mySettings.value("localities/localitiesFilterType","NO_FILTER").toString();
  if (myFilterType=="ENVIRONMENTALLY_UNIQUE")
  {
    radEnvironmentallyUnique->setChecked(true);
  }
  else if (myFilterType=="SPATIALLY_UNIQUE")
  {
    radSpatiallyUnique->setChecked(true);
  }
  else //no filtering
  {
    radNoFilter->setChecked(true);
  }

  /* Set the training proportion default, min and max values. */
  
  // Get the training proportion value from registry.
  double trainingProportion = mySettings.value("localities/TrainingProportion").toDouble();
  
  if( trainingProportion < 1 )
  {
    // Use default value.
    dSBTrainingProportion->setValue( 100 );
  }
  else
  {
    // Use value from registry
    dSBTrainingProportion->setValue( trainingProportion );
  }

  // Set the training proportion min and max values.
  dSBTrainingProportion->setMaximum( 100.00 );
  dSBTrainingProportion->setMinimum( 1.0 );

  // Set decimal points.
  dSBTrainingProportion->setDecimals( 0 );
}

OmgLocalitiesOptions::~OmgLocalitiesOptions()
{
}
void OmgLocalitiesOptions::accept()
{
  QSettings mySettings;
  qDebug("Saving localities options");
  if (radSingleFile->isChecked())
  {
    mySettings.setValue("localities/loaderType", "FILE");
  }
  else // radFilesInDirectory->isChecked();
  {
    mySettings.setValue("localities/loaderType", "DIRECTORY");
  }


  QString myFilterType="NO_FILTER"; //default
  if (radEnvironmentallyUnique->isChecked())
  {
    myFilterType="ENVIRONMENTALLY_UNIQUE";
  }
  else if (radSpatiallyUnique->isChecked())
  {
    myFilterType="SPATIALLY_UNIQUE";
  }
  mySettings.setValue("localities/localitiesFilterType",myFilterType);

  // Save TrainingProportion in register. 
  mySettings.setValue("localities/TrainingProportion",dSBTrainingProportion->value());

  done(1);
}

double OmgLocalitiesOptions::getTrainingProportion()
{
	return dSBTrainingProportion->value();
}

void OmgLocalitiesOptions::hideIndependentTest()
{
  grpTests->hide();
}

void OmgLocalitiesOptions::on_pbnSetCrs_clicked()
{
  QSettings mySettings;
  QgsGenericProjectionSelector * mySelector = new QgsGenericProjectionSelector( this );
  mySelector->setMessage("Set the input Coordinate Reference System for your occurrences");
  //mySelector->setSelectedCrsId( layer->srs().srsid() );
  mySelector->setSelectedCrsId( 4326 );
  if ( mySelector->exec() )
  {
    QgsCoordinateReferenceSystem srs( mySelector->selectedCrsId(), QgsCoordinateReferenceSystem::InternalCrsId );
    mySettings.setValue("openModeller/experimentDesigner/coordSystemName",srs.description() );
    mySettings.setValue("openModeller/experimentDesigner/coordSystem",srs.toWkt() );
  }
  else
  {
    QApplication::restoreOverrideCursor();
  }
  delete mySelector;
}
