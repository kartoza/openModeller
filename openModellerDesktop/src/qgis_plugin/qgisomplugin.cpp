/***************************************************************************
  qgisomplugin.cpp
  openModeller Plugin for QGIS
  -------------------
         begin                : [PluginDate]
         copyright            : [(C) Your Name and Date]
         email                : [Your Email]

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*  $Id: plugin.cpp 9327 2008-09-14 11:18:44Z jef $ */

//
// QGIS Specific includes
//

#include <qgisinterface.h>
#include <qgisgui.h>
#include <qgsvectorlayer.h> 
#include <qgsrasterlayer.h> 
#include <qgsmaplayerregistry.h>
#include <omgexperimentprinter.h>


//
// Qt4 Related Includes
//

#include <QAction>
#include <QToolBar>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>

//
// openModeller Desktop Includes
//
#include "omgalgorithmmanager.h"
#include "omglayersetmanager.h"
#include "omgdatafetcherwizard.h"
#include "omgexperimentdesigner.h"
#include "omgexperimentselector.h"
#include "omgmodellerpluginregistry.h"
#include "omgqgispluginoptions.h"
#include "omgtextfilesplitter.h"
#include "omgabout.h"
#include "omgtermsandconditions.h"
#include "qgisomplugin.h"
#include "omgexperimentprogress.h"
//#include "omgrasterthreshold.h"
#include "omgui.h"  // for version, csv export
#include "omgwizard.h"


static const char * const sIdent = "$Id: plugin.cpp 9327 2008-09-14 11:18:44Z jef $";
static const QString sName = QObject::tr( "openModeller" );
static const QString sDescription = QObject::tr( "openModeller Plugin for QGIS" );
static const QString sPluginVersion = QObject::tr( "Version 0.1" );
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::UI;

//////////////////////////////////////////////////////////////////////
//
// THE FOLLOWING METHODS ARE MANDATORY FOR ALL PLUGINS
//
//////////////////////////////////////////////////////////////////////

/**
 * Constructor for the plugin. The plugin is passed a pointer
 * an interface object that provides access to exposed functions in QGIS.
 * @param theQGisInterface - Pointer to the QGIS interface object
 */
QgisOmPlugin::QgisOmPlugin( QgisInterface * theQgisInterface ):
    QgisPlugin( sName, sDescription, sPluginVersion, sPluginType ),
    mQGisIface( theQgisInterface )
{
}

QgisOmPlugin::~QgisOmPlugin()
{

}

/*
 * Initialize the GUI interface for the plugin - this is only called once when the plugin is
 * added to the plugin registry in the QGIS application.
 */
void QgisOmPlugin::initGui()
{
  mpCurrentExperiment = 0;
  //icon displayed next to tree items to show they are running
  //start with a fall back to a non animated icon
  mRunningIcon = QIcon(":/status_running.png");
  //now try to setup icon animation...
  if (!QFile::exists(":/status_running.gif"))
  {
    //logMessage(" ****** Running movie does not exist ! ******** ");
  }
  else
  {
    mpRunningMovie = new QMovie(":/status_running.gif");
    if (mpRunningMovie->isValid())
    {
      //this will enable an animated icon next to the actively run model
      connect(mpRunningMovie, SIGNAL(updated(const QRect)), this, SLOT(updateRunningIcon(const QRect)));
    }
    else
    {
      //logMessage(" ****** Running movie is invalid ! ******** ");
    }
  }

  mpOmToolBar = mQGisIface->addToolBar( tr( "openModeller " ) );
  createActions();
  createMenus();
  createToolBars();



}

void QgisOmPlugin::createMenus()
{
  mQGisIface->addPluginToMenu( tr("openModeller"), mpOpenAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpLayerSetManagerAct );
  //mQGisIface->addPluginToMenu( tr("openModeller"), mpPreAnalysisAlgorithmManagerAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpAlgorithmManagerAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpNewAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpPrintPdfAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpOptionsAct );

  mQGisIface->addPluginToMenu( tr("openModeller"), mpFetchAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpToolsFileSplitterAct );

  //mQGisIface->addPluginToMenu( tr("openModeller"), mpToolsRasterThresholdAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpExportToCsvAct );
  //mQGisIface->addPluginToMenu( tr("openModeller"), mpToolsExternalTestsAct );

  //mQGisIface->addPluginToMenu( tr("openModeller"), mpHelpAct );
  //mQGisIface->addPluginToMenu( tr("openModeller"), mpSupportedFormatsAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpLicenseAct );
  mQGisIface->addPluginToMenu( tr("openModeller"), mpAboutOmgAct );

}

