/***************************************************************************
 *   Copyright (C) 2005 by Peter Brewer   *
 *   peter@pembshideaway.co.uk   *
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
#include "omglayersetmanager.h"
#include <omglayerset.h>
#include <omglayerselector.h>

//qt includes
#include <QSettings>
#include <QTextEdit>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QListWidgetItem>
OmgLayerSetManager::OmgLayerSetManager(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  connect(tblLayerSets, SIGNAL(cellClicked( int,int)), this, SLOT(cellClicked( int,int)));
  refreshLayerSetTable();
  //disable these buttons unless experimental is allowed
#ifdef OMG_NO_EXPERIMENTAL 
  pbnImport->setVisible(false);
  pbnExport->setVisible(false);
#endif
}

OmgLayerSetManager::~OmgLayerSetManager()
{}

void OmgLayerSetManager::refreshLayerSetTable(QString theGuid)
{

  mLayerSetMap.clear();
  tblLayerSets->clear();
  tblLayerSets->setRowCount(0);
  tblLayerSets->setColumnCount(2);


  //we do this in two passes
  //in the first pass we populate a qmap with all the layersets
  //we find....
  mLayerSetMap = Omgui::getAvailableLayerSets();

  //the second pass populates the table
  //doing it from the map ensures that the rows
  //are sorted by layerset name

  int mySelectedRow=0;
  int myCurrentRow=0;
  QMapIterator<QString, OmgLayerSet> myIterator(mLayerSetMap);
  while (myIterator.hasNext()) 
  {
    myIterator.next();
    OmgLayerSet myLayerSet = myIterator.value();
    if (theGuid.isEmpty())
    {
      theGuid=myLayerSet.guid();
    }
    if (myLayerSet.guid()==theGuid)
    {
      mySelectedRow=myCurrentRow;
    }
    // Insert new row ready to fill with details
    tblLayerSets->insertRow(myCurrentRow); 
    QString myGuid = myLayerSet.guid();
    // Add details to the new row
    QTableWidgetItem *mypFileNameItem= new QTableWidgetItem(myGuid);
    tblLayerSets->setItem(myCurrentRow, 0, mypFileNameItem);
    QTableWidgetItem *mypNameItem = new QTableWidgetItem(myLayerSet.name());
    tblLayerSets->setItem(myCurrentRow, 1, mypNameItem);
    //display an icon indicating if the layerset is local or remote (e.g. terralib)
    //OmgLayerSet::Origin myOrigin = myLayerSet.origin();
    //if (myOrigin==OmgLayerSet::USERPROFILE)
    //{
    QIcon myIcon;
    myIcon.addFile(":/localdata.png");
    mypNameItem->setIcon(myIcon);
    //}
    //else if (myOrigin==OmgLayerSet::ADAPTERPROFILE)
    //{
    //QIcon myIcon;
    //myIcon.addFile(":/remotedata.png");
    //mypNameItem->setIcon(myIcon);
    //}
    //else if (myOrigin==OmgLayerSet::UNDEFINED)
    //{
    //  mypNameItem->setTextColor(Qt::yellow);
    //}
    myCurrentRow++;
  }

  if (myCurrentRow>0)
  {
    tblLayerSets->setCurrentCell(mySelectedRow,1);
    cellClicked(mySelectedRow,1);
  }
  QStringList headerLabels;
  headerLabels << "File Name" << "Name";
  tblLayerSets->setHorizontalHeaderLabels(headerLabels);
  tblLayerSets->setColumnWidth(0,0);
  tblLayerSets->setColumnWidth(1,tblLayerSets->width());
  tblLayerSets->horizontalHeader()->hide();
  tblLayerSets->verticalHeader()->hide();
  tblLayerSets->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  if (mLayerSetMap.count()<1)
  {
    grpLayerset->setEnabled(false);
    leName->setText("");
    leDescription->setText("");
    lstLayers->clear();
    cboMask->clear();
  }
  else
  {
    grpLayerset->setEnabled(true);
  }
  lblLayers->setText(tr("Layers: ") + "("+QString::number(lstLayers->count())+")");
}

void OmgLayerSetManager::cellClicked(int theRow, int theColumn)
{
  //note we use the alg name not the id becuase user may have customised params
  //qDebug("OmgLayerSetManager::cellClicked");
  QString myGuid = tblLayerSets->item(tblLayerSets->currentRow(),0)->text();
  //qDebug("Guid is: " + myGuid.toLocal8Bit());
  QString myFileName = myGuid + ".xml";
  selectLayerSet(myFileName);
}
void OmgLayerSetManager::selectLayerSet(QString theFileName)
{
  QString myLayerSetDir = Omgui::userLayersetDirPath();
  OmgLayerSet myLayerSet;
  myLayerSet.fromXmlFile(myLayerSetDir + QDir::separator() + theFileName);
  leName->setText(myLayerSet.name());
  leDescription->setText(myLayerSet.description());
  //qDebug("Clearing Layers and mask");
  lstLayers->clear();
  cboMask->clear();
  //qDebug("Layers and mask cleared");
  int myMaskItemNo = -1;
  OmgLayerSet::LayersMap::iterator myIterator;
  OmgLayerSet::LayersMap myLayers = myLayerSet.layers();
  for (myIterator = myLayers.begin(); myIterator != myLayers.end(); ++myIterator)
  {
    OmgLayer myLayer = myIterator.value();
    QListWidgetItem *mypItem = new QListWidgetItem(myLayer.name(),lstLayers);
    if (myLayer.isCategorical())
    {
      QIcon myIcon;
      myIcon.addFile(":/categorical.png");
      mypItem->setIcon(myIcon);
    }
    else 
    {
      QIcon myIcon;
      myIcon.addFile(":/non-categorical.png");
      mypItem->setIcon(myIcon);
    }
    lstLayers->addItem(mypItem);
    cboMask->addItem(myLayer.name());
    if (myLayer.name()==myLayerSet.mask().name())
    {
      //qDebug("Mask match found: " + myLayer.name().toLocal8Bit() +  " == " + myLayerSet.mask().name().toLocal8Bit());
      myMaskItemNo=cboMask->count();
    }
  }
  if (!myLayerSet.mask().name().isEmpty() && myMaskItemNo < 0 )
  {
    cboMask->addItem(myLayerSet.mask().name());
    myMaskItemNo=cboMask->count();
  }
  //qDebug("Setting current item for mask to: " + QString::number(myMaskItemNo).toLocal8Bit());
  cboMask->setCurrentItem(myMaskItemNo-1);
  mLayerSet=myLayerSet;
  lblLayers->setText(tr("Layers: ") + "("+QString::number(lstLayers->count())+")");
}

void OmgLayerSetManager::on_pbnImport_clicked()
{
  QString myFileName = QFileDialog::getOpenFileName(
          this,
          "Choose a file",
          ".",
          "Models and Environments (*.xml)");
  if (!myFileName.isEmpty())
  {
    OmgLayerSet myLayerSet;
    if (myLayerSet.fromXmlFile(myFileName))
    {
      myLayerSet.setGuid();
      if (myLayerSet.name().isEmpty())
      {
        myLayerSet.setName(tr("Unnamed layerset"));
      }
      if (myLayerSet.description().isEmpty())
      {
        myLayerSet.setDescription(tr("No description"));
      }
      myLayerSet.save();
      refreshLayerSetTable();
    }
  }
}

void OmgLayerSetManager::on_toolNew_clicked()
{
  OmgLayerSet myLayerSet;
  leName->clear();
  leDescription->clear();
  lstLayers->clear();
  cboMask->clear();
  myLayerSet.setGuid();
  mLayerSet = myLayerSet;
  lblLayers->setText(tr("Layers: ") + "(0)");
}

void OmgLayerSetManager::resizeEvent ( QResizeEvent * theEvent )
{
  tblLayerSets->setColumnWidth(0,0);
  tblLayerSets->setColumnWidth(1,tblLayerSets->width());
  tblLayerSets->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
}

void OmgLayerSetManager::on_toolCopy_clicked()
{
  if (tblLayerSets->currentRow() < 0) 
  {
    return;
  }
  //to clone, we get the algorithm guid that is currently selected
  QString myGuid = tblLayerSets->item(tblLayerSets->currentRow(),0)->text();
  if (myGuid.isEmpty())
  {
    return;
  }
  QString myOriginalFileName = Omgui::userLayersetDirPath() + QDir::separator() + myGuid + ".xml";
  OmgLayerSet myLayerSet;
  myLayerSet.fromXmlFile(myOriginalFileName);
  /*
  int myCount = 1;
  while (mLayerSet.contains(myProfileName))
  {
    myProfileName = tr("Copy ") + QString::number(myCount++) + " of " + myLayerSet.name();
  }
  */
  //assign this layerset its own guid
  myLayerSet.setGuid();
  myLayerSet.setName(tr("Copy of ") + myLayerSet.name());
  myLayerSet.save();
  refreshLayerSetTable(myLayerSet.guid());
}
void OmgLayerSetManager::on_toolDelete_clicked()
{
  if (tblLayerSets->currentRow() < 0) 
  {
    return;
  }
  if ( QMessageBox::warning(this, "openModeller Desktop",
                    tr("Deleting a layer set is non-reversable.\n"
                       "Do you really want to delete it?"),
                       QMessageBox::Yes | QMessageBox::No,
                       QMessageBox::No) == QMessageBox::No)
  {
    //user said no
    return;
  }
  QString myGuid = tblLayerSets->item(tblLayerSets->currentRow(),0)->text();
  if (!myGuid.isEmpty())
  {
    QFile myFile(Omgui::userLayersetDirPath() + QDir::separator() + myGuid + ".xml");
    if (!myFile.remove())
    {
      QMessageBox::warning(this, tr("openModeller Desktop"),
      tr("Unable to delete file \n") + myFile.fileName());
    }
    refreshLayerSetTable();
  }
}
void OmgLayerSetManager::on_pbnApply_clicked()
{
  if (lstLayers->count()==0)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),tr("There are no layers in your layerset. Please add some layers before pressing apply."));
    return;
  }
  if (leName->text().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
      tr("Please specify a layerset name before pressing apply."));
    return;
  }
  //
  // first check if the user is creating a layerset that duplicates anothers name
  //
  QMapIterator<QString, OmgLayerSet> myIterator(mLayerSetMap);
  while (myIterator.hasNext()) 
  {
    myIterator.next();
    OmgLayerSet myLayerSet = myIterator.value();
    if ((myLayerSet.name()==leName->text()) && (myLayerSet.guid()!=mLayerSet.guid()))
    {
      QMessageBox::warning( this,tr("openModeller Desktop"),tr("The name you choose for your layerset must be unique.\n Update it and try again."));
      return;
    }
  }
  //work on temp copy & replace mLayerSet at last minute only !! (following mayer effective c++ tip here..)
  OmgLayerSet myLayerSet;
  myLayerSet.setGuid(mLayerSet.guid());
  myLayerSet.setName(leName->text());
  myLayerSet.setDescription(leDescription->text());
  OmgLayer myMask;
  myMask.setName(cboMask->currentText());
  myLayerSet.setMask(myMask);

  //!@TODO Handle checking for categorical data...
  for ( int myCounter = 0; myCounter < lstLayers->count(); myCounter++ )
  {
    QString myLayerName=lstLayers->item(myCounter)->text();
    OmgLayer myLayer;
    myLayer.setName(myLayerName);
    myLayer.setType(OmgLayer::MAP);
    myLayer.setCategorical(false);
    myLayerSet.addLayer(myLayer);
  }
  myLayerSet.save();
  mLayerSet=myLayerSet;
  refreshLayerSetTable(mLayerSet.guid());
}

