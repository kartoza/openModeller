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
#include "omgexperimentdesigner.h"
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
#include <QHash>
//std c++ stuff
#include <cassert>

OmgExperimentDesigner::OmgExperimentDesigner(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  //a flag so we can remind user once only if they are missing ids in their occ file;
  mFirstNonIdRecord=true;
  QDate myDate = QDate::currentDate();
  leExperimentName->setText(tr(""));
  leExperimentDescription->setText(tr("New Experiment created on ") 
      + myDate.longDayName(myDate.dayOfWeek()) 
      + ", " 
      + QString::number(myDate.day())
      + " " 
      + myDate.longMonthName(myDate.month()) 
      + ", " 
      + QString::number(myDate.year()));

  //when the user clicks on teh addItems tool in the omglistTaxa widget
  //they should be prompted to load a file of occurrence data

  connect(omglistTaxa, SIGNAL(addItemClicked()),
      this, SLOT(toolSelectLocalitiesFileClicked()));
  connect(omglistTaxa, SIGNAL(configureClicked()),
      this, SLOT(toolConfigureLocalitiesClicked()));

  //
  // First populate the algs combo
  //
  //iterate through the algorithm sets adding a row for each to the table
  mAlgorithmSet = OmgAlgorithmSet::getFromActivePlugin();
  mAlgorithmSet.loadAlgorithms(Omgui::userAlgorithmProfilesDirPath());
  //qDebug(myAlgorithmSet.toXml().toLocal8Bit());
  //iterate through the algorithm sets adding a row for each to the table
  //first empty the table
  omglistAlgorithms->clear();

  //see which items were checked the last time the user was here
  QSettings mySettings;
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  QStringList myList = mySettings.value("openModeller/experimentDesigner/" + myPluginName + "/selectedAlgorithms").toStringList();
  for (int i=0; i < mAlgorithmSet.count(); i++)
  {
    OmgAlgorithm myAlgorithm = mAlgorithmSet[i];
    // Add details to the new row
    QListWidgetItem *mypNameItem = new QListWidgetItem(myAlgorithm.name());
    mypNameItem->setData(Qt::UserRole,myAlgorithm.guid());
    //display the text in red if the origin of the alg is from the adapter
    //rather than the users xml alg profile store
    OmgAlgorithm::Origin myOrigin = myAlgorithm.origin();
    QIcon myIcon;
    if (myOrigin==OmgAlgorithm::USERPROFILE)
    {
      myIcon.addFile(":/algorithmTypeUser.png");
    }
    else if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
    {
      myIcon.addFile(":/algorithmTypeSystem.png");
    }
    else if (myOrigin==OmgAlgorithm::UNDEFINED)
    {
      mypNameItem->setTextColor(Qt::yellow);
    }
    mypNameItem->setIcon(myIcon);
    if (myList.contains(myAlgorithm.name()))
    {
      omglistAlgorithms->addItem(mypNameItem, true);
    }
    else
    {
      omglistAlgorithms->addItem(mypNameItem, false);
    }
  }

  //
  // Now the list of available SRSs
  // Note this way is much much faster than using an iterator!
  //
  OmgSpatialReferenceSystem mySRS;
  mWktMap = mySRS.getWktMap();
  QList<QString> myKeys = mWktMap.keys();
  cboCoordinateSystem->addItems(static_cast<QStringList>(myKeys));

  //
  //populate model creation and projection layerset combos
  //

  Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
  QMapIterator<QString, OmgLayerSet> myIterator(myLayerSetMap);
  while (myIterator.hasNext()) 
  {
    myIterator.next();
    OmgLayerSet myLayerSet = myIterator.value();
    QString myGuid = myLayerSet.guid();
    QString myName = myLayerSet.name();
    //display an icon indicating if the layerset is local or remote (e.g. terralib)
    QIcon myIcon;
    myIcon.addFile(":/localdata.png");
    cboModelLayerSet->addItem(myIcon,myName,myGuid);

    //
    // Add this layerset to the projections list
    // 
    QListWidgetItem *mypItem = new QListWidgetItem(myName);
    mypItem->setData(Qt::UserRole,myGuid);
    mypItem->setIcon(myIcon);
    QStringList myProjectionList = 
      mySettings.value("openModeller/experimentDesigner/" + 
          myPluginName + "/selectedProjectionLayerSets").toStringList();
    if (myProjectionList.contains(myName))
    {
      omglistProjectionLayerSets->addItem(mypItem, true);
    }
    else
    {
      omglistProjectionLayerSets->addItem(mypItem, false);
    }
  }
  //tweak the omglistTaxa list a bit
  //omglistTaxa->hideOptionsTool();
  //and the alg list
  omglistAlgorithms->hideOptionsTool();
  omglistAlgorithms->hideAddItemTool();
  omglistProjectionLayerSets->hideOptionsTool();
  omglistProjectionLayerSets->hideAddItemTool();

  //! @TODO populate other form controls
  readSettings();

	// User data path
  leDataDirectory->setText(mySettings.value("dataDirs/dataDir",QDir::homePath() + QString("/.omgui/")).toString());
	//populate teh output image formats combo
  QHash<QString,QString> outputFormatsHash = Omgui::getOutputFormats();
  QHashIterator<QString, QString> outputImageFormatsIterator(outputFormatsHash);
  QString myFormat = mySettings.value("outputFormat","GreyTiff100").toString();
  int myDefaultIndex=0;
  int myCurrentIndex=0;
  while (outputImageFormatsIterator.hasNext()) 
  {
    outputImageFormatsIterator.next();
    //We will add the hash item key to the Qt::UserRole 
    //for the combo list entry
    cboOutputFormat->addItem(outputImageFormatsIterator.value(), outputImageFormatsIterator.key());
    if (outputImageFormatsIterator.key()==myFormat)
    {
      myDefaultIndex=myCurrentIndex;
    }
    ++myCurrentIndex;
  }
  cboOutputFormat->setCurrentIndex(myDefaultIndex);
}


