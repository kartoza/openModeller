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
#include "omgwizard.h"
#include "omgexperiment.h"
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
#include <QTextStream>
#include <QHash>
#include <QDebug>
#include <QMap>
//std c++ stuff
#include <cassert>
//QGIS Includes
#include <qgsmaplayer.h>
#include <qgsvectorlayer.h>
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectordataprovider.h>
#include <qgsfeature.h>
#include <qgsgeometry.h>
#include <qgspoint.h>

OmgWizard::OmgWizard( QWidget* parent, Qt::WindowFlags flags )
    : QWizard( parent, flags )
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  mpExperimentPage = new OmgExperimentPage();
  mpOccurrencesPage = new OmgOccurrencesPage();
  mpFieldsPage = new OmgFieldsPage();
  mpTaxaPage = new OmgTaxaPage();
  mpAlgorithmPage = new OmgAlgorithmPage();
  mpLayerSetPage = new OmgLayerSetPage();
  mpProjectionPage = new OmgProjectionPage();
  setPage( EXPERIMENT_PAGE, mpExperimentPage );
  setPage( OCCURRENCES_PAGE, mpOccurrencesPage );
  setPage( FIELDS_PAGE, mpFieldsPage );
  setPage( TAXA_PAGE, mpTaxaPage );
  setPage( ALGORITHMS_PAGE, mpAlgorithmPage );
  setPage( LAYERSET_PAGE, mpLayerSetPage );
  setPage( PROJECTION_PAGE, mpProjectionPage );
  //to access widgets on any page you can just refer to them
  //directly if they were implemented within a wizardpage directly 
  //in the wizard in desigener.
  //e.g. to get the text browser on the welcome page do
  //textBrowser->hide();
  //for pages which have been manually created or as separate wizard pages you
  //need to refer to the control via the page instance as shown in the next
  //line

  //cant do as registerfield is protected
  //wizardExperimentPage->registerField("experimentName*", leExperimentName);
  //get a list of all raster layers
  bool myHasRastersFlag = false;
  QMap< QString, QgsMapLayer * > myLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap< QString, QgsMapLayer *>::iterator it;
  for ( it = myLayers.begin(); it != myLayers.end() ; ++it )
  {
    qDebug( it.key() );
    QgsMapLayer * myLayer = it.value();
    if ( myLayer->type() == QgsMapLayer::VectorLayer ) 
    {
      QgsVectorLayer * myVectorLayer =  qobject_cast<QgsVectorLayer *>( myLayer );
      if ( myVectorLayer->geometryType() == QGis::Point ||
           myVectorLayer->geometryType() == QGis::NoGeometry)
      {
        QListWidgetItem * mypItem = new QListWidgetItem( myLayer->name() );
        mypItem->setData( Qt::UserRole, it.key() );
        mpOccurrencesPage->lstPointLayers->addItem( mypItem );
      }
    }
    else // RasterLayer
    {
      QListWidgetItem * mypItem = new QListWidgetItem( myLayer->name() );
      mypItem->setData( Qt::UserRole, it.key() );
      mpLayerSetPage->lstRasterLayers->addItem( mypItem );
      myHasRastersFlag = true;
    }
  }
  if ( mpOccurrencesPage->lstPointLayers->count() > 0 )
  {
    mpOccurrencesPage->radUseOccurrencesLayer->setChecked( true );
    mpOccurrencesPage->lstPointLayers->setCurrentRow( 0 );
  }
  else
  {
    //disable the qgis layer selection since there are no layers
    mpOccurrencesPage->radUseOccurrencesFile->setChecked( true );
    mpOccurrencesPage->radUseOccurrencesLayer->setEnabled( false );
  }
  if ( myHasRastersFlag )
  {
    mpLayerSetPage->radUseQGISRasters->setEnabled( true );
    mpLayerSetPage->radUseQGISRasters->setChecked( true );
    mpLayerSetPage->lstRasterLayers->setEnabled( true );
  }
  else
  {
    mpLayerSetPage->radUseLayerset->setChecked( true );
    mpLayerSetPage->radUseQGISRasters->setEnabled( false );
    mpLayerSetPage->lstRasterLayers->setEnabled( false );
  }

  //a flag so we can remind user once only if they are missing ids in their occ file;
  mFirstNonIdRecord=true;
  QDate myDate = QDate::currentDate();
  mpExperimentPage->leExperimentName->setText(tr(""));
  mpExperimentPage->leExperimentDescription->setText(tr("New Experiment created on ") 
      + myDate.longDayName(myDate.dayOfWeek()) 
      + ", " 
      + QString::number(myDate.day())
      + " " 
      + myDate.longMonthName(myDate.month()) 
      + ", " 
      + QString::number(myDate.year()));

  //when the user clicks on teh addItems tool in the omglistTaxa widget
  //they should be prompted to load a file of occurrence data

  connect(mpTaxaPage->omglistTaxa, SIGNAL(addItemClicked()),
      this, SLOT(toolSelectLocalitiesFileClicked()));
  connect(mpTaxaPage->omglistTaxa, SIGNAL(configureClicked()),
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
  mpAlgorithmPage->omglistAlgorithms->clear();

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
      mpAlgorithmPage->omglistAlgorithms->addItem(mypNameItem, true);
    }
    else
    {
      mpAlgorithmPage->omglistAlgorithms->addItem(mypNameItem, false);
    }
  }

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
    mpLayerSetPage->cboModelLayerSet->addItem(myIcon,myName,myGuid);

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
      mpProjectionPage->omglistProjectionLayerSets->addItem(mypItem, true);
    }
    else
    {
      mpProjectionPage->omglistProjectionLayerSets->addItem(mypItem, false);
    }
  }
  if ( mpLayerSetPage->cboModelLayerSet->count() < 1 )
  {
    mpLayerSetPage->radUseLayerset->setEnabled( false );
    mpLayerSetPage->radUseQGISRasters->setChecked( true );
  }
  //tweak the omglistTaxa list a bit
  //omglistTaxa->hideOptionsTool();
  //and the alg list
  mpAlgorithmPage->omglistAlgorithms->hideOptionsTool();
  mpAlgorithmPage->omglistAlgorithms->hideAddItemTool();
  mpProjectionPage->omglistProjectionLayerSets->hideOptionsTool();
  mpProjectionPage->omglistProjectionLayerSets->hideAddItemTool();

  //! @TODO populate other form controls
  readSettings();

  // User data path
  mpExperimentPage->leDataDirectory->setText(mySettings.value("dataDirs/dataDir",QDir::homePath() + QString("/.omgui/")).toString());
  //populate the output image formats combo
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
    mpProjectionPage->cboOutputFormat->addItem(outputImageFormatsIterator.value(), outputImageFormatsIterator.key());
    if (outputImageFormatsIterator.key()==myFormat)
    {
      myDefaultIndex=myCurrentIndex;
    }
    ++myCurrentIndex;
  }
  mpProjectionPage->cboOutputFormat->setCurrentIndex(myDefaultIndex);

}


