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

#include "omgwizardpages.h"
#include "omgwizard.h" //for the enum
#include <omgui.h> //provides anciliary helper functions like getting app paths
#include "omglayerselector.h"
#include <QMessageBox>
#include <QList>
#include <QTextStream>
#include <QDir>
#include <QSettings>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>

OmgExperimentPage::OmgExperimentPage( ) : QWizardPage( ) 
{
  setupUi(this);
  registerField( "ExperimentName*", leExperimentName );
}


OmgExperimentPage::~OmgExperimentPage()
{
}

bool OmgExperimentPage::validatePage()
{
  QString myWorkDirName = QDir::convertSeparators(Omgui::getModelOutputDir()) 
    + QDir::separator() 
    + leExperimentName->text()
    + QDir::separator(); 
  QDir myWorkDir(myWorkDirName );
  if (myWorkDir.exists())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("An experiment directory with this name already exists!"));
    return false;
  }
  return true;
}




OmgOccurrencesPage::OmgOccurrencesPage( ) : QWizardPage( ) 
{
  setupUi(this);
  registerField( "UseOccurrencesFile", radUseOccurrencesFile );
  registerField( "OccurrencesQGISLayer",  lstPointLayers );
}


OmgOccurrencesPage::~OmgOccurrencesPage()
{
}

int OmgOccurrencesPage::nextId() const
{
  if (radUseOccurrencesLayer->isChecked()) 
  {
    return OmgWizard::FIELDS_PAGE;
  } 
  else 
  {
    return OmgWizard::TAXA_PAGE;
  }
}


//needed because isComplete request completeChanged signals manually emitted
void OmgOccurrencesPage::on_radUseOccurrencesLayer_toggled( bool theFlag )
{
  //QMessageBox::warning( this,tr("openModeller Desktop"),tr("layer checked" ) );
  emit completeChanged();
}

//needed because isComplete request completeChanged signals manually emitted
void OmgOccurrencesPage::on_radUseOccurrencesFile_toggled( bool theFlag )
{
  //QMessageBox::warning( this,tr("openModeller Desktop"),tr("layer file" ) );
  emit completeChanged();
}

//needed because isComplete request completeChanged signals manually emitted
void OmgOccurrencesPage::on_lstPointLayers_currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous )
{
  //QMessageBox::warning( this,tr("openModeller Desktop"),tr("Item changed" ) );
  emit completeChanged();
}

bool OmgOccurrencesPage::isComplete() const
{
  if (radUseOccurrencesLayer->isChecked()) 
  {
    if ( !lstPointLayers->currentItem() )
    {
      //QMessageBox::warning( NULL ,tr("openModeller Desktop"),tr("no items selected") );
      return false;
    }
  } 
  return true;
}

OmgFieldsPage::OmgFieldsPage( ) : QWizardPage( ) 
{
  setupUi(this);
  registerField( "XField", cboXField );
  registerField( "YField", cboYField );
  registerField( "TaxonField", cboTaxonField );
  registerField( "FrequencyField", cboFrequencyField );
}

OmgFieldsPage::~OmgFieldsPage()
{
}

void OmgFieldsPage::initializePage()
{
  cboFrequencyField->clear();
  cboXField->clear();
  cboYField->clear();
  cboFrequencyField->clear();
  //QMessageBox::warning( NULL ,tr("openModeller Desktop"),tr("Layer %1").arg( field("OccurrencesQGISLayer").toString() ) );
  OmgWizard * myWizard = qobject_cast<OmgWizard *>( wizard() );
  QgsVectorLayer * myLayer = myWizard->getOccurrenceLayer( field("OccurrencesQGISLayer").toInt() );
  cboFrequencyField->addItem( "[None]" );
  //disabling for now
  cboFrequencyField->setDisabled(true);
  //if its a point layer we will simply use the geometry from the shape
  if ( myLayer->geometryType() == QGis::Point )
  {
    cboXField->setEnabled( false );
    cboYField->setEnabled( false );
  }
  else
  {
    cboXField->setEnabled( true );
    cboYField->setEnabled( true );
  }
  QgsFieldMap myFields = myLayer->pendingFields();
  int myLatIndex = 0;
  int myLonIndex = 0;
  int myTaxonIndex = 0;
  int myPos = 0;
  for ( QgsFieldMap::const_iterator it = myFields.constBegin(); it != myFields.constEnd(); ++it )
  {
    QString myFieldName = it->name();
    cboXField->addItem( myFieldName );
    cboYField->addItem( myFieldName );
    cboTaxonField->addItem( myFieldName );
    cboFrequencyField->addItem( myFieldName );
    if ( myFieldName.toUpper() == "TAXONNAME" ) myTaxonIndex = myPos;
	if ( myFieldName.toUpper() == "TAXON" ) myTaxonIndex = myPos;
    if ( myFieldName.toUpper() == "LAT" || myFieldName.toUpper() == "Y" ) myLatIndex = myPos;
    if ( myFieldName.toUpper() == "LON" || myFieldName.toUpper() == "X" ) myLonIndex = myPos;
    ++myPos;
  }
  cboYField->setCurrentItem( myLatIndex );
  cboXField->setCurrentItem( myLonIndex );
  cboTaxonField->setCurrentItem( myTaxonIndex );
}