OmgExperimentDesigner::~OmgExperimentDesigner()
{
}

void OmgExperimentDesigner::on_pbnCancel_clicked()
{
  QDialog::reject();
}


void OmgExperimentDesigner::on_pbnOk_clicked()
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
  if (mWktMap[cboCoordinateSystem->currentText()].isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select a valid coordinate system."));
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

OmgExperiment * OmgExperimentDesigner::getExperiment()
{
  QSettings mySettings;
  emit logMessage("Creating experiment");
  //set up the experiment
  OmgExperiment * mypExperiment = new OmgExperiment();
  mypExperiment->setName(leExperimentName->text());
  mypExperiment->setDescription(leExperimentDescription->text());

  //Initialise the modeller adapter

  /* This is the Abstract plugin type. Depending on the users
   * options, it will be initialised to either the OmgModellerLocalPlugin
   * or OmgModellerWebServices plugin (or possibly other plugin types in 
   * the future) 
   */
  //qDebug("Experiment designer calling OmGui to get the modeller plugin");
  mpModellerPlugin = OmgModellerPluginRegistry::instance()->getPlugin();
  //qDebug("Experiment designer testing if returned modeller plugin is ok");
  if(!mpModellerPlugin)
  {
    //this is bad! TODO notify user he has no useable adapters
    //TODO handle this more gracefully than asserting!
    //qDebug("Experiment designer Error no valid modelling adapters could be loaded");
    QMessageBox::critical( this,tr("openModeller Desktop"),
        tr("No modelling plugins could be found.\nPlease report this problem to "
          "your system administrator or the openModeller developers."));
    //assert ("Undefined adapter type in __FILE__  , line  __LINE__");
    close();
  }
  else
  {
    //qDebug("Plugin is good to go....");
  }
  mypExperiment->setModellerPlugin(mpModellerPlugin);
  
  //
  // To build the experiment we have two (nested) loops
  // The outer loop walks the list of checked algorithm profiles
  // while the inner loop walks the list of check taxa
  // Thus the experiment consists of CheckedAlgs * CheckedTaxa models
  //
  
  //get the alg properties first
  //
  //QString myAlgGuid=cboModelAlgorithm->itemData(cboModelAlgorithm->currentIndex()).toString();
  int myAlgorithmCount = omglistAlgorithms->checkedItemCount();
  QStringList myAlgorithmList = omglistAlgorithms->checkedDataItems();

  int myTaxaCount = omglistTaxa->checkedItemCount();
  QStringList myTaxaList = omglistTaxa->checkedItems();

  float myTrainingProportion = myOptions.getTrainingProportion() / 100;

  for ( int myCounter = 0; myCounter < myTaxaCount; myCounter++ )
  {
    QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
    QString myTaxon, mySpeciesFileName;
    if (myLoaderType=="FILE") //read locs from a single file with many taxa
    {
      myTaxon = myTaxaList.at(myCounter);
      emit logMessage ("Adding " + myTaxon.toLocal8Bit() + " to experiment");
      mySpeciesFileName = mSpeciesFileName;
    }
    else //list entries are file names from the dir
    {
      QString myFileName = myTaxaList.at(myCounter);
      //we need to find out the first
      QString myPrefix = 
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString();
      mySpeciesFileName = myPrefix + QDir::separator() + myFileName;
      QStringList mySpeciesList = getSpeciesList(myPrefix + QDir::separator() + myFileName);
      if (mySpeciesList.size()<1)
      {
        continue;
      }
      myTaxon=mySpeciesList.at(0);
      QString myMessage("Adding " + myFileName + " to experiment for taxon: " + myTaxon);
      emit logMessage (myMessage);
      //qDebug(myMessage);
    }

    OmgLocalities myLocalities, myTrainingLocalities, myTestingLocalities;

    unsigned int myLocalitiesCount = loadLocalities(myLocalities, mySpeciesFileName, myTaxon);

    if (myLocalitiesCount == 0)
    {
      // TODO!!! No need to run the models. Warn user.
    }

    // Split Localities (test x training) if user asked for external test
    if (myTrainingProportion < 1.0)
    {
      splitLocality(myLocalities, myTrainingLocalities, myTestingLocalities, myTrainingProportion);

      if (myTrainingLocalities.size() == 0)
      {
        // TODO!!! No way to generate model without training points. Warn user.
      }
      if (myTestingLocalities.size() == 0)
      {
        // TODO!!! No external test. Warn user.
      }
    }
    else
    {
      // Use all available localities for training
      myTrainingLocalities = myLocalities;
    }

    for (int i=0; i < myAlgorithmCount; i++)
    {
      emit experimentProgress(i);
      QApplication::processEvents();

      //qDebug("Getting algorithm for GUID             : " + myAlgorithmList.at(i).toLocal8Bit());
      OmgAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm( myAlgorithmList.at(i) );
      //qDebug("myAlgorithms GUID (should match above!): " + myAlgorithm.guid().toLocal8Bit());

      OmgModel * mypModel = new OmgModel();
      mypModel->setCoordinateSystemName( cboCoordinateSystem->currentText() );
      mypModel->setCoordinateSystem( mWktMap[cboCoordinateSystem->currentText()] );

      mypModel->setSpeciesFile(mySpeciesFileName);
      mypModel->setTaxonName(myTaxon);
      Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
      OmgLayerSet myModelLayerSet = myLayerSetMap[cboModelLayerSet->currentText()];
      mypModel->setCreationLayerSet(myModelLayerSet);
      mypModel->setAlgorithm(myAlgorithm);

      //options for locality uniqueness
      //NOTE: consider applying the filter here, but environmentally unique would
      //      require access to layers (not possible with web services plugin)

      QString myFilter = mySettings.value("localities/localitiesFilterType","NO_FILTER").toString();

      if (myFilter=="NO_FILTER")
      {
        mypModel->setLocalitiesFilterType(OmgModel::NO_FILTER);
      }
      else if(myFilter=="SPATIALLY_UNIQUE")
      {
        mypModel->setLocalitiesFilterType(OmgModel::SPATIALLY_UNIQUE);
      }
      else //environmentally unique - program that will appply the filter needs access to layers!
      {
        mypModel->setLocalitiesFilterType(OmgModel::ENVIRONMENTALLY_UNIQUE);
      }

      mypModel->setTrainingProportion(myTrainingProportion); // not used anymore!

      mypModel->setLocalities(myTrainingLocalities);

      // Add internal test
      OmgModelTest * mypInternalTest = new OmgModelTest(mypModel);
      mypInternalTest->setLabel(QObject::tr("Internal Test"));
      mypInternalTest->setLocalities(myTrainingLocalities);
      mypInternalTest->setInternal(true);
      if (myModelTestOptions.getUseLowestThreshold())
      {
        mypInternalTest->setUseLowestThreshold(true);
      }
      else
      {
        mypInternalTest->setThreshold(myModelTestOptions.getThreshold()/100);
      }
      mypInternalTest->setNumBackgroundPoints(myModelTestOptions.getNumBackgroundPoints());
      double myMaxOmission = myModelTestOptions.getMaxOmission();
      if (myMaxOmission < 100 )
      {
        mypInternalTest->setMaxOmission(myMaxOmission/100);
      }
      mypModel->addTest(mypInternalTest);

      if (myTrainingProportion < 1.0 && myTestingLocalities.size() > 0)
      {
        // Add external test
        OmgModelTest * mypExternalTest = new OmgModelTest(mypModel);
        mypExternalTest->setLabel(QObject::tr("External Test"));
        mypExternalTest->setLocalities(myTestingLocalities);
        mypExternalTest->setNumBackgroundPoints(myModelTestOptions.getNumBackgroundPoints());
        if (myMaxOmission < 100 )
        {
          mypExternalTest->setMaxOmission(myMaxOmission/100);
        }
        mypModel->addTest(mypExternalTest);
      }

      //
      // Add projection(s) to the model
      //
      QStringList myProjectionsList = omglistProjectionLayerSets->checkedItems();
      QString myOutputFormatLayerName = cboOutputFormatLayer->currentText();
      QStringListIterator myIterator(myProjectionsList);
      while (myIterator.hasNext())
      {
        QString myLayerSetName = myIterator.next();
        OmgProjection * mypProjection = new OmgProjection();
        OmgLayerSet myProjectionLayerSet = myLayerSetMap[myLayerSetName];
        mypProjection->setOutputFormatLayerName(myOutputFormatLayerName);
        QString myRasterFormat = mySettings.value("outputFormat","GreyTiff").toString();
        mypProjection->setOmRasterFormat(myRasterFormat);
        mypProjection->setLayerSet(myProjectionLayerSet);
        mypProjection->setTaxonName(myTaxon);
        mypProjection->setAlgorithm(myAlgorithm);    
        mypModel->addProjection(mypProjection);
      }

      mypExperiment->addModel(mypModel);
      //for debugging only
      //Omgui::createTextFile("/tmp/model.xml",mypModel->toXml().toLocal8Bit());
    }
  
  }
  emit experimentMaximum(omglistTaxa->checkedItemCount() * myAlgorithmCount);
  //qDebug("Created experiment successfully");
  return mypExperiment;
}

void OmgExperimentDesigner::setStatus(QString theStatus)
{
  //qDebug( theStatus.toAscii() );
}

void OmgExperimentDesigner::toolConfigureLocalitiesClicked()
{
  myOptions.exec();
}

void OmgExperimentDesigner::toolSelectLocalitiesFileClicked()
{
  QSettings mySettings;
  QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
  if (myLoaderType=="FILE") //read locs from a single file with many taxa
  {
    QString myFileTypeQString;
    QString myTextFileFilterString="Text File (*.txt)";
    QString myFileName = QFileDialog::getOpenFileName(
        this,
        "Select localities text file" , //caption
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(), //initial dir
        myTextFileFilterString,  //filters to select
        &myFileTypeQString //the pointer to store selected filter
        );
    //std::cout << "Selected filetype filter is : " << myFileTypeQString.toAscii() << std::endl;
    if (myFileName.isEmpty()) return;
    setSpeciesList(myFileName);

    //store directory where localities file is for next time
    QFileInfo myFileInfo(myFileName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myFileInfo.dir().path()+QString("/") );
  }
  else //read files from a dir
  {
    omglistTaxa->clear();
    QString myDirName = QFileDialog::getExistingDirectory(this, 
        tr("Select Directory"),
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(),
        QFileDialog::ShowDirsOnly);
    if (myDirName.isEmpty()) return;
    setSpeciesList(myDirName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myDirName + QString("/") );

  }
} //end of on_pbnSelectLocalitiesFile_clicked


void OmgExperimentDesigner::setSpeciesList(QString theFileName)
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
      QMessageBox::warning( this,tr("openModeller OmgExperimentDesigner Warning"),
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

QStringList OmgExperimentDesigner::getSpeciesList(QString theFileName)
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
    QMessageBox::warning( this,QString("openModeller OmgExperimentDesigner Error"),QString("The localities file is not readable. Check you have the neccessary file permissions and try again."));
  }
  return myTaxonList;
}