OmgWizard::~OmgWizard()
{
}

void OmgWizard::accept()
{
  QSettings mySettings;

  //User data dir settings
  mySettings.setValue("dataDirs/dataDir", mpExperimentPage->leDataDirectory->text());
  //output format
  mySettings.setValue("outputFormat",mpProjectionPage->cboOutputFormat->itemData(mpProjectionPage->cboOutputFormat->currentIndex()).toString());

  //first do some sanity checking....
  if (mpExperimentPage->leExperimentName->text().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),tr("Please give this experiment a valid name!"));
    return;
  }

  QString myWorkDirName = QDir::convertSeparators(Omgui::getModelOutputDir()) 
    + QDir::separator() 
    + mpExperimentPage->leExperimentName->text()
    + QDir::separator(); 
  QDir myWorkDir(myWorkDirName );
  if (myWorkDir.exists())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("An experiment directory with this name already exists!"));
    return;
  }
  if(mpTaxaPage->omglistTaxa->checkedItemCount() < 1)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select one or more taxa before attempting to run the experiment"));
    return;
  }
  if (
      !mpLayerSetPage->radUseQGISRasters->isChecked() &&
      mpLayerSetPage->cboModelLayerSet->count()==0)
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please create a layer set using the Layer Set Manager and "
          "then select one layer set for the model creation process "
          "before attempting to run the experiment"));
    return;
  }
  if (mpProjectionPage->omglistProjectionLayerSets->count() < 1 &&
      !mpLayerSetPage->radUseQGISRasters->isChecked())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please create a layer set for model projection using the "
          "Layer Set Manager and then select one or more layer sets for the model "
          "projection process before attempting to run the experiment"));
    return;
  }
  if (mpProjectionPage->omglistProjectionLayerSets->checkedItemCount() < 1 &&
      !mpLayerSetPage->radUseQGISRasters->isChecked())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select a layer set for model projection "
          "before attempting to run the experiment"));
    return;
  }
  if (mpProjectionPage->cboOutputFormatLayer->currentText().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select an output format layer before attempting "
          "to run the model"));
    return;
  }
  //qDebug("Writing settings...");
  writeSettings();

  OmgExperiment * mypExperiment = getExperiment();
  qDebug( mypExperiment->toString() );
  //then ok is pressed in progress monitor we can close up shop here
  //we are going to pass the experiment to the main window. its the main windows job to delete it
  hide();
  emit experimentCreated( mypExperiment );
  done(1);
}