OmgTaxaPage::OmgTaxaPage( ) : QWizardPage( ) 
{
  setupUi(this);
}


OmgTaxaPage::~OmgTaxaPage()
{
}

void OmgTaxaPage::initializePage()
{
  QSettings mySettings;
  //QMessageBox::warning( this,tr("openModeller Desktop"),tr("Use Occurrences File value %1!").arg( field("UseOccurrencesFile").toString() ) );
  if ( field("UseOccurrencesFile").toBool() )
  {
    omglistTaxa->showOptionsTool();
    omglistTaxa->showAddItemTool();
    QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
    if (myLoaderType=="FILE") //read locs from a single file with many taxa
    {
      QString myFileName = mySettings.value("openModeller/experimentDesigner/localitiesFileName").toString();
      if (!myFileName.isEmpty())
      {
        setSpeciesList(myFileName);
      }
    }
    else //multiple files in a dir
    {
      QString myDirName = mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory").toString();
      if (!myDirName.isEmpty())
      {
        setSpeciesList(myDirName);
      }
    }
  }
  else
  {
    omglistTaxa->clear();
    omglistTaxa->hideOptionsTool();
    omglistTaxa->hideAddItemTool();
    OmgWizard * myWizard = qobject_cast<OmgWizard *>( wizard() );
    QgsVectorLayer * myLayer = myWizard->getOccurrenceLayer( field("OccurrencesQGISLayer").toInt() );
    QgsVectorDataProvider * mypDataProvider = myLayer->dataProvider();
    int myTaxonFieldIndex = field("TaxonField").toInt();
    QList< QVariant > myTaxaList;
    mypDataProvider->uniqueValues( myTaxonFieldIndex, myTaxaList );
    QList< QVariant >::iterator i;
    for (i = myTaxaList.begin(); i != myTaxaList.end(); ++i)
    {  
      QVariant myValue = *i;
      if ( myTaxaList.count() == 1 )
      {
        //tick by default since there is only one entry
        omglistTaxa->addItem( myValue.toString(), true );
      }
      else
      {
        omglistTaxa->addItem( myValue.toString(), false );
      }
    }
  }
}

void OmgTaxaPage::setSpeciesList(QString theFileName)
{
  QSettings mySettings;
  QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
  if (myLoaderType=="FILE") //read locs from a single file with many taxa
  {
    mSpeciesFileName=theFileName;
    //clear the existing entries in the taxon combo first
    omglistTaxa->clear();
    //
    // Now that we have the localities text file, we need to parse it and find
    // all unique instances of taxon names and populate the taxon combo...
    //
    QStringList myTaxonList = getSpeciesList(theFileName);
    //sort the taxon list alpabetically descending order
    //Qt4 built in sort does in ascending order so I wrote my own sort
    myTaxonList = Omgui::sortList(myTaxonList);
    //now find the uniqe entries in the qstringlist and
    //add each entry to the taxon combo
    QString myLastTaxon="";
    QStringList::Iterator myIterator= myTaxonList.begin();
    while( myIterator!= myTaxonList.end() )
    {
      QString myCurrentTaxon=*myIterator;
      if (myCurrentTaxon!=myLastTaxon)
      {
        omglistTaxa->addItem(myCurrentTaxon,true);
      }
      myLastTaxon=*myIterator;
      ++myIterator;
    }
  }
  else //read files from a dir
  {
    omglistTaxa->clear();
    QDir myDir(theFileName);
    myDir.setFilter( QDir::Files | QDir::Readable );
    myDir.setSorting( QDir::Name );
    QStringList myFilters;
    myFilters << "*.txt";
    myDir.setNameFilters( myFilters );

    QFileInfoList myList = myDir.entryInfoList();
    if (myList.size()==0)
    {
      QMessageBox::warning( this,tr("openModeller OmgWizard Warning"),
          tr("No valid localities files were found. "
            "Make sure that your localities files have a .txt extension "
            "and are in the format:\n"
            "ID <tab> Taxon name <tab> longitude <tab> latitude"));
      mFirstNonIdRecord=false;
    }
    for (int myCount = 0; myCount < myList.size(); ++myCount) 
    {
      QFileInfo myFileInfo = myList.at(myCount);
      if (!myFileInfo.fileName().endsWith(".txt"))
      {
        continue;
      }
      omglistTaxa->addItem(myFileInfo.fileName(),true);
    }
    //show how many files you have
    //lblFileCount->setText("("+QString::number(lstLocalitiesFiles->count())+")");
  }
} //end of setSpeciesList