bool OmgExperimentDesigner::checkLayersMatch()
{
  //! @TODO NB MORE SOPHISTICATED CHECKING SHOULD BE ADDED LATER!!!!
  //if (lstProjectionLayers->count()==lstModelLayers->count())
  //{
    return true;
  //}
  //else
  //{
  //  return false;
  //}
}//checkLayersMatch
void OmgExperimentDesigner::on_toolFormatLayer_clicked()
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
    cboOutputFormatLayer->insertItem(0, myList[0]);
    mySettings.setValue("openModeller/experimentDesigner/formatLayersDirectory",myOmgLayerSelector->getBaseDir());
  }
}

void OmgExperimentDesigner::readSettings()
{
  QSettings mySettings;
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();

  //select the last model creation layer set used by getting the name from qsettings
  QString myLayerSetName = mySettings.value("openModeller/experimentDesigner/" +
      myPluginName + "/modelLayerSet").toString();
  setComboToDefault( cboModelLayerSet, myLayerSetName, false);
  //select the last projection layer set used by getting the name from qsettings
  myLayerSetName = mySettings.value("openModeller/experimentDesigner/" + 
      myPluginName + "/projectionLayerSet").toString();
  //set the coordinate system to the same as the last time run
  QString myCoordSystem = mySettings.value("openModeller/experimentDesigner/coordSystem","Lat/Long WGS84").toString();
  setComboToDefault( cboCoordinateSystem, myCoordSystem, false);
  //set the localities filename
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
  // Restore the output format
  QString myOutputFormat = mySettings.value("openModeller/experimentDesigner/" + 
      myPluginName + "/outputFormatFile").toString();
  if (QFile::exists(myOutputFormat))
  {
    setComboToDefault(cboOutputFormatLayer,myOutputFormat,true);
  }
}