void OmgLayerSetManager::on_toolDeleteLayers_clicked()
{
  unsigned int myLayersCount = static_cast<unsigned int>(lstLayers->count());
  for ( unsigned int myInt = 0; myInt < myLayersCount; myInt++ )
  {
    QListWidgetItem *myItem = lstLayers->item( myInt );
    // if the item is selected...
    if ( lstLayers->isItemSelected(myItem) )
    {
      //remove the item if it is selected
      //this method is described in the qt docs but gives a compile errror saying the error is non existant!
      //lstLayers->removeItem(myInt);
      //so we kludge it for now!
      lstLayers->takeItem(myInt);
      //also remove the item from the mask layer combo
      //cboInputMaskLayer->removeItem(myInt);
      myInt--;
      myLayersCount--;
    }
  }
  lblLayers->setText(tr("Layers: ") + "("+QString::number(lstLayers->count())+")");
} //on_toolDeleteLayers_clicked
void OmgLayerSetManager::on_toolAddLayers_clicked()
{
  QSettings mySettings;
  QString myBaseDir = mySettings.value("openModeller/layerSetManager/modelLayersDirectory","c:/tmp").toString(); //initial dir
  OmgLayerSelector myOmgLayerSelector(myBaseDir, this);
  if(myOmgLayerSelector.exec())
  {
    QStringList myList=myOmgLayerSelector.getSelectedLayers();
    if (myList.size()<1)
    {
      return;
    }

    //sort on descending alphabetical order
    myList = Omgui::sortList(myList);
    QStringList::Iterator myIterator= myList.begin();
    while( myIterator!= myList.end() )
    {
      //make sure this layer is not already in the list
      QString myString = *myIterator;
      QList<QListWidgetItem*> myList = lstLayers->findItems(myString, Qt::MatchExactly);
      if (myList.size()<1)
      {
        lstLayers->insertItem( 0, myString );
        cboMask->addItem(myString);
      }
      ++myIterator;
    }
    mySettings.setValue("openModeller/layerSetManager/modelLayersDirectory",myOmgLayerSelector.getBaseDir());
    lblLayers->setText(tr("Layers: ") + "("+QString::number(lstLayers->count())+")");
  }
}//tool_addLayers_clicked

void OmgLayerSetManager::on_toolNewMaskLayer_clicked()
{
  QSettings mySettings;
  QString myBaseDir = mySettings.value("openModeller/layerSetManager/modelMasksDirectory","c:/tmp").toString(); //initial dir
  OmgLayerSelector myOmgLayerSelector(myBaseDir, this);
  myOmgLayerSelector.setSelectionMode(QAbstractItemView::SingleSelection);
  if(myOmgLayerSelector.exec())
  {
    QStringList myList=myOmgLayerSelector.getSelectedLayers();
    if (myList.size()<1)
    {
      return;
    }
    cboMask->addItem(myList[0]);
    int myNewIndex = cboMask->count()-1;
    if (myNewIndex >= 0)
    {
      cboMask->setCurrentIndex(myNewIndex);
    }
    mySettings.setValue("openModeller/layerSetManager/modelMasksDirectory",myOmgLayerSelector.getBaseDir());
  }
}//on_toolNewMaskLayer_clicked()