void QgisOmPlugin::createToolBars()
{
  mpOmToolBar->addAction(mpOpenAct);
  mpOmToolBar->addAction(mpLayerSetManagerAct);
  mpOmToolBar->addAction(mpAlgorithmManagerAct);
  //mpOmToolBar->addAction(mpPreAnalysisAlgorithmManagerAct);
  mpOmToolBar->addAction(mpNewAct);
  mpOmToolBar->addAction(mpPrintPdfAct);
  mpOmToolBar->addAction(mpOptionsAct);
  mpOmToolBar->addAction(mpFetchAct);
  mpOmToolBar->addAction(mpToolsFileSplitterAct);
  //mpOmToolBar->addAction(mpToolsRasterThresholdAct);
  mpOmToolBar->addAction(mpExportToCsvAct);
  //mpOmToolBar->addAction(mpToolsExternalTestsAct);
}

void QgisOmPlugin::createActions()
{
  mpOpenAct = new QAction(QIcon(":/fileopen.png"), tr("&Open Experiment"), this);
  mpOpenAct->setShortcut(tr("Ctrl+O"));
  mpOpenAct->setStatusTip(tr("Open an existing experiment"));
  connect(mpOpenAct, SIGNAL(triggered()), this, SLOT(openExperiment()));

  mpNewAct = new QAction(QIcon(":/filenewExperiment.png"), tr("&New Experiment"), this);
  mpNewAct->setShortcut(tr("Ctrl+N"));
  mpNewAct->setStatusTip(tr("Create a new experiment"));
  connect(mpNewAct, SIGNAL(triggered()), this, SLOT(newExperiment()));

  mpPrintPdfAct = new QAction(QIcon(":/pdf.png"), tr("&Save as pdf"), this);
  mpPrintPdfAct->setShortcut(tr("Ctrl+P"));
  mpPrintPdfAct->setStatusTip(tr("Save the experiment report as a pdf"));
  connect(mpPrintPdfAct, SIGNAL(triggered()), this, SLOT(printPdf()));

  mpFetchAct = new QAction(QIcon(":/filefetch.png"), tr("&Search for locality data..."), this);
  mpFetchAct->setShortcut(tr("Ctrl+W"));
  mpFetchAct->setStatusTip(tr("Search for locality data on GBIF and other data sources"));
  connect(mpFetchAct, SIGNAL(triggered()), this, SLOT(fileFetch()));

  //mpToolsRasterThresholdAct = new QAction(QIcon(":/threshold.png"), tr("Compute &Thresholds and Hotspots..."), this);
  //mpToolsRasterThresholdAct->setShortcut(tr("Ctrl+t"));
  //mpToolsRasterThresholdAct->setStatusTip(tr("Perform postprocessing functions to calculate thresholds and hotspots"));
  //connect(mpToolsRasterThresholdAct, SIGNAL(triggered()), this, SLOT(toolsRasterThreshold()));

  mpToolsExternalTestsAct = new QAction(QIcon(":/external_tests.png"), tr("Run &External Tests"), this);
  mpToolsExternalTestsAct->setShortcut(tr("Ctrl+e"));
  mpToolsExternalTestsAct->setStatusTip(tr("Run External Tests"));
  connect(mpToolsExternalTestsAct, SIGNAL(triggered()), this, SLOT(toolsExternalTests()));

  mpExportToCsvAct = new QAction(QIcon(":/export_csv.png"), tr("Export current experiment samples to CSV..."), this);
  //mpExportToCsvAct->setShortcut(tr("Ctrl+t"));
  mpExportToCsvAct->setStatusTip(tr("Export current experiment samples to CSV"));
  connect(mpExportToCsvAct, SIGNAL(triggered()), this, SLOT(exportSamplesToCsv()));
  mpExportToCsvAct->setEnabled( false );
      
  mpToolsFileSplitterAct = new QAction(QIcon(":/filesplitter.png"), tr("&Split Text File..."), this);
  mpToolsFileSplitterAct->setShortcut(tr("Ctrl+S"));
  mpToolsFileSplitterAct->setStatusTip(tr("Split a text file into several parts"));
  connect(mpToolsFileSplitterAct, SIGNAL(triggered()), this, SLOT(toolsFileSplitter()));

  mpOptionsAct = new QAction(QIcon(":/configure.png"), tr("&Preferences..."), this);
  mpOptionsAct->setShortcut(tr("Ctrl+P"));
  mpOptionsAct->setStatusTip(tr("Configure preferences for this application"));
  connect(mpOptionsAct, SIGNAL(triggered()), this, SLOT(settingsOptions()));

  mpAboutOmgAct = new QAction(QIcon(":/om_logo.png"), tr("About &openModeller Desktop"), this);
  mpAboutOmgAct->setStatusTip(tr("Show this application's About box"));
  connect(mpAboutOmgAct, SIGNAL(triggered()), this, SLOT(about()));

  mpSupportedFormatsAct = new QAction(tr("Supported Raster Formats"), this);
  mpSupportedFormatsAct->setStatusTip(tr("Show Supported Raster Formats (read)"));
  connect(mpSupportedFormatsAct, SIGNAL(triggered()), this, SLOT(supportedFormats()));

  mpLicenseAct = new QAction(tr("License Agreement"), this);
  mpLicenseAct->setStatusTip(tr("Show this application's licensing terms"));
  connect(mpLicenseAct, SIGNAL(triggered()), this, SLOT(showLicense()));

  mpHelpAct = new QAction(QIcon(":/help.png"),tr("Documentation"), this);
  mpHelpAct->setStatusTip(tr("Show help text for openModeller Desktop"));
  connect(mpHelpAct, SIGNAL(triggered()), this, SLOT(showHelp()));

  mpSurveyAct = new QAction(QIcon(":/survey.png"),tr("User Survey"), this);
  mpSurveyAct->setStatusTip(tr("Participate in the  openModeller Desktop user survey"));
  connect(mpSurveyAct, SIGNAL(triggered()), this, SLOT(showSurvey()));

  mpLayerSetManagerAct = new QAction(QIcon(":/layersetmanager.png"),tr("LayerSets"), this);
  mpLayerSetManagerAct->setStatusTip(tr("Edit layerSets"));
  connect(mpLayerSetManagerAct, SIGNAL(triggered()), this, SLOT(showLayerSetManager()));

  mpAlgorithmManagerAct = new QAction(QIcon(":/algorithmmanager.png"),tr("Algorithm profiles"), this);
  mpAlgorithmManagerAct->setShortcut(tr("Ctrl+A"));
  mpAlgorithmManagerAct->setStatusTip(tr("Edit algorithm profiles"));
  connect(mpAlgorithmManagerAct, SIGNAL(triggered()), this, SLOT(showAlgorithmManager()));

  mpPreAnalysisAlgorithmManagerAct = new QAction(QIcon(":/pre_analysis.png"), tr("New Pre-Analysis"), this);
  mpPreAnalysisAlgorithmManagerAct->setStatusTip(tr("Perform pre-analysis on layersets"));
  connect(mpPreAnalysisAlgorithmManagerAct, SIGNAL(triggered()), this, SLOT(showPreAnalysisAlgorithmManager()));

}
//method defined in interface
void QgisOmPlugin::help()
{
  //implement me!
}