OmgExperiment * OmgWizard::getExperiment()
{
  QSettings mySettings;
  emit logMessage("Creating experiment");
  //set up the experiment
  OmgExperiment * mypExperiment = new OmgExperiment();
  mypExperiment->setName(mpExperimentPage->leExperimentName->text());
  mypExperiment->setDescription(mpExperimentPage->leExperimentDescription->text());
  prepareQGISLayerSet();

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
  int myAlgorithmCount = mpAlgorithmPage->omglistAlgorithms->checkedItemCount();
  QStringList myAlgorithmList = mpAlgorithmPage->omglistAlgorithms->checkedDataItems();

  int myTaxaCount = mpTaxaPage->omglistTaxa->checkedItemCount();
  QStringList myTaxaList = mpTaxaPage->omglistTaxa->checkedItems();

  float myTrainingProportion = myOptions.getTrainingProportion() / 100;

  for ( int myCounter = 0; myCounter < myTaxaCount; myCounter++ )
  {
    QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
    QString myTaxon, mySpeciesFileName;
    myTaxon = myTaxaList.at(myCounter);
    if (myLoaderType=="FILE") //read locs from a single file with many taxa
    {
      emit logMessage ("Adding " + myTaxon.toLocal8Bit() + " to experiment");
      mySpeciesFileName = mpTaxaPage->speciesFileName();
    }
    else //list entries are file names from the dir
    {
      //we need to find out the first
      QString myPrefix = 
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString();
      mySpeciesFileName = myPrefix + QDir::separator() + myTaxon;
      QString myMessage("Adding " + mySpeciesFileName + " to experiment for taxon: " + myTaxon);
      emit logMessage (myMessage);
    }

    OmgLocalities myLocalities, myTrainingLocalities, myTestingLocalities;


    unsigned int myLocalitiesCount = 0;
    if ( mpOccurrencesPage->radUseOccurrencesLayer->isChecked( )) //load from qgis layer
    {
      qDebug("creating localities from qgis layer");

      QgsVectorLayer * myLayer = getOccurrenceLayer( field("OccurrencesQGISLayer").toInt() );
      QgsVectorDataProvider * myProvider = myLayer->dataProvider();
      QgsFeature f;
      int myTaxonFieldIndex = field("TaxonField").toInt();
      int myXFieldIndex = field("XField").toInt();
      int myYFieldIndex = field("YField").toInt();
      // subtract 1 to account for the [None] entry
      int myFrequencyFieldIndex = field("FrequencyField").toInt() - 1;
      QgsAttributeList myAttributeList;
      myAttributeList << myTaxonFieldIndex << myXFieldIndex << myYFieldIndex << myFrequencyFieldIndex;
      myLocalitiesCount = 0;
      myProvider->select( myAttributeList );
      //myProvider->rewind(); //not needed
      while ( myProvider->nextFeature( f ) )
      {
        qDebug("Getting feature from provider");
        qDebug((f.attributeMap()[ myTaxonFieldIndex  ]).toString());
        qDebug(myTaxon);
        if ( (f.attributeMap()[ myTaxonFieldIndex  ]).toString() == myTaxon )
        {
          QgsPoint myPoint;
          if ( myLayer->geometryType() == QGis::Point )
          {
            QgsGeometry * myGeometry = f.geometry();
            myPoint = myGeometry->asPoint();
          }
          else //use field attributes for x,y then
          {
            myPoint.setX( (f.attributeMap()[ myXFieldIndex  ]).toDouble() );
            myPoint.setY( (f.attributeMap()[ myYFieldIndex  ]).toDouble() );
          }
          OmgLocality myLocality;
          myLocality.setId( QString::number( f.id() ) ); //@todo use accession code if selected
          myLocality.setLabel( myTaxon );
          myLocality.setLongitude(myPoint.x());
          myLocality.setLatitude(myPoint.y());
          // use abundance if a valid field selected (0 index == [None] in combo)
          if (myFrequencyFieldIndex >= 0 ) 
          {
            myLocality.setAbundance( ( f.attributeMap()[ myFrequencyFieldIndex  ] ).toInt()  );
          }
          else //default to abundance of 1
          {
            myLocality.setAbundance( 1 );
          }
          if (myLocality.isValid())
          {
            qDebug("Locality added");
            myLocalities.push_back( myLocality );
            myLocalitiesCount ++;
          }
          else
          {
            qDebug("Locality was not added since it is invalid");
            qDebug(myLocality.toString());
          }
        }
      }
    }
    else //load from text file
    {
      myLocalitiesCount = loadLocalities(myLocalities, mySpeciesFileName, myTaxon);
    }

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
      //set the coordinate system
      QString myCoordSystem, myCoordSystemName;

      if ( mpOccurrencesPage->radUseOccurrencesLayer->isChecked( ))
      {
        QgsVectorLayer * myLayer = getOccurrenceLayer( field("OccurrencesQGISLayer").toInt() );
        int mySrsId = myLayer->crs().srsid();
        QgsCoordinateReferenceSystem srs( mySrsId, QgsCoordinateReferenceSystem::InternalCrsId );
        myCoordSystemName = srs.description();
        myCoordSystem = srs.toWkt();
      }
      else
      {
        myCoordSystemName = mySettings.value( "openModeller/experimentDesigner/coordSystemName","Lat/Long WGS84" ).toString();
        QgsCoordinateReferenceSystem srs( 4326, QgsCoordinateReferenceSystem::EpsgCrsId);
        myCoordSystem = mySettings.value( "openModeller/experimentDesigner/coordSystem", srs.toWkt() ).toString();
        //test the srs to make sure it is valid
        QgsCoordinateReferenceSystem myRestoredSrs;
        myRestoredSrs.createFromWkt( myCoordSystem );
        if ( !myRestoredSrs.isValid() )
        {
          //default is used because restored is invalid
          myCoordSystem = srs.toWkt();
          myCoordSystemName = srs.description();
        }
      }
      mypModel->setCoordinateSystemName( myCoordSystemName );
      mypModel->setCoordinateSystem( myCoordSystem );

      mypModel->setSpeciesFile(mySpeciesFileName);
      mypModel->setTaxonName(myTaxon);
      //decide if we want to dynamically create a layerset from the selected QGIS rasters
      if ( ! mpLayerSetPage->radUseQGISRasters->isEnabled( ) )
      {
        //use a predefined omg layerset
        Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
        OmgLayerSet myModelLayerSet = myLayerSetMap[mpLayerSetPage->cboModelLayerSet->currentText()];
        mypModel->setCreationLayerSet(myModelLayerSet);
      }
      else
      {
        //we called preparelayerset further up for this
        mypModel->setCreationLayerSet( mQGISLayerSet );
      }
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
      //decide if we want to dynamically create a layerset from the selected QGIS rasters
      if ( ! mpLayerSetPage->radUseQGISRasters->isEnabled( ) )
      {
        Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
        QStringList myProjectionsList = mpProjectionPage->omglistProjectionLayerSets->checkedItems();
        QString myOutputFormatLayerName = mpProjectionPage->cboOutputFormatLayer->currentText();
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
      }
      else
      {
        //we called preparelayerset further up for this
        //since we are using qgis layers, only a single projection is created
        QString myOutputFormatLayerName = mpProjectionPage->cboOutputFormatLayer->currentText();
        OmgProjection * mypProjection = new OmgProjection();
        mypProjection->setOutputFormatLayerName(myOutputFormatLayerName);
        QString myRasterFormat = mySettings.value("outputFormat","GreyTiff").toString();
        mypProjection->setOmRasterFormat(myRasterFormat);
        mypProjection->setLayerSet( mQGISLayerSet );
        mypProjection->setTaxonName(myTaxon);
        mypProjection->setAlgorithm(myAlgorithm);    
        mypModel->addProjection(mypProjection);
      }

      mypExperiment->addModel(mypModel);
      //for debugging only
      //Omgui::createTextFile("/tmp/model.xml",mypModel->toXml().toLocal8Bit());
    }
  
  }
  emit experimentMaximum(mpTaxaPage->omglistTaxa->checkedItemCount() * myAlgorithmCount);
  //qDebug("Created experiment successfully");
  return mypExperiment;
}

