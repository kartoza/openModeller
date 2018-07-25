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

#include "omgrasterthreshold.h"
#include "omglayerselector.h"

#include <omgclimatefilereader.h>
#include <omgfilewriter.h>
#include <../lib/omgclimatedataprocessor.h>
#include <omgclimatefilegroup.h>
#include <omggdal.h>
#include <omgui.h>

//qt includes
#include <QSettings>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QCheckBox>


OmgRasterThreshold::OmgRasterThreshold(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  readSettings();

  connect(&mOmgGdal, SIGNAL(error(QString)), this, SLOT(error(QString)));
  connect(&mOmgGdal, SIGNAL(updateProgress(int, int )), this, SLOT(updateProgress(int, int )));
  //connect(&mOmgGdal, SIGNAL(contourComplete()), this, SLOT(contourComplete()));
  // default sort mode
  //mSortMode=BY_TAXON; //or BY_ALGORITHM
  mSortMode = BY_ALGORITHM;
  treeExperiment->header()->resizeSection(0,treeExperiment->width());
}

OmgRasterThreshold::~OmgRasterThreshold()
{
}

void OmgRasterThreshold::resizeEvent ( QResizeEvent * theEvent )
{
  treeExperiment->header()->resizeSection(0,treeExperiment->width());
}

void OmgRasterThreshold::setExperiment(OmgExperiment * const thepExperiment)
{
  mpExperiment=thepExperiment;
  populateTree();
}

void OmgRasterThreshold::populateTree()
{
  if (!mpExperiment)
  {
    QMessageBox::warning( this,tr("No experiment"),
        tr("Please open an experiment before using this tool"));
    return ;
  }
  treeExperiment->clear();
  int myCount = mpExperiment->count();
  //make a node for the experiment
  QTreeWidgetItem * mypExperimentItem = new QTreeWidgetItem(treeExperiment);
  mypExperimentItem->setText(0,tr("Experiment"));
  QIcon myExperimentIcon;
  myExperimentIcon.addFile(":/filenewExperiment.png");
  mypExperimentItem->setIcon(0,myExperimentIcon);
  treeExperiment->setItemExpanded(mypExperimentItem,true);
  //select the experiemnt as active item 
  treeExperiment->setItemSelected(mypExperimentItem,true);
  //
  //make a parent node for each alg type or taxon (depending on sort order)
  //
  QMap <QString,QTreeWidgetItem *> myParentMap;
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      continue;
    }
    QString myName;
    QString myParentType;
    QString myIconFileName;
    if (mSortMode==BY_TAXON)
    {
      myName = mypModel->taxonName();
      myParentType = "Taxon";
      myIconFileName = ":/taxon.png";
    }
    else //BY_ALGORITHM
    {
      myName = mypModel->algorithm().name();
      myParentType = "Algorithm";
      //myIconFileName = ":/algorithmmanager.png";
    }
    if (!myParentMap.contains(myName))
    {
      QTreeWidgetItem * mypParentItem = new QTreeWidgetItem(mypExperimentItem);
      mypParentItem->setText(0,myName);
      QIcon myParentIcon;
      myParentIcon.addFile(myIconFileName);
      mypParentItem->setIcon(0,myParentIcon);
      myParentMap[myName]=mypParentItem;
      //used later to detect if an alg/taxon node was clicked
      mypParentItem->setData(0,Qt::UserRole,myParentType); 
      
      //expand the tree
      treeExperiment->setItemExpanded(mypParentItem,true);
    }
  }
  //
  // Loop through all models now, adding a nore for each
  // to the appropriate parent.
  //
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      continue;
    }
    //determine the correct parent for this model in the tree
    QTreeWidgetItem * mypParentItem;
    QString myItemText;
    if (mSortMode==BY_TAXON)
    {
      mypParentItem = myParentMap.value(mypModel->taxonName());
      myItemText = mypModel->algorithm().name();
    }
    else // BY_ALGORITHM
    {
      mypParentItem = myParentMap.value(mypModel->algorithm().name());
      myItemText = mypModel->taxonName();
    }
    //now the model - only add it if its completed
    if (mypModel->isCompleted())
    {
      QListIterator<OmgProjection *> myIterator(mypModel->projectionList());
      while (myIterator.hasNext())
      {
        OmgProjection * mypProjection = myIterator.next();
        QTreeWidgetItem * mypItem = new QTreeWidgetItem(mypParentItem);
        mypItem->setText(0,myItemText + " - " + mypProjection->layerSet().name());
        QIcon myIcon;
        myIcon.addFile(mypProjection->workDir() + mypProjection->thumbnailFileName());
        mypItem->setIcon(0,myIcon);
        mypItem->setData(0,Qt::UserRole,mypProjection->guid());
      }
    }
  }
  treeExperiment->sortItems (0, Qt::AscendingOrder );
}