void QgisOmPlugin::showLayerSetManager()
{
 OmgLayerSetManager myManager;
 myManager.show();
 myManager.exec();
      
}

void QgisOmPlugin::showAlgorithmManager()
{
 //OmgAlgorithmManager myManager(this,Qt::Tool);
 OmgAlgorithmManager myManager( mQGisIface->mainWindow(),Qt::Dialog );
 myManager.exec();
      
}

void QgisOmPlugin::fileFetch()
{
  OmgDataFetcherWizard myWizard;
  connect(&myWizard, SIGNAL(shapefileReady( QString )),
      this, SLOT(showShapefile( QString )));
  myWizard.exec();
}

void QgisOmPlugin::showShapefile( QString theShapefile )
{
  //QMessageBox::information(mQGisIface->mainWindow(),"Shp created","A new shapefile was created :" + theShapefile); 
  QFileInfo myMapFileInfo ( theShapefile );
  QgsVectorLayer * mypLayer = new QgsVectorLayer ( myMapFileInfo.filePath(),
            myMapFileInfo.completeBaseName(), "ogr" );
  QgsMapLayerRegistry::instance()->addMapLayer( mypLayer );
}

void QgisOmPlugin::settingsOptions()
{
  OmgQgisPluginOptions myOptions( mQGisIface->mainWindow()  );
  myOptions.exec();
  QSettings mySettings;
  // if it's using a plugin other than Local, disable Pre-Analysis
  if (mySettings.value("openModeller/modellerPluginType", "").toString() == "Local Modeller Plugin")
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(true);
  } else
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(false);
  }
}