void OmgExperimentDesigner::writeSettings()
{
  QSettings mySettings;
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  mySettings.setValue("openModeller/experimentDesigner/localitiesFileName", mSpeciesFileName);
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/modelLayerSet",
      cboModelLayerSet->currentText());
  mySettings.setValue("openModeller/experimentDesigner/coordSystem",cboCoordinateSystem->currentText());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/outputFormatFile",cboOutputFormatLayer->currentText());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/selectedProjectionLayerSets",omglistProjectionLayerSets->checkedItems());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/selectedAlgorithms",omglistAlgorithms->checkedItems());
}

/* update the layer formats combo when a model layer set is changed */
void OmgExperimentDesigner::on_cboModelLayerSet_currentIndexChanged(int theIndex)
{
  refreshMapFormatCombo();
}

/* update the layer formats combo when a projection  layer set is changed */
void OmgExperimentDesigner::on_cboProjectionLayerSet_currentIndexChanged(int theIndex)
{
  refreshMapFormatCombo();
}

void OmgExperimentDesigner::refreshMapFormatCombo()
{
  Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
  OmgLayerSet myModelLayerSet = myLayerSetMap[cboModelLayerSet->currentText()];
  //OmgLayerSet myProjectionLayerSet = myLayerSetMap[cboProjectionLayerSet->currentText()];
  QStringList myNameList = myModelLayerSet.nameList();
  //myNameList << myProjectionLayerSet.nameList();
  myNameList = Omgui::sortList(myNameList);
  myNameList = Omgui::uniqueList(myNameList);
  cboOutputFormatLayer->clear();
  cboOutputFormatLayer->insertItems(0,myNameList);
}
QStringList OmgExperimentDesigner::getListWidgetEntries(const QListWidget * theListWidget)
{
  QStringList myList;
  for ( int myCounter = 0; myCounter < theListWidget->count(); myCounter++ )
  {
    myList.append(theListWidget->item(myCounter)->text());
  }
  return myList;
}

bool OmgExperimentDesigner::setComboToDefault(QComboBox * thepCombo, QString theDefault, bool theAddItemFlag)
{
  if (!theDefault.isEmpty())
  {
    //loop through list looking for a match
    for ( int myCounter = 0; myCounter < thepCombo->count(); myCounter++ )
    {
      thepCombo->setCurrentIndex(myCounter);
      if (thepCombo->currentText()==theDefault)
      {
        break;
      }
    }
    if (theAddItemFlag)
    {
      //if we get this far then no match was found and we should insert our
      //default as a new combo list entry
      thepCombo->insertItem(theDefault);
      thepCombo->setCurrentIndex(thepCombo->count()-1);
    }
  }
  else
  {
    return false;
  }
  return true;
}

void OmgExperimentDesigner::on_toolSelectDataDirectory_clicked()
{
  QString myFileName = QFileDialog::getExistingDirectory(
      this,
      tr("Select a directory where your data will be saved") , //caption
      leDataDirectory->text() //initial dir
    );
	leDataDirectory->setText(myFileName);
}

void OmgExperimentDesigner::on_toolModelSettings_clicked()
{
	myModelTestOptions.exec();
}