void OmgRasterThreshold::on_cboSortBy_currentIndexChanged(QString theString)
{
  if (theString==tr("Algorithm"))
  {
    mSortMode= BY_ALGORITHM;
  }
  else
  {
    mSortMode= BY_TAXON;
  }
  populateTree();
}

void OmgRasterThreshold::on_pbnThresholdOutputDir_clicked()
{
  QSettings mySettings;
  QString myLastDir = mySettings.value("rasterthreshold/thresholdOutputDirectory","/tmp").toString(); //initial dir
  qDebug ("Last Dir: " + QString(myLastDir).toLocal8Bit());
  QString myDirName = QFileDialog::getExistingDirectory(
                        this,
                        "Choose a directory to save outputs to",
                        myLastDir
                      );
  if (!myDirName.isEmpty())
  {
    mySettings.setValue("rasterthreshold/thresholdOutputDirectory",myDirName);
    leThresholdOutputDir->setText(myDirName);
  }
}

void OmgRasterThreshold::on_pbnSelectThresholdAggregateFile_clicked()
{
  QSettings mySettings;
  QString myLastFile = mySettings.value("rasterthreshold/aggregateFile","/tmp").toString();
  QFileInfo myFileInfo(myLastFile);
  QString myDir = myFileInfo.dir().path();
  qDebug ("Last Dir: " + QString(myDir).toLocal8Bit());
  QString myFileName = QFileDialog::getSaveFileName(
                         this,
                         "Choose a filename to aggregate file as",
                         myDir,
                         "Ascii Grid File (*.asc)");
  if (!myFileName.isEmpty())
  {
    mySettings.setValue("rasterthreshold/aggregateFile",myFileName);
    leThresholdAggregateFile->setText(myFileName);
  }
}