void OmgWizard::setStatus(QString theStatus)
{
  //qDebug( theStatus.toAscii() );
}

void OmgWizard::toolConfigureLocalitiesClicked()
{
  myOptions.exec();
}

void OmgWizard::toolSelectLocalitiesFileClicked()
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
    mpTaxaPage->setSpeciesList(myFileName);

    //store directory where localities file is for next time
    QFileInfo myFileInfo(myFileName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myFileInfo.dir().path()+QString("/") );
  }
  else //read files from a dir
  {
    mpTaxaPage->omglistTaxa->clear();
    QString myDirName = QFileDialog::getExistingDirectory(this, 
        tr("Select Directory"),
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(),
        QFileDialog::ShowDirsOnly);
    if (myDirName.isEmpty()) return;
    mpTaxaPage->setSpeciesList(myDirName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myDirName + QString("/") );

  }
} //end of on_pbnSelectLocalitiesFile_clicked



bool OmgWizard::checkLayersMatch()
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

void OmgWizard::readSettings()
{
  QSettings mySettings;
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();

  //select the last model creation layer set used by getting the name from qsettings
  QString myLayerSetName = mySettings.value("openModeller/experimentDesigner/" +
      myPluginName + "/modelLayerSet").toString();
  setComboToDefault( mpLayerSetPage->cboModelLayerSet, myLayerSetName, false);
  //select the last projection layer set used by getting the name from qsettings
  myLayerSetName = mySettings.value("openModeller/experimentDesigner/" + 
      myPluginName + "/projectionLayerSet").toString();
  //set the localities filename
  // Restore the output format
  QString myOutputFormat = mySettings.value("openModeller/experimentDesigner/" + 
      myPluginName + "/outputFormatFile").toString();
  setComboToDefault( mpProjectionPage->cboOutputFormatLayer,myOutputFormat,true);
}