bool QgisOmPlugin::toolsFileSplitter()
{
  OmgTextFileSplitter mySplitter;
  if (mySplitter.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
bool QgisOmPlugin::toolsRasterThreshold()
{
  if ( !mpCurrentExperiment )
  {
    return false;
  }
  OmgRasterThreshold myRasterThreshold;
  myRasterThreshold.setExperiment(mpCurrentExperiment);
  if (myRasterThreshold.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}
*/

void QgisOmPlugin::exportSamplesToCsv()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsCsvDir",".").toString();

  //create a file dialog using the filter list generated above
  QFileDialog * myQFileDialog = new QFileDialog
  (
    mQGisIface->mainWindow(),
    QFileDialog::tr("Save samples to delimited file"),
    myLastUsedDir,
    "Comma delimited (*.csv);;Tab delimited (*.txt)"
  );
  myQFileDialog->setFileMode(QFileDialog::AnyFile);
  myQFileDialog->setAcceptMode(QFileDialog::AcceptSave);

  //prompt the user for a filename
  QString myOutputFileName; 
  if (myQFileDialog->exec() == QDialog::Accepted)
  {
    QStringList myFiles = myQFileDialog->selectedFiles();
    if (!myFiles.isEmpty())
    {
      myOutputFileName = myFiles[0];
    }
  }
  QString myDelimiter;
  if (!myOutputFileName.isEmpty())
  {
    if (myQFileDialog->selectedFilter()=="Comma delimited (*.csv)")
    {
      //ensure the user never ommitted the extension from the filename
      if (!myOutputFileName.toUpper().endsWith(".CSV")) 
      {
        myOutputFileName+=".csv";
      }
      myDelimiter = ",";
    }
    else if (myQFileDialog->selectedFilter()=="Tab delimited (*.txt)")
    {
      //ensure the user never ommitted the extension from the filename
      if (!myOutputFileName.toUpper().endsWith(".TXT")) 
      {
        myOutputFileName+=".txt";
        myDelimiter = "\t";
      }
    }
    else
    {
      QMessageBox::warning( mQGisIface->mainWindow(),tr("openModeller Desktop"),tr("Unknown delimited format: ") +
             myQFileDialog->selectedFilter());
      delete myQFileDialog;
      return;
    }
    if ( mpCurrentExperiment->isCompleted() )
    {
      //loop through models for the first alg, writing each ones samples to a file
      int myCount = mpCurrentExperiment->count();
      if ( myCount < 1 )
      {
        QMessageBox::warning( mQGisIface->mainWindow(),tr("openModeller Desktop"),tr("This experiment has no models, nothing to export.") );
      }
      QString myAlgorithmName = mpCurrentExperiment->getModel(0)->algorithm().name();
      for (int i=0; i<myCount; i++)
      {
        OmgModel * mypModel=mpCurrentExperiment->getModel(i);
        bool myWriteHeadersFlag = true;
        if ( i > 0) 
        {
          myWriteHeadersFlag = false;
        }
        bool myAppendModeFlag = true;
        //we are going to write out the csv only for the first alg
        //because all subsequent modes will have the same samples as the 
        //first alg.
        if ( mypModel->algorithm().name() != myAlgorithmName )
        {
          break;
        }
        Omgui::createTextFile( myOutputFileName , 
            mypModel->localitiesToCsv( myDelimiter, myWriteHeadersFlag ), 
            myAppendModeFlag );
      }
    }
    //QFile::copy(mpCurrentModel->csvFileName(), myOutputFileName);
    myQSettings.setValue("openModeller/lastSaveAsCsvDir", myQFileDialog->directory().dirName());
    QMessageBox::information( mQGisIface->mainWindow(),tr("openModeller Desktop"),tr("The occurrence sample data has been saved as %1.").arg( myOutputFileName ) );
  }
  delete myQFileDialog;
}


void QgisOmPlugin::openExperiment()
{
  OmgExperimentSelector mySelector( mQGisIface->mainWindow() );
  mySelector.hidePreAnalysis();
  //mySelector.show();
  connect(&mySelector, SIGNAL(loadExperiment(OmgExperiment *)), this, SLOT(loadExperiment(OmgExperiment *)));
  connect(&mySelector, SIGNAL(loadPreAnalysis(OmgPreAnalysis *)), this, SLOT(setCurrentPreAnalysis(OmgPreAnalysis *)));
  mySelector.exec();
}

void QgisOmPlugin::newExperiment()
{
  //fist check we have some layersets available and if not 
  //suggest to the user to do that first!
  bool myHasRastersFlag = false;
  QMap< QString, QgsMapLayer * > myLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap< QString, QgsMapLayer *>::iterator it;
  for ( it = myLayers.begin(); it != myLayers.end() ; ++it )
  {
    QgsMapLayer * myLayer = it.value();
    if ( myLayer->type() == QgsMapLayer::RasterLayer ) 
    {
      myHasRastersFlag = true;
    }
  }
  if ( Omgui::getAvailableLayerSets().count() < 1  && ! myHasRastersFlag )
  {
    QMessageBox myMessage;
    myMessage.setText(tr("You have not created any LayerSets yet. Before attempting to create an") +
                      tr(" experiment you need to have at least one LayerSet defined or you should") +
                      tr("have one or more raster layers loaded in QGIS. Would") +
                      tr(" you like to open the LayerSet Manager now?"));
    myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (myMessage.exec()) {
      case QMessageBox::Yes:
        // yes was clicked
        showLayerSetManager();
        return;
        break;
      case QMessageBox::No:
        // no was clicked
        return;
        break;
      default:
        // should never be reached
        break;
    }
  }


  OmgWizard myWizard( mQGisIface->mainWindow() );
  connect(&myWizard, SIGNAL(experimentCreated(OmgExperiment *)), 
      this, SLOT(setCurrentExperiment(OmgExperiment *)));
  myWizard.exec();
  /*
  OmgExperimentDesigner myOmgExperimentDesigner( mQGisIface->mainWindow() ) ;
  connect(&myOmgExperimentDesigner, SIGNAL(experimentCreated(OmgExperiment *)), 
      this, SLOT(setCurrentExperiment(OmgExperiment *)));
  myOmgExperimentDesigner.exec();
  */
}

//differs from setCurrentExperiment in that this will add layers into qgis toc too
void QgisOmPlugin::loadExperiment( OmgExperiment * thepExperiment )
{
  if (mpCurrentExperiment)
  {
    delete mpCurrentExperiment; 
  }
  if (mpCurrentPreAnalysis)
  {
    delete mpCurrentPreAnalysis;
  }
  mpCurrentExperiment = thepExperiment;
  if ( mpCurrentExperiment->isCompleted() )
  {
    //loop through projections adding each to the toc
    int myCount = mpCurrentExperiment->count();
    for (int i=0; i<myCount; i++)
    {
      OmgModel * mypModel=mpCurrentExperiment->getModel(i);
      if (mypModel==NULL)
      {
        continue;
      }
      //determine the correct parent for this model in the tree
      QString myName = mypModel->taxonName() + " - " + mypModel->algorithm().name();
      qDebug(" Loading projections ");
      //now the model - only add it if its completed
      if (mypModel->isCompleted())
      {
        QListIterator<OmgProjection *> myIterator(mypModel->projectionList());
        while (myIterator.hasNext())
        {
          OmgProjection * mypProjection = myIterator.next();
          myName += " - " + mypProjection->layerSet().name();
          qDebug( mypProjection->rawImageFileName() );
          QString myRasterFile (mypProjection->workDir() + mypProjection->rawImageFileName());
          if (!QFile::exists(myRasterFile))
          {
            qDebug("Layer does not exist in file system");
          }
          else //layer exists
          {
            QFileInfo myRasterFileInfo(myRasterFile);
            QgsRasterLayer * mypRasterLayer = new QgsRasterLayer(myRasterFileInfo.filePath(), 
                myRasterFileInfo.completeBaseName());

            if (mypRasterLayer->isValid())
            {
              QString myFormat = mypProjection->omRasterFormat();
              //float myInputNoData = Omgui::getOutputFormatNoData().value(myFormat);
              float myFormatMinimum = Omgui::getOutputFormatRanges().value(myFormat).first;
              float myFormatMaximum = Omgui::getOutputFormatRanges().value(myFormat).second;
              mypRasterLayer->setDrawingStyle(QgsRasterLayer::SingleBandPseudoColor);
              mypRasterLayer->setColorShadingAlgorithm(QgsRasterLayer::PseudoColorShader);  
              //mypRasterLayer->setMinimumValue(mypRasterLayer->grayBandName(),myFormatMinimum, false);
              //mypRasterLayer->setMaximumValue(mypRasterLayer->grayBandName(),myFormatMaximum);
              //mypRasterLayer->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum, false);
              qDebug("Raster Layer is valid");
              // Add the layer to the Layer Registry
              QgsMapLayerRegistry::instance()->addMapLayer(mypRasterLayer, TRUE);
            } //raster is valid
            else
            {
              qDebug("Raster Layer is NOT valid");
            }
          }//raster exists
        }
        Omgui::loadModelOccurrencesLayer( mypModel );
      }
    }
  }
  else
  {
	//Initialise the modeller adapter

	/* This is the Abstract plugin type. Depending on the users
	 * options, it will be initialised to either the OmgModellerLocalPlugin
	 * or OmgModellerWebServices plugin (or possibly other plugin types in 
	 * the future) 
	*/
	//qDebug("Experiment designer calling OmGui to get the modeller plugin");
	OmgModellerPluginInterface * mypModellerPlugin;
	mypModellerPlugin = OmgModellerPluginRegistry::instance()->getPlugin();
	//qDebug("Experiment designer testing if returned modeller plugin is ok");
	if(!mypModellerPlugin)
	{
	  //this is bad! TODO notify user he has no useable adapters
	  //TODO handle this more gracefully than asserting!
	  //qDebug("Experiment designer Error no valid modelling adapters could be loaded");
	  QMessageBox::critical( 0,tr("openModeller Desktop"),
	  tr("No modelling plugins could be found.\nPlease report this problem to "
	  "your system administrator or the openModeller developers."));
	  //assert ("Undefined adapter type in __FILE__  , line  __LINE__");
	  return;
	}
	else
	{
	  //qDebug("Plugin is good to go....");
	}
	mpCurrentExperiment->setModellerPlugin(mypModellerPlugin);
    OmgExperimentProgress myProgress;
    myProgress.setExperiment( mpCurrentExperiment );
    myProgress.exec();
  }
  mpExportToCsvAct->setEnabled( true );
  //QMessageBox::information( mQGisIface->mainWindow(), "New experiment created","Your experiment has been started");
}

//differs from loadExperiment in that this will NOT add layers into qgis toc too
void QgisOmPlugin::setCurrentExperiment( OmgExperiment * thepExperiment )
{
  if (mpCurrentExperiment)
  {
    delete mpCurrentExperiment; 
  }
  if (mpCurrentPreAnalysis)
  {
    delete mpCurrentPreAnalysis;
  }
  mpCurrentExperiment = thepExperiment;
  if ( mpCurrentExperiment->isCompleted() )
  {
    mpExportToCsvAct->setEnabled( true );
  }
  else
  {
    OmgExperimentProgress myProgress;
    myProgress.setExperiment( mpCurrentExperiment );
    myProgress.exec();
    mpExportToCsvAct->setEnabled( true );
  }
  //QMessageBox::information( mQGisIface->mainWindow(), "New experiment created","Your experiment has been started");
}

void QgisOmPlugin::runExperiment()
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  mpRunningMovie->start();

  //make sure the modeller plugin is set...
  OmgModellerPluginInterface * mypModellerPlugin = 
    OmgModellerPluginRegistry::instance()->getPlugin();
  mpCurrentExperiment->setModellerPlugin(mypModellerPlugin);


  //
  // Get the messenger instance that arbitrates comms 
  // between the plugin and our gui
  //
  const OmgPluginMessenger * mypMessenger = 
    mpCurrentExperiment->modellerPlugin()->getMessenger();

  QSettings mySettings;
  /*
  //
  // Disconnect the experiment and messenger in case
  // we are resuming / rerunning the same experiment
  //
  disconnect(mpCurrentExperiment, SIGNAL(experimentProgress(int )),
      this, SLOT(setExperimentProgress(int)));
  disconnect(mpCurrentExperiment, SIGNAL(experimentMaximum(int )),
      this, SLOT(setExperimentMaximum(int)));
  disconnect(mpCurrentExperiment, SIGNAL(logMessage(QString )),
      this, SLOT(logMessage(QString)));
  disconnect(mpCurrentExperiment, SIGNAL(experimentStopped()),
      this, SLOT(experimentStopped()));
  disconnect(mpCurrentExperiment, SIGNAL(modelCompleted(QString)),
      this, SLOT(modelCompleted(QString)));
  // disconnect the signals from the modeller adapter
  // used for monitoring the progress of individual models
  disconnect(mypMessenger, SIGNAL(modelCreationProgress(QString,int )),
      this, SLOT(setModelCreationProgress(QString,int)));
  disconnect(mypMessenger, SIGNAL(modelProjectionProgress(QString,int )),
      this, SLOT(setModelProjectionProgress(QString,int)));
  disconnect(mypMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(logMessage(QString,QString)));
  disconnect(mypMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(logError(QString,QString)));

  //
  // Connect the signals of our om experiment to our progress monitor
  // Used for showing progress of the experiment as it runs
  //
  connect(mpCurrentExperiment, SIGNAL(experimentProgress(int )),
      this, SLOT(setExperimentProgress(int)));
  connect(mpCurrentExperiment, SIGNAL(experimentMaximum(int )),
      this, SLOT(setExperimentMaximum(int)));
  connect(mpCurrentExperiment, SIGNAL(logMessage(QString )),
      this, SLOT(logMessage(QString)));
  connect(mpCurrentExperiment, SIGNAL(experimentStopped()),
      this, SLOT(experimentStopped()));
  // Connect the signals from the modeller adapter
  // used for monitoring the progress of individual models
  connect(mypMessenger, SIGNAL(modelCreationProgress(QString,int )),
      this, SLOT(setModelCreationProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelProjectionProgress(QString,int )),
      this, SLOT(setModelProjectionProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(logMessage(QString,QString)));
  connect(mypMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(logError(QString,QString)));
  connect(mpCurrentExperiment, SIGNAL(modelCompleted(QString)),
      this, SLOT(modelCompleted(QString)));

  mpModelProgress->show();
  mpExperimentProgress->show();

  //reconfigure the run tool to cancel rather
  mpRunAct->setIcon(QIcon(":/status_aborted.png"));
  mpRunAct->setText(tr("Cancel cu&rrent experiment"));
  mpRunAct->setShortcut(tr("Ctrl+R"));
  mpRunAct->setStatusTip(tr("Cancel current experiment"));
  disconnect(mpRunAct);
  connect(mpRunAct, SIGNAL(triggered()), this, SLOT(stopExperiment()));
  disableControlsWhileRunning();

  //clear the log file first
  bool myLogToFileFlag = mySettings.value("openModeller/logToFile", false).toBool();
  if (myLogToFileFlag)
  {
    QString myFileName = mySettings.value("openModeller/logFile","/tmp/omglog.txt").toString();
    QFile myFile( myFileName );
    if ( myFile.exists() )
    {
      myFile.remove(myFileName);
    }
  }


  //
  // Show some help info in the textbrowser to let the user
  // know what is going on
  //
  QString myString;
  myString = "<h1>" + tr("Experiment in progress") + "</h1>\n";
  myString += "<table>";
  myString += experimentHelpText();
  // finish up

  myString += "</table>";
  tabModelViewDetails->setEnabled(true);
  setReportStyleSheet();
  tbReport->setHtml(myString); 

  //
  // Make sure this comes last
  //
  */

  bool myThreadingFlag = mySettings.value("openModeller/runExperimentInThread", true).toBool();
  if (myThreadingFlag)
  {
    //we call the qthread start method which in turn will
    //call the run method of our experiment after creating
    //a new thread.
    //logMessage ("Running experiment in its own thread is enabled");
    mpCurrentExperiment->start();
  }
  else
  {
    //logMessage ("Running experiment in its own thread is disabled");
    mpCurrentExperiment->run();
  }
}

void QgisOmPlugin::showLicense()
{
  OmgTermsAndConditions myTerms( mQGisIface->mainWindow(),Qt::Dialog );
  myTerms.show();
  myTerms.exec();
}

void QgisOmPlugin::about( )
{
  OmgAbout myAbout( mQGisIface->mainWindow(),Qt::Dialog );
  myAbout.show();
  myAbout.exec();
}

void QgisOmPlugin::updateRunningIcon(const QRect & theRect )
{
  //logMessage("Updating running icon...");
  mRunningIcon = QIcon(mpRunningMovie->currentPixmap());
}

void QgisOmPlugin::printPdf()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsPdfDir",".").toString();

  //create a file dialog 
  QFileDialog * mypFileDialog = new QFileDialog(
      mQGisIface->mainWindow(),
      QFileDialog::tr("Save experiment report to portable document format (.pdf)"),
      myLastUsedDir,
      tr("Portable Document Format (*.pdf)") );
  mypFileDialog->setFileMode(QFileDialog::AnyFile);
  mypFileDialog->setAcceptMode(QFileDialog::AcceptSave);

  //prompt the user for a filename
  QString myOutputFileName; 
  if (mypFileDialog->exec() == QDialog::Accepted)
  {
    QStringList myFiles = mypFileDialog->selectedFiles();
    if (!myFiles.isEmpty())
    {
      myOutputFileName = myFiles[0];
    }
  }

  if (!myOutputFileName.isEmpty())
  {
    if (mypFileDialog->selectedFilter()==tr("Portable Document Format (*.pdf)"))
    {
      //ensure the user never ommitted the extension from the filename
      if (!myOutputFileName.toUpper().endsWith(".PDF")) 
      {
        myOutputFileName+=".pdf";
      }

      QPrinter myPrinter;
      myPrinter.setPageSize(QPrinter::A4);
      myPrinter.setOutputFormat(QPrinter::PdfFormat);
      myPrinter.setOutputFileName(myOutputFileName);
      myPrinter.setDocName("openModellerDesktopExperimentReport");
      // set the experiment printer up
      OmgExperimentPrinter * mypExperimentPrinter = new OmgExperimentPrinter();
      QProgressDialog myProgressDialog;
      myProgressDialog.setValue(0);
      myProgressDialog.setLabelText(tr("Please wait while your report is generated"));
      myProgressDialog.exec();
      myProgressDialog.setWindowModality(Qt::WindowModal);
      myProgressDialog.setAutoClose(true);
      connect(mypExperimentPrinter, SIGNAL(progress(int)), &myProgressDialog, SLOT(setValue(int)));
      connect(mypExperimentPrinter, SIGNAL(maximum(int)), &myProgressDialog, SLOT(setMaximum(int)));
      mypExperimentPrinter->setExperiment(mpCurrentExperiment);
      mypExperimentPrinter->setPrinter(myPrinter);
      //crashes when I use it in thread mode..
      //mypExperimentPrinter->start();
      mypExperimentPrinter->run();
      myProgressDialog.close();
      delete mypExperimentPrinter;
      //new to qt4.3 - bounce dock icon or flash menu bar entry
      //only does anything if this window is not focussed
      QApplication::alert( mQGisIface->mainWindow() );
    }
    else
    {
      QMessageBox::warning( mQGisIface->mainWindow(),tr("openModeller Desktop"),tr("Unknown delimited format: ") +
             mypFileDialog->selectedFilter());

    }
    myQSettings.setValue("openModeller/lastSaveAsPdfDir", mypFileDialog->directory().absolutePath());
  }
  delete mypFileDialog;
}