void OmgRasterThreshold::accept()
{
  // Create hotspot map - where each cell is the sum of
  // all the thresholded input layers for that cell
  writeSettings();
  // Whether to save out each intermediate thresholded file
  bool myIntermediatesFlag = cbxCreateThresholdFiles->isChecked();
  // use the first rasters nulls as a test for invalid rasters
  // if a subsequent raster has nulls in a different place
  // we want to flag that raster as bad / corrupt
  //bool myNullsMustMatchFlag=true;
  
  int myThresholdPercent=spinThreshold->value();
  qDebug("Threshold set to " + QString::number(myThresholdPercent).toLocal8Bit());
  // Get a count of selected models
  // we conly consider items that are not 'Algorithm'
  // 'Experiment' or 'Taxon' to avoid non model items
  QStringList myList;
  QListIterator<QTreeWidgetItem*> myIterator(treeExperiment->selectedItems());
  while (myIterator.hasNext())
  {
    QString myId = myIterator.next()->data(0,Qt::UserRole).toString();
    if (!myId.isEmpty() && myId!=(tr("Algorithm")) && 
        myId!=(tr("Taxon")) && myId!=(tr("Experiment")))
    {
      myList << myId;
    }
  }

  //
  // Check we have at least one layer
  //

  if (myList.count() < 1)
  {
    QMessageBox::warning( this,tr("No Inputs Defined"),
        tr("Please select one or more model rasters in the experiment tree"));
    return;
  }

  if (myIntermediatesFlag && leThresholdOutputDir->text().isEmpty())
  {
    QMessageBox::warning( this,tr("Output directory not set"),
        tr("Please set the output directory before clicking Process"));
    return;
  }
  if ( leThresholdAggregateFile->text().isEmpty())
  {
    QMessageBox::warning( this,tr("Output aggregate file not set"),
        tr("Please set the output file for the hotspot map before clicking Process"));
    return;
  }
  //
  // Check if the user wants to overwrite existing file
  //
  QFileInfo myInfo(leThresholdAggregateFile->text());
  if (myInfo.exists())
  {
    QMessageBox myMessage;
    myMessage.setText(tr("The threshold file already exists."
          "Would you like to overwrite it?"));
    myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (myMessage.exec()) {
      case QMessageBox::No:
        // no was clicked
        return;
        break;
      case QMessageBox::Yes:
        // do nothing
        break;
      default:
        // should never be reached
        break;
    }

  }
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  OmgProjection const * mypFirstProjection = mpExperiment->getProjection( myList.at(0) );
  QString myFirstFile(mypFirstProjection->workDir() + mypFirstProjection->rawImageFileName());
  qDebug("File Name" + myFirstFile.toLocal8Bit());
  QString myHeader = mOmgGdal.getAsciiHeader(myFirstFile);
  //get the dimensions of the first file - we assume all files have same dimensions!
  OmgClimateFileReader myFirstFileReader(myFirstFile,OmgClimateFileReader::GDAL);
  int myXDim = myFirstFileReader.xDim();
  int myYDim = myFirstFileReader.yDim();
  int myArrayLength=myYDim * myXDim;

  // NOTE: declaring an array dynamically like this: int myThresholdArray[myYDim][myXDim];
  // is illegal in C++ and will cause segfaults etc, so we allocate our memory like this:
  int * myThresholdArray = new int [myArrayLength];

  float myOutputNoData=-9999; //for the aggregate and threshold output files
  QString myFormat = mypFirstProjection->omRasterFormat();
  float myInputNoData = Omgui::getOutputFormatNoData().value(myFormat);

  OmgClimateDataProcessor myDataProcessor(myInputNoData);
  OmgFileWriter * myFileWriter=0;
  //initialise the aggegate matrix because who knows what default values it has
  //NOTE we dont initialise to nodata - only when we encounter a cell with nodata
  //does the aggregate value for that cell become nodata
  //memset (myAggregateArray,0,myYDim*myXDim);
  memset (myThresholdArray,0,myYDim*myXDim);
  
  // Iterate through file list and process
  for ( int myFileInt = 0; myFileInt < myList.count(); myFileInt++ )
  {
    OmgProjection const * mypProjection = mpExperiment->getProjection( myList.at(myFileInt) );
    //calculate teh actual threshold value given the file format
    //for this model and the required threshold percent
    myFormat = mypProjection->omRasterFormat();
    myInputNoData = Omgui::getOutputFormatNoData().value(myFormat);
    float myFormatMinimum = Omgui::getOutputFormatRanges().value(myFormat).first;
    float myFormatMaximum = Omgui::getOutputFormatRanges().value(myFormat).second;
    int myThresholdPercent = spinThreshold->value();
    float myThreshold = myFormatMinimum + 
      (((myFormatMaximum - myFormatMinimum)/100) * myThresholdPercent);
    QString myFormula("Threshold converted to raster range:\n " + 
        QString::number(myFormatMaximum) +
        "+ (((" + QString::number(myFormatMaximum) + 
        " - " + QString::number(myFormatMinimum) +
        ")/100) * " + QString::number(myThresholdPercent) + ")");
    qDebug(myFormula.toLocal8Bit());
    qDebug("Threshold set to " + QString::number(myThreshold).toLocal8Bit());
    QString myFileName(mypProjection->workDir() + mypProjection->rawImageFileName());
    //
    OmgClimateFileReader myFileReader(myFileName,OmgClimateFileReader::GDAL);
    if (myIntermediatesFlag)
    {
      QFileInfo myFileInfo(myFileName);
      QString myBaseString = leThresholdOutputDir->text()+QString("/")+myFileInfo.baseName();  // excludes any extension
      QString myOutFileName(myBaseString+"_Threshold_" + 
          QString::number(spinThreshold->value()) +".asc");
      myFileWriter = new OmgFileWriter(myOutFileName,OmgFileWriter::ESRI_ASCII);
      myFileWriter->writeString(myHeader);
    }
    //
    while (!myFileReader.isAtMatrixEnd())
    {
      //threshold will return 1, 0 or the output nodata value
      float myElement = myFileReader.getElement();
      int myThresholdedElement = static_cast<int>(myDataProcessor.threshold(myElement,myThreshold));
      //write this element to the individual threshold file for a species
      if (myIntermediatesFlag)
      {
        if (myFileReader.currentCol()==0 && myFileReader.currentRow()>0)
        {
          myFileWriter->sendLineBreak();
        }
        myFileWriter->writeElement(myThresholdedElement);
      }
      // write it to the threshold array
      int y=myFileReader.currentRow();
      int x=myFileReader.currentCol();
      int myPos=(y*myXDim)+x;
      int myThresholdValue=myThresholdArray[myPos];
      //if the output matrix is already marked as nodata dont bother trying to change it!
      if (myOutputNoData!=myThresholdValue)
      {
        //any time we get a nodata back from threshold, that cell should always be marked nodata!

        if (myInputNoData==myElement)
        {
          myThresholdArray[myPos]=static_cast<int>(myOutputNoData);
        }
        else
        {
          myThresholdArray[myPos]=static_cast<int>(myThresholdValue+myThresholdedElement);
        }
      }
    }
    qDebug("Written rows: " + QString::number(myFileReader.currentRow()));
    qDebug("Written cols: " + QString::number(myFileReader.currentCol()));
    emit updateProgress(myFileInt,myList.count());
    if (myIntermediatesFlag)
    {
      OmgGdal myGdal;
      QFileInfo myFileInfo(myFileWriter->fileName());
      //get the raw name but without its extension
      QString myBaseName=myFileInfo.baseName();
      QString myImageFileName = myBaseName+".png";
      //convert asc generated by threshold to pseudocolor png
      //qDebug("Calling omggdal::writeImage(" +  mypModel->rawImageFileName().toLocal8Bit() + ","
      //+ myImageFileName.toLocal8Bit()+")");
      QString myPreviewFile(mypProjection->workDir() + myBaseName + ".png");
      myGdal.writeImage(myFileWriter->fileName(),myPreviewFile, 300, 400);
      qDebug("Writing threshold preview to : " + myPreviewFile.toLocal8Bit());
      //lblPreview->setPixmap(QPixmap(myPreviewFile));

      myFileWriter->close();
      delete myFileWriter;
    }

  }
  //now write out the final threshold hotspot matrix
  OmgFileWriter myHotspotWriter (leThresholdAggregateFile->text(),OmgFileWriter::ESRI_ASCII);
  // make sure the header for the generated files has our output no data
  myHeader.replace(QString::number(myInputNoData),QString::number(myOutputNoData));;
  myHotspotWriter.writeString(myHeader);
  for (int y=0;y<myYDim;y++)
  {
    for (int x=0;x<myXDim;x++)
    {
      int myPos=(y*myXDim)+x;
      myHotspotWriter.writeElement(myThresholdArray[myPos]);
    }
    myHotspotWriter.sendLineBreak();
  }
  myHotspotWriter.close();

  delete[] myThresholdArray;
  progressBar->reset();
  QApplication::restoreOverrideCursor();
  //done(1);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  QMessageBox::information( this,QString(tr("Threshold result")),QString(tr("Thresholding completed successfully.")));
  close();

}

