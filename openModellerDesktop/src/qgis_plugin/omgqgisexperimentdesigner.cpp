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
#include "omgqgisexperimentdesigner.h"
#include "omgexperiment.h"
#include "omglayerselector.h"
#include "omgalgorithmmanager.h"
#include <omgui.h> //provides anciliary helper functions like getting app paths
#include <omgmodellerpluginregistry.h>
#include <omglayerset.h>
#include <omgpluginmessenger.h>
//Qt includes 
#include <QDate>
#include <QDir>
#include <QString>
#include <QPluginLoader>
#include <QFileDialog>
#include <QFileInfoList>
#include <QMessageBox>
#include <QSettings>
#include <QHash>
//std c++ stuff
#include <cassert>

OmgQGisExperimentDesigner::OmgQGisExperimentDesigner(QWidget* parent, Qt::WFlags fl)
    : OmgExperimentDesigner(parent,fl)
{
}


OmgQGisExperimentDesigner::~OmgQGisExperimentDesigner()
{
}

void OmgQGisExperimentDesigner::on_pbnOk_clicked()
{
  QSettings mySettings;

  //User data dir settings
  mySettings.setValue("dataDirs/dataDir", leDataDirectory->text());
  //output format
  mySettings.setValue("outputFormat",cboOutputFormat->itemData(cboOutputFormat->currentIndex()).toString());

  //first do some sanity checking....
  if (leExperimentName->text().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),tr("Please give this experiment a valid name!"));
    return;
  }

  QString myWorkDirName = QDir::convertSeparators(Omgui::getModelOutputDir()) 
    + QDir::separator() 
    + leExperimentName->text()
    + QDir::separator(); 
  QDir myWorkDir(myWorkDirName );
  if (myWorkDir.exists())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("An experiment directory with this name already exists!"));
    return;
  }
  if(omglistTaxa->checkedItemCount() < 1)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select one or more taxa before attempting to run the experiment"));
    return;
  }
  if (cboModelLayerSet->count()==0)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please create a layer set using the Layer Set Manager and "
          "then select one layer set for the model creation process "
          "before attempting to run the experiment"));
    return;
  }
  if (omglistProjectionLayerSets->count() < 1)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please create a layer set for model projection using the "
          "Layer Set Manager and then select one or more layer sets for the model "
          "projection process before attempting to run the experiment"));
    return;
  }
  if (cboOutputFormatLayer->currentText().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select an output format layer before attempting "
          "to run the model"));
    return;
  }
  //qDebug("Writing settings...");
  writeSettings();

  OmgExperiment * mypExperiment = getExperiment();
  //then ok is pressed in progress monitor we can close up shop here
  //we are going to pass the experiment to the main window. its the main windows job to delete it
  emit experimentCreated(mypExperiment);
  done(1);
}