QStringList OmgTaxaPage::getSpeciesList(QString theFileName)
{
  QStringList myTaxonList;
  QFile myQFile( theFileName );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
    unsigned int myCounter=0;
    while ( !myQTextStream.atEnd() )
    {
      myCurrentLine = myQTextStream.readLine(); // line of text excluding '\n'
      if (myCurrentLine.startsWith("#"))
      {
        continue;
      }
      //split on word boundaries ignoring empty parts
      QStringList myList = myCurrentLine.split(QRegExp("[\t]"));
      //qDebug("Read line : " + myList.join(" -- ").toLocal8Bit());
      if (myList.size() < 4)
      {
        continue;
      }
      else //new file format with at least 4 columns
      {
        QString myId=myList.at(0).simplified();
        QString myTaxonName=myList.at(1).simplified();
        if (myTaxonName!= "")
        {
          //make sure there are only single spaces separating words.
          myTaxonName=myTaxonName.replace( QRegExp(" {2,}"), " " );
          myTaxonList.append(myTaxonName);
          ++myCounter;
        }
      }
    }
    myQFile.close();
  }
  else
  {
    QMessageBox::warning( this,QString("openModeller OmgWizard Error"),QString("The localities file is not readable. Check you have the neccessary file permissions and try again."));
  }
  return myTaxonList;
}

bool OmgTaxaPage::validatePage()
{
  if ( omglistTaxa->checkedItemCount() < 1 )
  {
    QMessageBox::warning( this,QString("openModeller OmgWizard Error"),QString("You need to select at least one taxon!"));
    return false;
  }
  return true;
}

OmgAlgorithmPage::OmgAlgorithmPage( ) : QWizardPage( ) 
{
  setupUi(this);
}


OmgAlgorithmPage::~OmgAlgorithmPage()
{
}

OmgLayerSetPage::OmgLayerSetPage( ) : QWizardPage( ) 
{
  setupUi(this);
  registerField( "UseQGISRasterLayers",  radUseQGISRasters );
}


OmgLayerSetPage::~OmgLayerSetPage()
{
}

bool OmgLayerSetPage::validatePage()
{
  if ( radUseQGISRasters->isChecked( ) )
  {
    if ( lstRasterLayers->selectedItems().count() < 1 )
    {
      QMessageBox::warning( this,QString("openModeller OmgWizard Error"),QString("You need to select at least one raster layer!"));
      return false;
    }
    return true;
  }
  else
  {
    if ( cboModelLayerSet->count() <1 )
    {
      QMessageBox::warning( this,QString("openModeller OmgWizard Error"),QString("You need to define at least one raster layerset before you can proceed. Exit the wizard and use the layerset manager to create a layerset."));
      return false;
    }
    return true;
  }
}

OmgProjectionPage::OmgProjectionPage( ) : QWizardPage( ) 
{
  setupUi(this);
}


OmgProjectionPage::~OmgProjectionPage()
{
}

void OmgProjectionPage::initializePage()
{
  qDebug( "Initializing projection page" ); 
  OmgWizard * myWizard = qobject_cast<OmgWizard *>( wizard() );
  myWizard->refreshMapFormatCombo();
  if ( field("UseQGISRasterLayers").toBool() )
  {
    setSubTitle("You are running your model using QGIS layers so the same layers will be used to project the model. Choose the output template and format below.");
    omglistProjectionLayerSets->setEnabled( false );
  }
  else
  {
    setSubTitle("Once a model has been created it needs to be projected to create a map. Also choose the output template and format below.");
    omglistProjectionLayerSets->setEnabled( true );
  }
}

void OmgProjectionPage::on_toolFormatLayer_clicked()
{
  QSettings mySettings;
  QString myBaseDir = mySettings.value("openModeller/experimentDesigner/formatLayersDirectory","c:/tmp").toString(); //initial dir
  OmgLayerSelector * myOmgLayerSelector = new OmgLayerSelector(myBaseDir, this);
  myOmgLayerSelector->setSelectionMode(QAbstractItemView::SingleSelection);
  if(myOmgLayerSelector->exec())
  {
    QStringList myList=myOmgLayerSelector->getSelectedLayers();
    //sort on descending alphabetical order
    myList = Omgui::sortList(myList);
    if (myList.size()<1)
    {
      //do nothing
      return;
    }
    else
    {
      cboOutputFormatLayer->insertItem(0, myList[0]);
    }
    mySettings.setValue("openModeller/experimentDesigner/formatLayersDirectory",myOmgLayerSelector->getBaseDir());
  }
}