void OmgWizard::writeSettings()
{
  QSettings mySettings;
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  mySettings.setValue("openModeller/experimentDesigner/localitiesFileName", mSpeciesFileName);
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/modelLayerSet",
      mpLayerSetPage->cboModelLayerSet->currentText());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/outputFormatFile", mpProjectionPage->cboOutputFormatLayer->currentText());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/selectedProjectionLayerSets", mpProjectionPage->omglistProjectionLayerSets->checkedItems());
  mySettings.setValue("openModeller/experimentDesigner/" + myPluginName + "/selectedAlgorithms",
      mpAlgorithmPage->omglistAlgorithms->checkedItems());
}

void OmgWizard::refreshMapFormatCombo()
{
  if ( mpLayerSetPage->radUseQGISRasters->isChecked( )) //load from qgis layer
  {
    qDebug( "Using QGIS rasters list to fill output format combo" );
    prepareQGISLayerSet();
    QStringList myNameList = mQGISLayerSet.nameList();
    myNameList = Omgui::sortList(myNameList);
    myNameList = Omgui::uniqueList(myNameList);
    mpProjectionPage->cboOutputFormatLayer->clear();
    mpProjectionPage->cboOutputFormatLayer->insertItems(0,myNameList);
  }
  else
  {
    qDebug( "Using Layerset rasters list to fill output format combo" );
    Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
    OmgLayerSet myModelLayerSet = myLayerSetMap[mpLayerSetPage->cboModelLayerSet->currentText()];
    //OmgLayerSet myProjectionLayerSet = myLayerSetMap[cboProjectionLayerSet->currentText()];
    QStringList myNameList = myModelLayerSet.nameList();
    //myNameList << myProjectionLayerSet.nameList();
    myNameList = Omgui::sortList(myNameList);
    myNameList = Omgui::uniqueList(myNameList);
    mpProjectionPage->cboOutputFormatLayer->clear();
    mpProjectionPage->cboOutputFormatLayer->insertItems(0,myNameList);
  }
}
QStringList OmgWizard::getListWidgetEntries(const QListWidget * theListWidget)
{
  QStringList myList;
  for ( int myCounter = 0; myCounter < theListWidget->count(); myCounter++ )
  {
    myList.append(theListWidget->item(myCounter)->text());
  }
  return myList;
}