//
//
// Unload the plugin by cleaning up the GUI
void QgisOmPlugin::unload()
{
  // remove the GUI
  //mQGisIface->removePluginMenu( "&openModeller", mpRunOmAction );
  //mQGisIface->removeToolBarIcon( mpRunOmAction );
  //delete mpRunOmAction;
  //delete mpOmToolBar;
}


//////////////////////////////////////////////////////////////////////////
//
//
//  THE FOLLOWING CODE IS AUTOGENERATED BY THE PLUGIN BUILDER SCRIPT
//    YOU WOULD NORMALLY NOT NEED TO MODIFY THIS, AND YOUR PLUGIN
//      MAY NOT WORK PROPERLY IF YOU MODIFY THIS INCORRECTLY
//
//
//////////////////////////////////////////////////////////////////////////


/**
 * Required extern functions needed  for every plugin
 * These functions can be called prior to creating an instance
 * of the plugin class
 */
// Class factory to return a new instance of the plugin class
QGISEXTERN QgisPlugin * classFactory( QgisInterface * theQgisInterfacePointer )
{
  return new QgisOmPlugin( theQgisInterfacePointer );
}
// Return the name of the plugin - note that we do not user class members as
// the class may not yet be insantiated when this method is called.
QGISEXTERN QString name()
{
  return sName;
}

// Return the description
QGISEXTERN QString description()
{
  return sDescription;
}

// Return the type (either UI or MapLayer plugin)
QGISEXTERN int type()
{
  return sPluginType;
}

// Return the version number for the plugin
QGISEXTERN QString version()
{
  return sPluginVersion;
}

// Delete ourself
QGISEXTERN void unload( QgisPlugin * thePluginPointer )
{
  delete thePluginPointer;
}