void OmgRasterThreshold::readSettings()
{
  QSettings mySettings;
  spinThreshold->setValue(mySettings.value("rasterthreshold/thresholdLevel",70).toInt());
  cbxCreateThresholdFiles->setChecked(mySettings.value("rasterthreshold/createThresholdFiles","0").toBool());
  leThresholdOutputDir->setText(mySettings.value("rasterthreshold/thresholdOutputDir","").toString());

  leThresholdAggregateFile->setText(mySettings.value("rasterthreshold/thresholdAggregateFile",".").toString());


}

void OmgRasterThreshold::writeSettings()
{
  QSettings mySettings;


  mySettings.setValue("rasterthreshold/thresholdLevel",spinThreshold->value());
  mySettings.setValue("rasterthreshold/createThresholdFiles",cbxCreateThresholdFiles->isChecked());
  mySettings.setValue("rasterthreshold/thresholdOutputDir",leThresholdOutputDir->text());
  mySettings.setValue("rasterthreshold/thresholdAggregateFile",leThresholdAggregateFile->text());

}

void OmgRasterThreshold::error(QString theError)
{
  QMessageBox::warning( this,QString(tr("File Format Converter Error")),theError);
}


void OmgRasterThreshold::updateProgress (int theValue, int theMaximumValue)
{
  progressBar->setMaximum(theMaximumValue);
  progressBar->setValue(theValue);
  QApplication::processEvents();
}