bool OmgWizard::setComboToDefault(QComboBox * thepCombo, QString theDefault, bool theAddItemFlag)
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

void OmgWizard::on_toolSelectDataDirectory_clicked()
{
  QString myFileName = QFileDialog::getExistingDirectory(
      this,
      tr("Select a directory where your data will be saved") , //caption
      mpExperimentPage->leDataDirectory->text() //initial dir
    );
  mpExperimentPage->leDataDirectory->setText(myFileName);
}

void OmgWizard::on_toolModelSettings_clicked()
{
  myModelTestOptions.exec();
}

QgsVectorLayer * OmgWizard::getOccurrenceLayer( int theId )
{
  QString myLayerId = mpOccurrencesPage->lstPointLayers->item( theId )->data( Qt::UserRole ).toString();
  QgsMapLayer * myLayer = QgsMapLayerRegistry::instance()->mapLayer( myLayerId );
  QgsVectorLayer * myVectorLayer =  qobject_cast<QgsVectorLayer *>( myLayer );
  return myVectorLayer;
}

void OmgWizard::prepareQGISLayerSet( )
{
  OmgLayerSet myLayerSet;
  myLayerSet.setGuid();
  QListWidget * mypLayers = mpLayerSetPage->lstRasterLayers;
  QList<QListWidgetItem *> myItems = mypLayers->selectedItems();
  for ( int myCounter = 0; myCounter < myItems.size(); ++myCounter )
  {
    QString myLayerId = myItems.at( myCounter )->data( Qt::UserRole ).toString();
    QgsMapLayer * myMapLayer = QgsMapLayerRegistry::instance()->mapLayer( myLayerId );
    QString myLayerName = myMapLayer->source();
    OmgLayer myLayer;
    myLayer.setName(myLayerName);
    myLayer.setType(OmgLayer::MAP);
    myLayer.setCategorical(false);
    myLayerSet.addLayer(myLayer);
    //kludge for now - set more elegantly from ui later
    if ( !myCounter )
    {
      myLayerSet.setMask( myLayer );
    }
  }
  //myLayerSet.save();
  mQGISLayerSet = myLayerSet;
}
