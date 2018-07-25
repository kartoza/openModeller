/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License,  or     *
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
#include "omgmainwindow.h"

#include "omgabout.h"
#include "omgalgorithm.h"
#include "omgalgorithmmanager.h"
#include "omgclimateconverter.h"
#include "omgexperimentdesigner.h"
#include "omgexperimentselector.h"
#include "omggdalrastercontour.h"
#include "omggdalwarp.h"
#include "omglayersetmanager.h"
#include "omgoptions.h"
#include "omgrasterthreshold.h"
#include "omgdatafetcherwizard.h"
#include "omgtermsandconditions.h"
#include "omgtextfilesplitter.h"
#include "omgmodeltests.h"
#include "omgtipfactory.h"
#include "omgsurveywizard.h"
#include "omgpublishtocatalogue.h"
#include "omgmodellerplugininterface.h"
#include "omgmodellerpluginregistry.h"
#include "omgpreanalysisalgorithmmanager.h"
#include "omgsupportedfileformats.h"
#include <omgui.h>  // for version

#ifdef WITH_QGIS
//this next include is COPIED into omg sources
//from qgis sources since I needed to modify it
//It is needed to enable qgis plugins to work in 
//omdesktop.
#include "qgisappinterface.h"
//
// QGIS Includes
//
#include <qgisplugin.h>
#include <qgsapplication.h>
#include <qgsfield.h>
#include <qgsmapcanvas.h>
#include <qgsmaplayerregistry.h>
#include <qgsproject.h>
#include <qgspluginregistry.h>
#include <qgsproviderregistry.h>
#include <qgsrasterlayer.h>
#include <qgssinglesymbolrenderer.h>
#include <qgssymbol.h>
#include <qgsuniquevaluerenderer.h>
#include <qgsvectordataprovider.h>
#include <qgsvectorlayer.h>

//
// QGIS Map tools
//
#include "qgsmaptoolpan.h"
#include "qgsmaptoolzoom.h"
//#include "qgsmaptoolidentify.h"
//#include "qgsmeasure.h"
//
// These are the other headers for available map tools (not used in this example)
//
//#include "qgsmaptoolcapture.h"
//#include "qgsmaptoolselect.h"
//#include "qgsmaptoolvertexedit.h"
#endif

#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFrame>
#include <QHeaderView>
#include <QImage>
#include <QLayout>
#include <QLibrary>
#include <QListWidget>
#include <QMap>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QPixmap>
#include <QPrinter>
#include <QScrollArea>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QTextDocument>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeWidget>
#include <QWhatsThis>
#include <QWidget>

//needed for loading qgis plugins
typedef QgisPlugin *create_ui(QgisInterface * qI);

OmgMainWindow::OmgMainWindow(QWidget* parent, Qt::WFlags fl)
    : QMainWindow(parent,fl),
      //needed for loading qgis plugins
      mpQgisInterface(new QgisAppInterface(this)),
      mpCurrentModel(new OmgModel()),
      mpLocalitiesModel(new OmgLocalitiesModel())

{
  //required by Qt4 to initialise the ui
  setupUi(this);
  
  //setUnifiedTitleAndToolBarOnMac ( true );
  connect(splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(setTreeColumnSizes(int, int)));
  connect(cboReportMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(reportModeChanged(QString)));
 
  // Set view mode
  setViewMode("Browse");


  //Set up experiment tree
  treeExperiment->clear();
  tableView->setModel(mpLocalitiesModel); 
  setEnableModelDetailWidgets(false);

  //QScrollArea *mypScrollArea = new QScrollArea(frameImage);
  //mypScrollArea->setWidget(mPictureWidget);
  //setCentralWidget(mypScrollArea);
  //setCentralWidget(frameImage);
  createActions();
  createTrayIcon();
  createMenus();
  createToolBars();
  createStatusBar();

#ifdef WITH_QGIS
  //make sure the toolbars and actions are created first
  //ie dont move this defined block above the preceding lines ^
  
  // Instantiate Provider Registry
  QString myPluginDirName        = QgsApplication::pluginPath(); 
  QgsProviderRegistry::instance(myPluginDirName);

  // Create the Map Canvas
  mpMapCanvas= new QgsMapCanvas(tabMap, 0);
  //qDebug(mpMapCanvas->extent().stringRep(2));
  mpMapCanvas->enableAntiAliasing(true);
  mpMapCanvas->useImageToRender(false);
  mpMapCanvas->setCanvasColor(QColor(255, 255, 255));
  mpMapCanvas->freeze(false);
  mpMapCanvas->setVisible(true);
  mpMapCanvas->refresh();
  mpMapCanvas->show();
  //load the north arrow and scale bar plugins
  //note: any qgis plugins found will be loaded automatically
  //so just drop in additional ones as needed
#ifdef WIN32
  QString pluginExt = "*.dll";
#else
  QString pluginExt = "*.so*";
#endif

  // check all libs in the current plugin directory and get name and descriptions
  QDir myPluginDir(myPluginDirName, pluginExt, QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::NoSymLinks);

  for (uint i = 0; i < myPluginDir.count(); i++)
  {
    QString myFullPath = myPluginDirName + QDir::separator() + myPluginDir[i];
    qDebug("Examining " + myFullPath.toLocal8Bit());
    //check its a plugin we actually want
    //on some platforms e.g. linux we dont have plugins
    //bundled specifically with openModeller Desktop app
    //and some of them may cause segfault so we screen 
    //for just the ones we want
    bool myPluginWantedFlag=false;
    QStringList myPluginList;
    myPluginList << "north" << "copyright" << "scalebar";
    QStringListIterator myIterator(myPluginList);
    while (myIterator.hasNext())
    {
      if (myFullPath.contains(myIterator.next()))
      {
        myPluginWantedFlag=true;
        break; 
      }
    }
    if (!myPluginWantedFlag)
    {
      continue;
    }


    QLibrary *myLib = new QLibrary(myFullPath);
    bool myLoadedFlag = myLib->load();
    if (myLoadedFlag)
    {
        //purposely leaving this one to stdout!
        std::cout << "Loaded " << myLib->library().toLocal8Bit().data() << std::endl;
        
        name_t * myName = (name_t *) myLib->resolve("name");
        description_t *  myDescription = (description_t *)  myLib->resolve("description");
        version_t *  myVersion =  (version_t *) myLib->resolve("version");
        if (myName && myDescription  && myVersion )
        {
          //check if the plugin was active on last session
          QString myEntryName = myName();
          // Windows stores a "true" value as a 1 in the registry so we
          // have to use readBoolEntry in this function

          qDebug("Loading plugin: " + myEntryName.toLocal8Bit());

          loadQGisPlugin(myName(), myDescription(), myFullPath);
        }
        else
        {
          qDebug("Failed to get name, description, or type for " + 
              myLib->library().toLocal8Bit());
        }
    }
    else
    {
      qDebug("Failed to load " + myLib->library().toLocal8Bit());
      qDebug("Reason: " + myLib->errorString().toLocal8Bit());
    }
    delete myLib;
  }

  
  // Lay our widgets out in the main window
  delete lblOutputMap; //dont need this when using qgis integration
  tabMap->layout()->addWidget(mpMapCanvas);
  //create the maptools
  mpPanTool = new QgsMapToolPan(mpMapCanvas);
  mpPanTool->setAction(mpPanAct);
  mpZoomInTool = new QgsMapToolZoom(mpMapCanvas, FALSE); // false = in
  mpZoomInTool->setAction(mpZoomInAct);
  mpZoomOutTool = new QgsMapToolZoom(mpMapCanvas, TRUE ); //true = out
  mpZoomOutTool->setAction(mpZoomOutAct);
  /*
  mpIdentifyTool = new QgsMapToolIdentify(mpMapCanvas);
  mpIdentifyTool->setAction(mpIdentifyAct);
  mpMeasureTool = new QgsMeasure(FALSE , mpMapCanvas); //FALSE = non area mode
  mpMeasureTool->setAction(mpMeasureAct);
  mpMeasureAreaTool = new QgsMeasure(TRUE , mpMapCanvas); //TRUE = area mode
  mpMeasureAreaTool->setAction(mpMeasureAreaAct);
  */

#endif //with qgis

#ifdef OMG_NO_EXPERIMENTAL
  //* Disable these tools until they are properly useable
  mpToolsContourAct->setVisible(false);
  mpToolsWarpAct->setVisible(false);
  mpCloseAct->setVisible(false);
  mpToolsClimateConvertAct->setVisible(false);
  //mpToolsRasterThresholdAct->setVisible(false);

  mpDataPrepToolBar->removeAction(mpToolsWarpAct);
  mpDataPrepToolBar->removeAction(mpToolsClimateConvertAct);
  mpPostProcessingToolBar->removeAction(mpToolsContourAct);
  //mpPostProcessingToolBar->removeAction(mpToolsRasterThresholdAct);

  mpSaveAct->setVisible(false);
  mpFileToolBar->removeAction(mpSaveAct);
#endif  
  

  readSettings();
  setTreeColumnSizes(0,0);
  disableMapControls();

  showHelp();

  
  //hide the report mode widgets
  //eventually delete this I think since I dont think average
  //users care about it
  cboReportMode->hide();
  lblReportMode->hide();
  
  // default sort mode
  //mSortMode=BY_TAXON; //or BY_ALGORITHM
  mSortMode = BY_ALGORITHM;
  //set up the ossim planet widget
#ifdef WITH_OSSIMPLANET_QT
  initialiseOssimPlanet();
#else
  tabModelViewDetails->removeTab(3);
#endif
  listThumbnails->hide();

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
      logMessage(" ****** Running movie is invalid ! ******** ");
    }
  }
  //copy in the default user profiles if they dont exist
  QString myPath = Omgui::userAlgorithmProfilesDirPath();
  QStringList myList;
  //myList << "algorithm-EnvironmentalDistance-Chebyshev.xml";
  //myList << "algorithm-EnvironmentalDistance-Mahalanobis.xml";
  //myList << "algorithm-EnvironmentalDistance-Manhattan.xml";
  //myList << "algorithm-SVM-C-SVC.xml";
  //myList << "algorithm-SVM-Nu-SVC.xml";
  //myList << "algorithm-SVM-One-Class.xml";
  QStringListIterator myIterator(myList);
  while (myIterator.hasNext())
  {
    QString myProfile = myIterator.next();
    if (!QFile::exists(myPath+myProfile))
    {
      //copy from resource bundle to filesystem
      if (!QFile::exists(":/" + myProfile))
      {
        qDebug("Resouce file :/" + myProfile.toLocal8Bit()
            + " does not exist...skipping");
        continue;
      }

      bool myResult = QFile::copy(":/" + myProfile,
          myPath+myProfile);
      QString myMessage = "Copying :/" + myProfile +
        " to " + myPath + myProfile; 
      if (myResult)
      {
        myMessage += " succeeded";
      }
      else
      {
        myMessage += " failed";
      }
      qDebug(myMessage.toLocal8Bit());
    }
  }
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, false);
  tabModelViewDetails->setTabEnabled(2, false);
  tabModelViewDetails->setTabEnabled(3, true);

  mpPostProcessingMenu->setDisabled(true);
  mpPostProcessingToolBar->setDisabled(true);
}


OmgMainWindow::~OmgMainWindow()
{
}

void OmgMainWindow::loadQGisPlugin(QString theName, 
    QString theDescription, QString theFullPathName)
{
  QSettings mySettings;
  // first check to see if its already loaded
  QgsPluginRegistry *mypRegistry = QgsPluginRegistry::instance();
  QString myLibName = mypRegistry->library(theName);
  if (myLibName.length() > 0)
  {
    // plugin is loaded
    // QMessageBox::warning(this, "Already Loaded", description + " is already loaded");
  }
  else
  {
    QLibrary *mypLib = new QLibrary(theFullPathName);
    qDebug("Library name is " + mypLib->library().toLocal8Bit());

    bool myLoadedFlag = mypLib->load();
    if (myLoadedFlag)
    {
      std::cerr << "Loaded test plugin library" << std::endl;
      std::cerr << "Attempting to resolve the classFactory function" << std::endl;

      type_t *mypType = (type_t *) mypLib->resolve("type");


      switch (mypType())
      {
        case QgisPlugin::UI:
          {
            create_ui *cf = (create_ui *) mypLib->resolve("classFactory");
            if (cf)
            {
              QgisPlugin *mypPlugin = cf(mpQgisInterface);
              if (mypPlugin)
              {
                mypPlugin->initGui();
                // add it to the plugin registry
                mypRegistry->addPlugin(mypLib->library(), theName, mypPlugin);
                //add it to the qsettings file [ts]
                mySettings.setValue("/QGisPlugins/" + theName, true);
              }
              else
              {
                // something went wrong
                QMessageBox::warning(this, tr("Error Loading Plugin"), tr("There was an error loading %1."));
                //disable it to the qsettings file [ts]
                mySettings.setValue("/QGisPlugins/" + theName, false);
              }
            }
            else
            {
              //#ifdef QGISDEBUG
              std::cerr << "Unable to find the class factory for " 
                << theFullPathName.toLocal8Bit().data() << std::endl;
              //#endif
            }

          }
          break;
        case QgisPlugin::MAPLAYER:
        default:
          // type is unknown
          //#ifdef QGISDEBUG
          std::cerr << "Plugin " << theFullPathName.toLocal8Bit().data() << " did not return a valid type and cannot be loaded" << std::endl;
          //#endif
          break;
      }
    }
    else
    {
      //#ifdef QGISDEBUG
      std::cerr << "Failed to load " << theFullPathName.toLocal8Bit().data() << "\n";
      //#endif
    }
    delete mypLib;
  }
}

void OmgMainWindow::fileExit()
{
#ifdef WITH_QGIS  
  mpMapCanvas->freeze(true);
  delete mpMapCanvas;
#endif
  writeSettings();
  if (QSystemTrayIcon::isSystemTrayAvailable ())
  {
    mTrayIcon->hide();
    if (mTrayIcon) //QPointer guarded pointer
    {
      delete mTrayIcon;
    }
  }
  qApp->quit();
}

void OmgMainWindow::closeEvent(QCloseEvent *thepEvent)
{
  QSettings mySettings;
  bool myUseSytemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
  bool myMinimiseToSystemTrayFlag = mySettings.value("openModeller/closeToSystemTray", false).toBool();
  if (QSystemTrayIcon::isSystemTrayAvailable() && 
      myUseSytemTrayFlag &&
      myMinimiseToSystemTrayFlag)
  {
    QSettings mySettings;
    if (mySettings.value("openModeller/closeToSystemTray", true).toBool())
    { 
      QMessageBox::information(this, tr("Systray"),
          tr("The program will keep running in the "
            "system tray. To terminate the program, "
            "choose <b>Quit</b> in the context menu "
            "of the system tray entry, or choose exit "
            "from the file menu. You can disable this "
            "behaviour in the options dialog."));
      hide();
      thepEvent->ignore();
      return;
    }
  }
  //otherwise exit like normal
  fileExit();
}

void OmgMainWindow::fileNew()
{
  //fist check we have some layersets available and if not 
  //suggest to the user to do that first!
  if ( Omgui::getAvailableLayerSets().count() < 1)
  {
    QMessageBox myMessage;
    myMessage.setText(tr("You have not created any LayerSets yet. Before attempting to create an") +
                      tr(" experiment you need to have at least one LayerSet defined. Would") +
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


  OmgExperimentDesigner myOmgExperimentDesigner(this) ;
  connect(&myOmgExperimentDesigner, SIGNAL(experimentCreated(OmgExperiment *)), 
      this, SLOT(setCurrentExperiment(OmgExperiment *)));
  myOmgExperimentDesigner.exec();
}
void OmgMainWindow::resetExperiment()
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  QMessageBox myMessage;
  myMessage.setText(tr("Resetting an experiment will mark"
        " all models as 'not run'. You will need to rerun the "
        " experiment to have the models considered complete."
        " No existing model outputs will be deleted, but when you "
        " rerun the experiment they will be overwritten."
        " Are you sure you want to reset the experiment now?"));
  myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  switch (myMessage.exec()) 
  {
    case QMessageBox::Yes:
      // yes was clicked
      mpCurrentExperiment->reset();
      populateTree();
      return;
      break;
    case QMessageBox::No:
      // no was clicked
      //do nothing
      break;
    default:
      // should never be reached
      break;
  }
}
void OmgMainWindow::runExperiment()
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

  QSettings mySettings;
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

  bool myThreadingFlag = mySettings.value("openModeller/runExperimentInThread", true).toBool();
  if (myThreadingFlag)
  {
    //we call the qthread start method which in turn will
    //call the run method of our experiment after creating
    //a new thread.
    logMessage ("Running experiment in its own thread is enabled");
    mpCurrentExperiment->start();
  }
  else
  {
    logMessage ("Running experiment in its own thread is disabled");
    mpCurrentExperiment->run();
  }
}

void OmgMainWindow::runPreAnalysis()
{
  if (!mpCurrentPreAnalysis)
  {
    return;
  }
  mpRunningMovie->start();

  //
  // Disconnect the experiment and messenger in case
  // we are resuming / rerunning the same experiment
  //
  disconnect(mpCurrentPreAnalysis, SIGNAL(logMessage(QString )),
      this, SLOT(logMessage(QString)));
  disconnect(mpCurrentPreAnalysis, SIGNAL(preAnalysisCompleted()),
      this, SLOT(preAnalysisCompleted()));

  //
  // Connect the signals of our om experiment to our progress monitor
  // Used for showing progress of the experiment as it runs
  //
  connect(mpCurrentPreAnalysis, SIGNAL(logMessage(QString )),
      this, SLOT(logMessage(QString)));
  connect(mpCurrentPreAnalysis, SIGNAL(preAnalysisCompleted()),
      this, SLOT(preAnalysisCompleted()));

  /* maybe implemented later */
  /* //reconfigure the run tool to cancel rather
  mpRunAct->setIcon(QIcon(":/status_aborted.png"));
  mpRunAct->setText(tr("Cancel cu&rrent experiment"));
  mpRunAct->setShortcut(tr("Ctrl+R"));
  mpRunAct->setStatusTip(tr("Cancel current experiment"));
  disconnect(mpRunAct);
  connect(mpRunAct, SIGNAL(triggered()), this, SLOT(stopExperiment()));
  disableControlsWhileRunning(); */

  QSettings mySettings;
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
  myString = "<h1>" + tr("Pre-Analysis in progress...") + "</h1>\n";
  tabModelViewDetails->setEnabled(true);
  setReportStyleSheet();
  tbReport->setHtml(myString); 

  //
  // Make sure this comes last
  //

  bool myThreadingFlag = mySettings.value("openModeller/runExperimentInThread", true).toBool();
  if (myThreadingFlag)
  {
    //we call the qthread start method which in turn will
    //call the run method of our experiment after creating
    //a new thread.
    logMessage ("Running pre-analysis in its own thread is enabled");
    mpCurrentPreAnalysis->start();
  }
  else
  {
    logMessage ("Running pre-analysis in its own thread is disabled");
    mpCurrentPreAnalysis->run();
  }
}

void OmgMainWindow::closeExperiment()
{
  //qDebug("Closing current experiment...");
  clearCurrentExperiment();

}

void OmgMainWindow::fileFetch()
{
  OmgDataFetcherWizard myWizard;
  myWizard.exec();
}

/*bool OmgMainWindow::toolsConvert()
{
  OmgGdalConverter myConverter;
  if (myConverter.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}*/

bool OmgMainWindow::toolsContour()
{
  OmgGdalRasterContour myContour;
  if (myContour.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool OmgMainWindow::toolsRasterThreshold()
{
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

bool OmgMainWindow::toolsExternalTests()
{
  OmgModelTests myOmgModelTests(this);
  myOmgModelTests.setExperiment(mpCurrentExperiment);
  
  connect(&myOmgModelTests, SIGNAL(refreshExperiment()), 
      this, SLOT(refreshExperiment()));

  if( myOmgModelTests.exec() == QDialog::Accepted )
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool OmgMainWindow::toolsClimateConvert()
{
  OmgClimateConverter myConverter;
  if (myConverter.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool OmgMainWindow::toolsWarp()
{
  OmgGdalWarp myWarp;
  if (myWarp.exec()==QDialog::Accepted)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool OmgMainWindow::toolsFileSplitter()
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

bool OmgMainWindow::checkLicenseIsAgreed()
{
  QSettings mySettings;
  if (mySettings.value("licensing/termsAgreedFlag",0).toInt()==1)
  {
    return true;
  }
  else
  {
    OmgTermsAndConditions myTerms;
    if (myTerms.exec()==QDialog::Accepted)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

void OmgMainWindow::showLicense()
{
  OmgTermsAndConditions myTerms;
  myTerms.show();
  myTerms.exec();
        
}


void OmgMainWindow::about()
{
  OmgAbout myAbout;
  myAbout.show();
  myAbout.exec();
}

void OmgMainWindow::settingsOptions()
{
  OmgOptions myOptions(this);
  myOptions.exec();
  QSettings mySettings;
  if (QSystemTrayIcon::isSystemTrayAvailable())
  {
    //after the options closes lets see if the 
    //systray icon behaviour is changed
    bool myUseSytemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
    if (myUseSytemTrayFlag)
    {
      mTrayIcon->show();
    }
    else
    {
      mTrayIcon->hide();
    }
  }

  // if it's using a plugin other than Local, disable Pre-Analysis
  if (mySettings.value("openModeller/modellerPluginType", "").toString() == "Local Modeller Plugin")
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(true);
  } else
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(false);
  }
}

void OmgMainWindow::createActions()
{
  mpNewAct = new QAction(QIcon(":/filenewExperiment.png"), tr("&New Experiment"), this);
  mpNewAct->setShortcut(tr("Ctrl+N"));
  mpNewAct->setStatusTip(tr("Create a new experiment"));
  connect(mpNewAct, SIGNAL(triggered()), this, SLOT(fileNew()));

  mpPrintPdfAct = new QAction(QIcon(":/pdf.png"), tr("&Save as pdf"), this);
  mpPrintPdfAct->setShortcut(tr("Ctrl+P"));
  mpPrintPdfAct->setStatusTip(tr("Save the experiment report as a pdf"));
  connect(mpPrintPdfAct, SIGNAL(triggered()), this, SLOT(printPdf()));

  mpRefreshReportsAct = new QAction(QIcon(":/refresh.png"), tr("Refresh all reports"), this);
  mpRefreshReportsAct->setStatusTip(tr("Refresh all reports"));
  connect(mpRefreshReportsAct, SIGNAL(triggered()), this, SLOT(refreshReports()));

  mpRunAct = new QAction(QIcon(":/status_restart.png"), tr("&Run / Resume current experiment"), this);
  mpRunAct->setShortcut(tr("Ctrl+R"));
  mpRunAct->setStatusTip(tr("Run / Resume current experiment"));
  connect(mpRunAct, SIGNAL(triggered()), this, SLOT(runExperiment()));

  mpResetAct = new QAction(QIcon(":/status_reset.png"), tr("&Reset the current experiment"), this);
  mpResetAct->setStatusTip(tr("Reset the current experiment"));
  connect(mpResetAct, SIGNAL(triggered()), this, SLOT(resetExperiment()));

  mpOpenAct = new QAction(QIcon(":/fileopen.png"), tr("&Open..."), this);
  mpOpenAct->setShortcut(tr("Ctrl+O"));
  mpOpenAct->setStatusTip(tr("Open a previously saved experiment"));
  connect(mpOpenAct, SIGNAL(triggered()), this, SLOT(fileOpen()));
  mpOpenAct->setEnabled(true);
  
  mpSaveAct = new QAction(QIcon(":/filesave.png"), tr("&Save..."), this);
  mpSaveAct->setShortcut(tr("Ctrl+S"));
  mpSaveAct->setStatusTip(tr("Save the experiment"));
  connect(mpSaveAct, SIGNAL(triggered()), this, SLOT(fileSave()));
  mpSaveAct->setEnabled(true);
  

  mpCloseAct = new QAction(QIcon(":/fileclose.png"), tr("&Close"), this);
  mpCloseAct->setStatusTip(tr("Close current experiment"));
  connect(mpCloseAct, SIGNAL(triggered()), this, SLOT(closeExperiment()));
  
  mpFetchAct = new QAction(QIcon(":/filefetch.png"), tr("&Search for locality data..."), this);
  mpFetchAct->setShortcut(tr("Ctrl+W"));
  mpFetchAct->setStatusTip(tr("Search for locality data on GBIF and other data sources"));
  connect(mpFetchAct, SIGNAL(triggered()), this, SLOT(fileFetch()));

  mpExitAct = new QAction(QIcon(":/exit.png"), tr("&Exit..."), this);
  mpExitAct->setShortcut(tr("Ctrl+Q"));
  mpExitAct->setStatusTip(tr("Close this application"));
  connect(mpExitAct, SIGNAL(triggered()), this, SLOT(fileExit()));

  /*mpToolsConvertAct = new QAction(QIcon(":/tools_converter.png"), tr("Convert &GIS Formats"), this);
  mpToolsConvertAct->setShortcut(tr("Ctrl+G"));
  mpToolsConvertAct->setStatusTip(tr("Convert model and environmental data"));
  connect(mpToolsConvertAct, SIGNAL(triggered()), this, SLOT(toolsConvert()));*/

  mpToolsClimateConvertAct = new QAction(QIcon(":/hadleyicon.png"), tr("&Import Climate Data"), this);
  mpToolsClimateConvertAct->setShortcut(tr("Ctrl+I"));
  mpToolsClimateConvertAct->setStatusTip(tr("Convert climate data to generic GIS format"));
  connect(mpToolsClimateConvertAct, SIGNAL(triggered()), this, SLOT(toolsClimateConvert()));
  
  mpToolsContourAct = new QAction(QIcon(":/contour.png"), tr("&Convert to Contours"), this);
  mpToolsContourAct->setShortcut(tr("Ctrl+C"));
  mpToolsContourAct->setStatusTip(tr("Create contours at equal intervals from raster data"));
  connect(mpToolsContourAct, SIGNAL(triggered()), this, SLOT(toolsContour()));

  
  mpToolsRasterThresholdAct = new QAction(QIcon(":/threshold.png"), tr("Compute &Thresholds and Hotspots..."), this);
  mpToolsRasterThresholdAct->setShortcut(tr("Ctrl+t"));
  mpToolsRasterThresholdAct->setStatusTip(tr("Perform postprocessing functions to calculate thresholds and hotspots"));
  connect(mpToolsRasterThresholdAct, SIGNAL(triggered()), this, SLOT(toolsRasterThreshold()));

  mpToolsExternalTestsAct = new QAction(QIcon(":/external_tests.png"), tr("Run &External Tests"), this);
  mpToolsExternalTestsAct->setShortcut(tr("Ctrl+e"));
  mpToolsExternalTestsAct->setStatusTip(tr("Run External Tests"));
  connect(mpToolsExternalTestsAct, SIGNAL(triggered()), this, SLOT(toolsExternalTests()));

  mpExportToCsvAct = new QAction(QIcon(":/export_csv.png"), tr("Export current model samples to CSV..."), this);
  //mpExportToCsvAct->setShortcut(tr("Ctrl+t"));
  mpExportToCsvAct->setStatusTip(tr("Export current model samples to CSV"));
  connect(mpExportToCsvAct, SIGNAL(triggered()), this, SLOT(exportSamplesToCsv()));
      
  mpToolsWarpAct = new QAction(QIcon(":/resize.png"), tr("&Resize Raster..."), this);
  mpToolsWarpAct->setShortcut(tr("Ctrl+R"));
  mpToolsWarpAct->setStatusTip(tr("Resize a raster file"));
  connect(mpToolsWarpAct, SIGNAL(triggered()), this, SLOT(toolsWarp()));

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

  mpAvailableAlgorithmsReportAct = new QAction(QIcon(":/algorithmReport.png"),tr("Algorithm Profiles Report"), this);
  mpAvailableAlgorithmsReportAct->setStatusTip(tr("Print a report showing the various algorithm profiles that have been defined"));
  mpAvailableAlgorithmsReportAct->setEnabled(true);
  connect(mpAvailableAlgorithmsReportAct, SIGNAL(triggered()), this, SLOT(printAlgorithmProfilesReport()));

  mpExperimentReportAct = new QAction(QIcon(":/experimentReport.png"),tr("Experiment Report"), this);
  mpExperimentReportAct->setStatusTip(tr("Print a report showing all the models in this experiment"));
  mpExperimentReportAct->setEnabled(true);
  connect(mpExperimentReportAct, SIGNAL(triggered()), this, SLOT(printExperimentReport()));

  mpPreAnalysisAlgorithmManagerAct = new QAction(QIcon(":/pre_analysis.png"), tr("New Pre-Analysis"), this);
  mpPreAnalysisAlgorithmManagerAct->setStatusTip(tr("Perform pre-analysis on layersets"));
  connect(mpPreAnalysisAlgorithmManagerAct, SIGNAL(triggered()), this, SLOT(showPreAnalysisAlgorithmManager()));
  // if it's using a plugin other than Local, disable Pre-Analysis
  QSettings mySettings;
  if (mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin").toString() == "Local Modeller Plugin")
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(true);
  } else
  {
    mpPreAnalysisAlgorithmManagerAct->setEnabled(false);
  }


#ifdef WITH_QGIS  
  //qDebug("QGIS Support enabled");
  mpSaveAsImageAct = new QAction(QIcon(":/export_image.png"), tr("Export map image &as..."), this);
  mpSaveAsImageAct->setShortcut(tr("Ctrl+A"));
  mpSaveAsImageAct->setStatusTip(tr("Save map current map view as an image"));
  connect(mpSaveAsImageAct, SIGNAL(triggered()), this, SLOT(saveMapAsImage()));
  mpSaveAsImageAct->setEnabled(true);
  
  mpZoomInAct = new QAction(QIcon(":/zoomIn.png"),tr("Zoom In"), this);
  mpZoomInAct->setStatusTip(tr("Zoom in on the map by dragging a rectangle"));
  mpZoomInAct->setEnabled(true);
  connect(mpZoomInAct, SIGNAL(triggered()), this, SLOT(zoomInMode()));

  mpZoomOutAct = new QAction(QIcon(":/zoomOut.png"),tr("Zoom Out"), this);
  mpZoomOutAct->setStatusTip(tr("Zoom out on the map by dragging a rectangle"));
  mpZoomOutAct->setEnabled(true);
  connect(mpZoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOutMode()));

  mpPanAct = new QAction(QIcon(":/pan.png"),tr("Pan"), this);
  mpPanAct->setStatusTip(tr("Pan the map by dragging it"));
  mpPanAct->setEnabled(true);
  connect(mpPanAct, SIGNAL(triggered()), this, SLOT(panMode()));

  mpZoomFullAct = new QAction(QIcon(":/zoomFull.png"),tr("Zoom to all"), this);
  mpZoomFullAct->setStatusTip(tr("Zoom to the full extents of the layers"));
  mpZoomFullAct->setEnabled(true);
  connect(mpZoomFullAct, SIGNAL(triggered()), this, SLOT(zoomFull()));

  mpZoomPreviousAct = new QAction(QIcon(":/zoomPrevious.png"),tr("Zoom to previous"), this);
  mpZoomPreviousAct->setStatusTip(tr("Zoom to the previous extents"));
  mpZoomPreviousAct->setEnabled(true);
  connect(mpZoomPreviousAct, SIGNAL(triggered()), this, SLOT(zoomPrevious()));

  /*
  mpIdentifyAct = new QAction(QIcon(":/identify.png"),tr("Identify"), this);
  mpIdentifyAct->setStatusTip(tr("Identify raster cell values on the map by clicking them"));
  mpIdentifyAct->setEnabled(true);
  connect(mpIdentifyAct, SIGNAL(triggered()), this, SLOT(identifyMode()));

  mpMeasureAct = new QAction(QIcon(":/measureDist.png"),tr("Measure Distances"), this);
  mpMeasureAct->setStatusTip(tr("Measure distances on the map"));
  mpMeasureAct->setEnabled(true);
  connect(mpMeasureAct, SIGNAL(triggered()), this, SLOT(measureMode()));

  mpMeasureAreaAct = new QAction(QIcon(":/measureArea.png"),tr("Measure Areas"), this);
  mpMeasureAreaAct->setStatusTip(tr("Measure areas on the map"));
  mpMeasureAreaAct->setEnabled(true);
  connect(mpMeasureAreaAct, SIGNAL(triggered()), this, SLOT(measureAreaMode()));
  */
#endif


  //
  // These are special actions for our systray icon
  //
  mMinimizeAct = new QAction(tr("Mi&nimize"), this);
  connect(mMinimizeAct, SIGNAL(triggered()), this, SLOT(hide()));

  mMaximizeAct = new QAction(tr("Ma&ximize"), this);
  connect(mMaximizeAct, SIGNAL(triggered()), this, SLOT(showMaximized()));

  mRestoreAct = new QAction(tr("&Restore"), this);
  connect(mRestoreAct, SIGNAL(triggered()), this, SLOT(showNormal()));

  mQuitAct = new QAction(tr("&Quit"), this);
  connect(mQuitAct, SIGNAL(triggered()), this, SLOT(fileExit()));

  //
  // Thes are actions for context menus
  //
  mpDeleteModelAct = new QAction(tr("Delete &model"), this);
  connect(mpDeleteModelAct, SIGNAL(triggered()), this, SLOT(removeModelFromExperiment()));
  mpRunModelAct = new QAction(tr("(Re)run &model"), this);
  connect(mpRunModelAct, SIGNAL(triggered()), this, SLOT(runModel()));
  mpPublishModelAct = new QAction(tr("Publish &model"), this);
  connect(mpPublishModelAct, SIGNAL(triggered()), this, SLOT(publishModel()));

  mpDeleteAlgorithmAct = new QAction(tr("Delete &algorithm "), this);
  connect(mpDeleteAlgorithmAct, SIGNAL(triggered()), this, SLOT(removeAlgorithmFromExperiment()));
  mpDeleteTaxonAct = new QAction(tr("Delete &taxon"), this);
  connect(mpDeleteTaxonAct, SIGNAL(triggered()), this, SLOT(removeTaxonFromExperiment()));
}

void OmgMainWindow::createMenus()
{
  mpFileMenu = menuBar()->addMenu(tr("&File"));
  mpFileMenu->addSeparator();
  mpFileMenu->addAction(mpNewAct);
  mpFileMenu->addAction(mpRunAct);
  mpFileMenu->addAction(mpResetAct);
  mpFileMenu->addAction(mpOpenAct);
  mpFileMenu->addAction(mpCloseAct);
  mpFileMenu->addAction(mpSaveAct);
  mpFileMenu->addAction(mpSaveAsImageAct);
  mpFileMenu->addAction(mpPrintPdfAct);
  mpFileMenu->addAction(mpRefreshReportsAct);
  mpFileMenu->addSeparator();
  mpFileMenu->addAction(mpExitAct);

  mpEditMenu = menuBar()->addMenu(tr("&Edit"));
  mpEditMenu->addAction(mpLayerSetManagerAct);
  mpEditMenu->addAction(mpPreAnalysisAlgorithmManagerAct);
  mpEditMenu->addAction(mpAlgorithmManagerAct);
  mpEditMenu->addSeparator();
  mpEditMenu->addAction(mpOptionsAct);

  mpDataPrepMenu = menuBar()->addMenu(tr("&Data Preparation"));
  mpDataPrepMenu->addAction(mpFetchAct);
  //mpDataPrepMenu->addAction(mpToolsConvertAct);
  mpDataPrepMenu->addAction(mpToolsClimateConvertAct);
  mpDataPrepMenu->addAction(mpToolsWarpAct);
  mpDataPrepMenu->addAction(mpToolsFileSplitterAct);

  mpReportsMenu = menuBar()->addMenu(tr("&Reports"));
  mpReportsMenu->addAction(mpAvailableAlgorithmsReportAct);
  mpReportsMenu->addAction(mpExperimentReportAct);

  mpPostProcessingMenu = menuBar()->addMenu(tr("&Post Processing"));
  mpPostProcessingMenu->addAction(mpToolsRasterThresholdAct);
#ifndef OMG_NO_EXPERIMENTAL
  mpPostProcessingMenu->addAction(mpToolsContourAct);
#endif
  mpPostProcessingMenu->addAction(mpExportToCsvAct);
  mpPostProcessingMenu->addAction(mpToolsExternalTestsAct);
  menuBar()->addSeparator();

  mpHelpMenu = menuBar()->addMenu(tr("&Help"));
  mpHelpMenu->addAction(mpHelpAct);
  mpHelpMenu->addAction(mpSupportedFormatsAct);
  mpHelpMenu->addAction(mpLicenseAct);
  //mpHelpMenu->addAction(mpSurveyAct);
  mpHelpMenu->addSeparator();
  mpHelpMenu->addAction(mpAboutOmgAct);



  //
  // Context menus
  //

  mpExperimentContextMenu = new QMenu(this);
  
  mpTaxonContextMenu = new QMenu(this);
  mpTaxonContextMenu->addAction(mpDeleteTaxonAct);
  
  mpModelContextMenu = new QMenu(this);
  mpModelContextMenu->addAction(mpDeleteModelAct); 
  mpModelContextMenu->addAction(mpRunModelAct); 
#ifdef OMG_WEBCATALOGUE
  mpModelContextMenu->addAction(mpPublishModelAct);
#endif

  mpAlgorithmContextMenu = new QMenu(this);
  mpAlgorithmContextMenu->addAction(mpDeleteAlgorithmAct); 
}

void OmgMainWindow::createToolBars()
{
  //NOTE that the objectname needs to be set for each
  //tool bar for the QMainWindow::saveState method to work properly
  
  //file toolbar
  mpFileToolBar = addToolBar(tr("File"));
  mpFileToolBar->setIconSize(QSize(32,32));
  mpFileToolBar->setObjectName("FileToolBar");
  mpFileToolBar->addAction(mpLayerSetManagerAct);
  mpFileToolBar->addAction(mpAlgorithmManagerAct);
  mpFileToolBar->addAction(mpNewAct);
  mpFileToolBar->addAction(mpPreAnalysisAlgorithmManagerAct);
  mpFileToolBar->addAction(mpRunAct);
  mpFileToolBar->addAction(mpSaveAct);
  mpFileToolBar->addAction(mpOpenAct);
  mpFileToolBar->addAction(mpPrintPdfAct);
  mpFileToolBar->addAction(mpOptionsAct);
  //mpFileToolBar->setSizePolicy(QSizePolicy::Minimum , QSizePolicy::Preferred);
  
  //Data Preparation toolbar
  mpDataPrepToolBar = addToolBar(tr("Data Preparation Tools"));
  mpDataPrepToolBar->setIconSize(QSize(32,32));
  mpDataPrepToolBar->setObjectName("DataPrepToolBar");
  mpDataPrepToolBar->addAction(mpFetchAct);
  //mpDataPrepToolBar->addAction(mpToolsConvertAct);
  mpDataPrepToolBar->addAction(mpToolsClimateConvertAct);
  mpDataPrepToolBar->addAction(mpToolsWarpAct);
  mpDataPrepToolBar->addAction(mpToolsFileSplitterAct);
  //mpDataPrepToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  //Post processing toolbar
  mpPostProcessingToolBar = addToolBar(tr("Post Processing Tools"));
  mpPostProcessingToolBar->setIconSize(QSize(32,32));
  mpPostProcessingToolBar->setObjectName("PostProcessingToolBar");
  mpPostProcessingToolBar->addAction(mpToolsRasterThresholdAct);
  mpPostProcessingToolBar->addAction(mpToolsContourAct);
  mpPostProcessingToolBar->addAction(mpExportToCsvAct);
  mpPostProcessingToolBar->addAction(mpToolsExternalTestsAct);
  //mpPostProcessingToolBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  
  //reports toolbar
  mpReportsToolBar = addToolBar(tr("Reports"));
  mpReportsToolBar->setIconSize(QSize(32,32));
  mpReportsToolBar->setObjectName("ReportsToolBar");
  mpReportsToolBar->addAction(mpAvailableAlgorithmsReportAct);
  mpReportsToolBar->addAction(mpExperimentReportAct);
  //mpReportsToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

#ifdef WITH_QGIS  
  //qDebug("QGIS Support enabled - adding toolbar");
  //map browser toolbar
  mpMapToolBar = addToolBar(tr("Map"));
  mpMapToolBar->setIconSize(QSize(32,32));
  mpMapToolBar->setObjectName("MapToolBar");
  //mpMapToolBar->addAction(mpActionAddLayer);
  mpMapToolBar->addAction(mpSaveAsImageAct);
  mpMapToolBar->addAction(mpZoomInAct);
  mpMapToolBar->addAction(mpZoomOutAct);
  mpMapToolBar->addAction(mpZoomFullAct);
  mpMapToolBar->addAction(mpZoomPreviousAct);
  mpMapToolBar->addAction(mpPanAct);
  //mpMapToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  /*
  mpMapToolBar->addAction(mpIdentifyAct);
  mpMapToolBar->addAction(mpMeasureAct);
  mpMapToolBar->addAction(mpMeasureAreaAct);
  */
#endif
  mpFileToolBar->showMaximized();
  mpDataPrepToolBar->showMaximized();
  mpPostProcessingToolBar->showMaximized();  
}
void OmgMainWindow::createStatusBar()
{
  //a progress bar for models
  //initially we make it a child of statusbar,
  //but during model run it will become a child of
  //each tree widget in turn
  mpModelProgress = new QProgressBar(statusBar());
  //just big enough to replace the icon next to the tree item
  mpModelProgress->setMaximumWidth(100);
  mpModelProgress->hide();
  QWhatsThis::add(mpModelProgress, tr("This bar displays how far any running model job has progressed."));
  statusBar()->addWidget(mpModelProgress, 1,true);
  //a progress bar for model creation
  mpExperimentProgress = new QProgressBar(statusBar());
  mpExperimentProgress->setMaximumWidth(100);
  mpExperimentProgress->hide();
  QWhatsThis::add(mpExperimentProgress, tr("This bar displays how far the current running experiment has progressed."));
  statusBar()->addWidget(mpExperimentProgress, 1,true);

  //experiment progress label
  QFont myFont( "Arial", 9 );
  statusBar()->setFont(myFont);
  mpExperimentProgressLabel = new QLabel(QString(),statusBar());
  mpExperimentProgressLabel->setFont(myFont);
  mpExperimentProgressLabel->setMinimumWidth(10);
  mpExperimentProgressLabel->setMargin(3);
  mpExperimentProgressLabel->setAlignment(Qt::AlignCenter);
  mpExperimentProgressLabel->setFrameStyle(QFrame::NoFrame);
  mpExperimentProgressLabel->setText("");
  statusBar()->addWidget(mpExperimentProgressLabel, 0,true);

  statusBar()->showMessage(tr("Ready"));
}

void OmgMainWindow::readSettings()
{
  // restore window and toolbar positions
  QSettings mySettings;
  // restore the toolbar etc state
  QVariant myState = mySettings.value("/Geometry/state");
  this->restoreState(myState.toByteArray());
  //Save the splitter state splitter
  splitter->restoreState(mySettings.value("/Geometry/splitterState").toByteArray());
  // restore window geometry
  QDesktopWidget *myDesktop = QApplication::desktop();
  int myDesktopWidth = myDesktop->width();          // returns desktop width
  int myDesktopHeight = myDesktop->height();         // returns desktop height
  int myWidth = mySettings.value("/Geometry/width", 600).toInt();
  int myHeight = mySettings.value("/Geometry/height", 400).toInt();
  int myPosX = mySettings.value("/Geometry/x", (myDesktopWidth - 600) / 2).toInt();
  int myPosY = mySettings.value("/Geometry/y", (myDesktopHeight - 400) / 2).toInt();
  resize(myWidth,myHeight);
  move(myPosX,myPosY);
  if (mySettings.value("/Geometry/maximized", false).toBool())
  {
    showMaximized();
  }
  /*
  resize((myWidth < myDesktopWidth ? myWidth : myDesktopWidth ) ,
         (myHeight < myDesktopHeight ? myHeight : myDesktopHeight ));
  //only move teh window if its still has its top left corner on the screen
  //after the move
  if ((myPosX >= 0) && (myPosX < myDesktopWidth) && (myPosY >= 0) && (myPosY < myDesktopHeight))
  {
    move(myPosX, myPosY);
  }
  */

}

void OmgMainWindow::writeSettings()
{
  // restore the toolbar and dock widgets postions using Qt4 settings API
  QSettings mySettings;
  //record if the window was maximised
  mySettings.setValue("/Geometry/maximized", this->isMaximized());
  // we dont want to store the maximised geometry so unmaximise first
  showNormal();


  // store the toolbar/dock widget settings using Qt4 settings API
  mySettings.setValue("/Geometry/state", this->saveState());
  mySettings.setValue("/Geometry/splitterState", splitter->saveState());
  

  // store window geometry
  QPoint myPoint = this->pos();
  QSize mySize = this->size();
  mySettings.setValue("/Geometry/x", myPoint.x());
  mySettings.setValue("/Geometry/y", myPoint.y());
  mySettings.setValue("/Geometry/width", mySize.width());
  mySettings.setValue("/Geometry/height", mySize.height());
}

void OmgMainWindow::publishModel()
{
    OmgPublishToCatalogue myPublisher;
    myPublisher.setModel(mpCurrentModel);
    myPublisher.exec();
}

#ifdef WITH_QGIS

/**
 
  Convenience function for readily creating file filters.
 
  Given a long name for a file filter and a regular expression, return
  a file filter string suitable for use in a QFileDialog::OpenFiles()
  call.  The regular express, glob, will have both all lower and upper
  case versions added.
*/
 
static QString createFileFilter_(QString const &theLongName, QString const &glob)
{
  return theLongName + " (" + glob.toLower() + " " + glob.toUpper() + ");;";
}  // createFileFilter

#endif



void OmgMainWindow::fileSave()
{

}
void OmgMainWindow::fileOpen()
{
  OmgExperimentSelector mySelector(this);
  //mySelector.show();
  connect(&mySelector, SIGNAL(loadExperiment(OmgExperiment *)), this, SLOT(setCurrentExperiment(OmgExperiment *)));
  connect(&mySelector, SIGNAL(loadPreAnalysis(OmgPreAnalysis *)), this, SLOT(setCurrentPreAnalysis(OmgPreAnalysis *)));
  mySelector.exec();
}
void OmgMainWindow::exportSamplesToCsv()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsCsvDir",".").toString();

  //create a file dialog using the filter list generated above
  std::auto_ptr < QFileDialog > myQFileDialog
  (
    new QFileDialog(
      this,
      QFileDialog::tr("Save samples to delimited file"),
      myLastUsedDir,
      "Comma delimited (*.csv);;Tab delimited (*.txt)"
      )
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

  if (!myOutputFileName.isEmpty())
  {
    if (myQFileDialog->selectedFilter()=="Comma delimited (*.csv)")
    {
      //ensure the user never ommitted the extension from the filename
      if (!myOutputFileName.toUpper().endsWith(".CSV")) 
      {
        myOutputFileName+=".csv";
      }
      Omgui::createTextFile(myOutputFileName , mpCurrentModel->localitiesToCsv(","));
    }
    else if (myQFileDialog->selectedFilter()=="Tab delimited (*.txt)")
    {
      //ensure the user never ommitted the extension from the filename
      if (!myOutputFileName.toUpper().endsWith(".TXT")) 
      {
        myOutputFileName+=".txt";
      }
      Omgui::createTextFile(myOutputFileName , mpCurrentModel->localitiesToCsv("\t"));
    }
    else
    {
      QMessageBox::warning( this,tr("openModeller Desktop"),tr("Unknown delimited format: ") +
             myQFileDialog->selectedFilter());

    }
    //QFile::copy(mpCurrentModel->csvFileName(), myOutputFileName);
    myQSettings.setValue("openModeller/lastSaveAsCsvDir", myQFileDialog->directory().dirName());
  }
}

void OmgMainWindow::saveMapAsImage()
{
#ifdef WITH_QGIS
  //qDebug("QGIS Support enabled - save map as image using QGIS");
  //create a map to hold the QImageIO names and the filter names
  //the QImageIO name must be passed to the pixmap saveas image function
  typedef QMap<QString, QString> FilterMap;
  FilterMap myFilterMap;

  //find out the last used filter
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedFilter = myQSettings.value("openModeller/saveAsImageFilter").toString();
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsImageDir",".").toString();

  QList<QByteArray> myFormats = QPictureIO::outputFormats();
  // Workaround for a problem with Qt4 - calls to outputFormats tend
  // to return nothing :(
  if (myFormats.count() == 0)
  {
    myFormats.append("png");
    myFormats.append("jpg");
  }
  // get a list of supported output image types
  int myCounterInt=0;
  QString myFilters;
  for ( ; myCounterInt < myFormats.count(); myCounterInt++ )
  {
    QString myFormat=QString(myFormats.at( myCounterInt ));
    QString myFilter = createFileFilter_(myFormat + " format", "*."+myFormat);
    myFilters += myFilter;
    myFilterMap[myFilter] = myFormat;
  }

  //qDebug() << "Available Filters Map: " ;
  //FilterMap::Iterator myIterator;
  //for ( myIterator = myFilterMap.begin(); myIterator != myFilterMap.end(); ++myIterator )
  //{
  //    //qDebug() << myIterator.key() << "  :  " << myIterator.data() ;
  //}

  //create a file dialog using the filter list generated above
  std::auto_ptr < QFileDialog > myQFileDialog
  (
    new QFileDialog(
      this,
      QFileDialog::tr("Save file dialog"),
      myLastUsedDir,
      myFilters
    )
  );


  // allow for selection of more than one file
  myQFileDialog->setFileMode(QFileDialog::AnyFile);

  if (myLastUsedFilter!=QString::null)       // set the filter to the last one used
  {
    myQFileDialog->selectFilter(myLastUsedFilter);
  }


  //prompt the user for a filename
  QString myOutputFileName; // = myQFileDialog->getSaveFileName(); //delete this
  if (myQFileDialog->exec() == QDialog::Accepted)
  {
    QStringList myFiles = myQFileDialog->selectedFiles();
    if (!myFiles.isEmpty())
    {
      myOutputFileName = myFiles[0];
    }
  }

  QString myFilterString = myQFileDialog->selectedFilter()+";;";

  //  std::cout << "Selected filter: " << myFilterString << std::endl;
  //  std::cout << "Image type to be passed to pixmap saver: " << myFilterMap[myFilterString] << std::endl;

  myQSettings.setValue("openModeller/lastSaveAsImageFilter" , myFilterString);
  myQSettings.setValue("openModeller/lastSaveAsImageDir", myQFileDialog->directory().dirName());

  if (!myOutputFileName.isEmpty())
  {
    mpMapCanvas->saveAsImage(myOutputFileName,NULL,myFilterMap[myFilterString]);
    statusBar()->message(tr("Saved map image to") + " " + myOutputFileName);
  }
#endif
}

void OmgMainWindow::setCurrentModel(OmgModel * thepModel)
{
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, true);
  tabModelViewDetails->setTabEnabled(2, false);
  tabModelViewDetails->setTabEnabled(3, true);

  if (thepModel == NULL) 
  { 
    return;
  }
  mpCurrentModel = thepModel;
  disableMapControls();
  if (thepModel->hasError())
  {
    QString myBusyText = tr("This model failed. See log report below for "
      "possible reasons why.");
    tbReport->clear();
    setReportStyleSheet();
	  tbReport->setHtml(
      "<center>"
      "<table>"
      "<tr>"
      "<td><center><img src=\":/status_aborted.png\"></center></td>"
      "</tr><tr>"
      "<td><h2><center>" + myBusyText + "</center></h2></td>"
      "</tr>"
      "</table>"
      "</center>"
      "<pre>"
      + thepModel->modelLog() +
      "</pre>"
      );
    return;
  }
  else if (thepModel->isCompleted() != true)
  {
    QString myBusyText = tr("This model is currently"
      " running or has not yet been run.");
    tbReport->clear();
    setReportStyleSheet();
	  tbReport->setHtml(
      "<center>"
      "<table>"
      "<tr>"
      "<td><center><img src=\":/status_queued.png\"></center></td>"
      "</tr><tr>"
      "<td><h2><center>" + myBusyText + "</center></h2></td>"
      "</tr>"
      "</table>"
      "</center>");
    return;
  }
  else //model is good and displayable
  {
    mpLocalitiesModel->setModel(mpCurrentModel);
    //qDebug("Setting model to : "+mpCurrentModel->taxonName().toLocal8Bit());
    showModelReport(cboReportMode->currentText());
  }
}

void OmgMainWindow::setCurrentPreAnalysisReport()
{
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, false);
  tabModelViewDetails->setTabEnabled(2, false);
  tabModelViewDetails->setTabEnabled(3, true);
  mpRunAct->setEnabled(false);

  if (!mpCurrentPreAnalysis) 
  { 
    return;
  }
  disableMapControls();

  //qDebug("Setting model to : "+mpCurrentModel->taxonName().toLocal8Bit());
  showPreAnalysisReport(cboReportMode->currentText());
}

void OmgMainWindow::setCurrentProjection(OmgProjection const * thepProjection)
{
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, true);
  tabModelViewDetails->setTabEnabled(2, true);
  tabModelViewDetails->setTabEnabled(3, true);

  if (NULL == thepProjection) 
  { 
    return;
  }
  else if (thepProjection->hasError())
  {
    QString myBusyText = tr("This model projection failed. See log report below for "
      "possible reasons why.");
    tbReport->clear();
    setReportStyleSheet();
	  tbReport->setHtml(
      "<center>"
      "<table>"
      "<tr>"
      "<td><center><img src=\":/status_aborted.png\"></center></td>"
      "</tr><tr>"
      "<td><h2><center>" + myBusyText + "</center></h2></td>"
      "</tr>"
      "</table>"
      "</center>"
      "<pre>"
      + thepProjection->projectionLog() +
      "</pre>"
      );
    return;
  }
  else if (thepProjection->isCompleted() != true)
  {
    QString myBusyText = tr("This projection is currently"
      " running or has not yet been run.");
    tbReport->clear();
    setReportStyleSheet();
	  tbReport->setHtml(
      "<center>"
      "<table>"
      "<tr>"
      "<td><center><img src=\":/status_queued.png\"></center></td>"
      "</tr><tr>"
      "<td><h2><center>" + myBusyText + "</center></h2></td>"
      "</tr>"
      "</table>"
      "</center>");
    return;
  }
  mpCurrentModel = mpCurrentExperiment->getModel(thepProjection->modelGuid());
  mpLocalitiesModel->setModel(mpCurrentModel);
  disableMapControls();
  //qDebug("Setting model to : "+mpCurrentModel->taxonName().toLocal8Bit());
  showProjectionReport(thepProjection,cboReportMode->currentText());
  
#ifdef WITH_QGIS
  QSettings mySettings;
  //
  // Set the copyrightlabel plugin text to the 
  // species name and alg name
  // and projection layerset name
  //
  QString myLabel(mpCurrentModel->algorithm().name() +
        "<br/>" +
        mpCurrentModel->taxonName() +
        "<br/>" +
        thepProjection->layerSet().name());
  QgsProject::instance()->writeEntry("CopyrightLabel","/Label", myLabel);
  //now we need to call projectRead signal
  //on the copyright plugin to get it to update
  //itself
  emit projectRead();
    
  //qDebug("QGIS Support enabled - loading layers");
  //create a layerset
  QList<QgsMapCanvasLayer> myList;
  mpMapCanvas->freeze(true);
  QgsMapLayerRegistry::instance()->removeAllMapLayers();
  mpMapCanvas->freeze(false);
  //
  // First the vector layer for occurrences and absences
  //
  bool myResult = Omgui::loadModelOccurrencesLayer( mpCurrentModel );
  if ( !myResult )
  {
    logMessage ("Vector Layer does not exist in file system");
  }
  //
  // The vector layer for tests
  // If the provider is not found or some other problem occurred
  // we continue with just the raster
  QListIterator<OmgModelTest *> myTestIterator( mpCurrentModel->getModelTests() );
  
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();

    QString myShapeFile (mpCurrentModel->workDir() + mypModelTest->shapefileName());
    qDebug("Trying to load: " + mypModelTest->shapefileName().toLocal8Bit());

    if (!QFile::exists(myShapeFile))
    {
      qDebug("Vector layer does not exist");
      logMessage ("Vector Layer does not exist in file system");
    }
    else
    {
      QString myProviderName = "ogr";
      QFileInfo myVectorFileInfo(myShapeFile);
      QgsVectorLayer * mypVectorLayer = new QgsVectorLayer(myVectorFileInfo.filePath(), 
        myVectorFileInfo.completeBaseName(), myProviderName);
      if (mypVectorLayer->isValid())
      {
        qDebug("Vector Layer is valid");
        //
        // For backwards compatibility we need to check
        // if this layer has a status field. If not
        // we display as a single symble, otherwise we
        // use a unique value renderer
        //
        QgsVectorDataProvider *mypProvider = 
          dynamic_cast<QgsVectorDataProvider *>(mypVectorLayer->dataProvider());    
        if (mypProvider)
        { 
          double mySymbolSize(mySettings.value("mapping/localitySymbolSize",3.0).toDouble());
          QgsFieldMap myFields = mypProvider->fields();
          bool myNewFormatFlag = false;
          for (int i = 0; i < myFields.size(); i++ )
          {
            if (myFields[i].name()=="status")
            {
              myNewFormatFlag=true;
              break;
            }
          }
          if (myNewFormatFlag) // uniquevalue renderer
          {
            QColor myPresenceColor(mySettings.value("mapping/presenceColour",QColor(Qt::green).name()).toString());
            QColor myAbsenceColor(mySettings.value("mapping/absenceColour",QColor(Qt::red).name()).toString());
            
            //
            // Presence with NO samples symbol
            //
            QgsSymbol *  mypPresence2Symbol = new QgsSymbol(mypVectorLayer->geometryType(),"Present - Not Used","");
            mypPresence2Symbol->setNamedPointSymbol("hard:triangle");
            mypPresence2Symbol->setFillColor(myPresenceColor);
            mypPresence2Symbol->setColor(Qt::black); //outline
            mypPresence2Symbol->setPointSize(mySymbolSize);
            mypPresence2Symbol->setFillStyle(Qt::SolidPattern);

            //create renderer
            QgsUniqueValueRenderer *mypRenderer = new QgsUniqueValueRenderer(mypVectorLayer->geometryType());
            qDebug("Provider ok...getting status field");
            int myFieldIndex = mypProvider->fieldNameIndex("status");
            if(myFieldIndex > 0) 
            {
              qDebug("Status field found");
              mypRenderer->setClassificationField(myFieldIndex);
              
              mypRenderer->insertValue("Present - Not Used",mypPresence2Symbol);
                            
              mypVectorLayer->setRenderer(mypRenderer);
              // Add the layer to the Layer Registry
              QgsMapLayerRegistry::instance()->addMapLayer(mypVectorLayer, TRUE);
              myList.append(QgsMapCanvasLayer(mypVectorLayer, TRUE)); //bool visibility
            } // field index
          }
          else // old format so use single symbol renderer
          {
            // old format did not support absence
            QgsSymbol *  mypSymbol = new QgsSymbol(mypVectorLayer->geometryType());
            QColor myPresenceColor(mySettings.value("mapping/presenceColour",QColor(Qt::yellow).name()).toString());
            mypSymbol->setFillColor(myPresenceColor);
            mypSymbol->setColor(Qt::black); //outline
            mypSymbol->setPointSize(mySymbolSize);
            mypSymbol->setFillStyle(Qt::SolidPattern);
            QgsSingleSymbolRenderer *mypRenderer = new QgsSingleSymbolRenderer(mypVectorLayer->geometryType());
            mypRenderer->addSymbol(mypSymbol);
            mypVectorLayer->setRenderer(mypRenderer);
            // Add the layer to the Layer Registry
            QgsMapLayerRegistry::instance()->addMapLayer(mypVectorLayer, TRUE);
            myList.append(QgsMapCanvasLayer(mypVectorLayer, TRUE)); //bool visibility
          } //old format
        } //provider valid
      } //vector layer valid
      else
      {
        //qDebug("Vector Layer is NOT valid, and Not Loaded");
      }
    }
  }//*/ Finish Model tests shapefiles.




  //
  // Next the user defined country boundaries layer (if any)
  //
  QString myShapeFile = mySettings.value("mapping/contextLayer").toString();
  QFileInfo myVectorFileInfo(myShapeFile);
  if (myVectorFileInfo.exists())
  {
    qDebug("Loading context layer");
    QString myProviderName      = "ogr";
    QgsVectorLayer * mypVectorLayer = new QgsVectorLayer(myVectorFileInfo.filePath(), 
        myVectorFileInfo.completeBaseName(), myProviderName);
    if (mypVectorLayer->isValid())
    {
      //qDebug("Vector Layer is valid");
      //
      // For backwards compatibility we need to check
      // if this layer has a status field. If not
      // we display as a single symble, otherwise we
      // use a unique value renderer
      //
      QgsVectorDataProvider *mypProvider = dynamic_cast<QgsVectorDataProvider *>(mypVectorLayer->dataProvider());    
      if (mypProvider)
      {
        QgsSymbol *  mypSymbol = new QgsSymbol(mypVectorLayer->geometryType());
        QColor myContextColor(mySettings.value("mapping/contextLineColour",QColor(Qt::gray).name()).toString());
        mypSymbol->setFillColor(myContextColor); //ignored since we paint with nobrush
        mypSymbol->setColor(myContextColor); //outline
        mypSymbol->setLineWidth(mySettings.value("mapping/contextLineWidth",0.1).toDouble());
        mypSymbol->setFillStyle(Qt::NoBrush);
        QgsSingleSymbolRenderer *mypRenderer = new QgsSingleSymbolRenderer(mypVectorLayer->geometryType());
        mypRenderer->addSymbol(mypSymbol);
        mypVectorLayer->setRenderer(mypRenderer);
        // Add the layer to the Layer Registry
        QgsMapLayerRegistry::instance()->addMapLayer(mypVectorLayer, TRUE);
        myList.append(QgsMapCanvasLayer(mypVectorLayer, TRUE)); //bool visibility
      } 
    }//context vector layer is valid
  }//context vector layer exists

  //
  //next the raster...
  //
  //check the file exists first
  QString myRasterFile (thepProjection->workDir() + thepProjection->rawImageFileName());
  if (!QFile::exists(myRasterFile))
  {
    qDebug("Layer does not exist in file system");
    //zoom to the extent of all vectors in this case
    mpMapCanvas->zoomToFullExtent();
  }
  else //layer exists
  {
    QFileInfo myRasterFileInfo(myRasterFile);
    QgsRasterLayer * mypRasterLayer = new QgsRasterLayer(myRasterFileInfo.filePath(), 
        myRasterFileInfo.completeBaseName());

    if (mypRasterLayer->isValid())
    {
      QString myFormat = thepProjection->omRasterFormat();
      //float myInputNoData = Omgui::getOutputFormatNoData().value(myFormat);
      float myFormatMinimum = Omgui::getOutputFormatRanges().value(myFormat).first;
      float myFormatMaximum = Omgui::getOutputFormatRanges().value(myFormat).second;
      mypRasterLayer->setDrawingStyle(QgsRasterLayer::SingleBandPseudoColor);
      mypRasterLayer->setColorShadingAlgorithm(QgsRasterLayer::PseudoColorShader);  
      mypRasterLayer->setMinimumValue(mypRasterLayer->grayBandName(),myFormatMinimum, false);
      mypRasterLayer->setMaximumValue(mypRasterLayer->grayBandName(),myFormatMaximum);
      mypRasterLayer->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum, false);
      qDebug("Raster Layer is valid");
      // Add the layer to the Layer Registry
      QgsMapLayerRegistry::instance()->addMapLayer(mypRasterLayer, TRUE);
      // Add the raster layer to the Layer Set
      myList.append(QgsMapCanvasLayer(mypRasterLayer, TRUE));


      // set the canvas to the extent of our layer
      bool myResetExtentsFlag= mySettings.value("mapping/resetExtents", false).toBool();
      if (myResetExtentsFlag || mResetExtentsFlag)
      {
        mpMapCanvas->setExtent(mypRasterLayer->extent());
        mpMapCanvas->zoomToFullExtent();
        mResetExtentsFlag=false;
      }
    } //raster is valid
    else
    {
      qDebug("Raster Layer is NOT valid");
    }
  }//raster exists
  // Set the Map Canvas Layer Set
  mpMapCanvas->setLayerSet(myList);
  enableMapControls();
#else
  lblOutputMap->setPixmap(QPixmap(mpCurrentModel->workDir() + mpCurrentModel->colouredImageFileName()));
#endif
  
} //setCurrentProjection


void OmgMainWindow::on_cboSortBy_currentIndexChanged(QString theString)
{
  if (theString==tr("Algorithm"))
  {
    mSortMode= BY_ALGORITHM;
  }
  else
  {
    mSortMode= BY_TAXON;
  }
  if(mpCurrentExperiment)
    populateTree();
  else
    populatePreAnalysisTree();
}

void OmgMainWindow::setCurrentExperiment(OmgExperiment * thepExperiment)
{
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, false);
  tabModelViewDetails->setTabEnabled(2, false);
  tabModelViewDetails->setTabEnabled(3, true);
  mpRunAct->setEnabled(true);

  mpPostProcessingMenu->setDisabled(false);
  mpPostProcessingToolBar->setDisabled(false);
  
  if (mpCurrentExperiment)
  {
    delete mpCurrentExperiment; 
  }
  if (mpCurrentPreAnalysis)
  {
    delete mpCurrentPreAnalysis;
  }
  mpCurrentExperiment = thepExperiment;
  //force zoom to extents of first layer clicked
  mResetExtentsFlag=true;
  //Set up experiment tree  
  populateTree();
  
  
  //Turn on all the model details widgets
  setEnableModelDetailWidgets(true);
  //adjust the window title to reflect the name of the experiment
  setWindowTitle("openModeller Desktop - " + mpCurrentExperiment->name());
  //prompt the user to run the experiment if it still has unrun models
  if ( !mpCurrentExperiment->isCompleted() )
  {
    QMessageBox myMessage;
    myMessage.setText(tr("Would you like to run this experiment now?"));
    myMessage.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    switch (myMessage.exec()) {
      case QMessageBox::Yes:
        // yes was clicked
        runExperiment();
        return;
        break;
      case QMessageBox::No:
        // no was clicked
        //do nothing
        break;
      default:
        // should never be reached
        break;
    }
  }
}

void OmgMainWindow::setCurrentPreAnalysis(OmgPreAnalysis * thepPreAnalysis)
{
  tabModelViewDetails->setTabEnabled(0, true);
  tabModelViewDetails->setTabEnabled(1, false);
  tabModelViewDetails->setTabEnabled(2, false);
  tabModelViewDetails->setTabEnabled(3, true);

  if (mpCurrentExperiment)
  {
    delete mpCurrentExperiment; 
  }
  if (mpCurrentPreAnalysis)
  {
    delete mpCurrentPreAnalysis; 
  }
  mpCurrentPreAnalysis = thepPreAnalysis;
  //force zoom to extents of first layer clicked
  mResetExtentsFlag=true;
  //Set up experiment tree  
  populatePreAnalysisTree();
  
  
  //Turn on all the model details widgets
  setEnableModelDetailWidgets(true);
  //adjust the window title to reflect the name of the experiment
  setWindowTitle("openModeller Desktop - " + mpCurrentPreAnalysis->name());
  //prompt the user to run the experiment if it still has unrun models

  if(!mpCurrentPreAnalysis->isCompleted())
  {
    runPreAnalysis();
  }
  else
  {
    setCurrentPreAnalysisReport();
  }
}

void OmgMainWindow::populateTree()
{
  if (!mpCurrentExperiment)
  {
    return ;
  }
  disconnect(treeExperiment, SIGNAL(currentItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)), 0, 0);
  treeExperiment->clear();
  makeExperimentNode();
  //
  //make a parent node for each alg type or taxon (depending on sort order)
  //
  if (mSortMode==BY_TAXON)
  {
    makeTaxonTreeNodes();
  }
  else //BY_ALGORITHM
  {
    makeAlgorithmTreeNodes();
  }
  //
  // Loop through all models now, adding a nore for each
  // to the appropriate parent.
  //
  int myCount = mpCurrentExperiment->count();
  for (int i=0; i<myCount; i++)
  {
    makeModelTreeNode(i);
  }
  treeExperiment->sortItems (0, Qt::AscendingOrder );
  // 
  // For trapping right mouse clicks on the tree
  // See the qt docs on thie but in essence we 
  // need to tell teh treewidget that we have
  // a custom context menu handler otherwise
  // we wont receive events.
  // 
  //
  treeExperiment->setContextMenuPolicy ( Qt::CustomContextMenu );
  connect(treeExperiment, SIGNAL(customContextMenuRequested (const QPoint & )), 
          this, SLOT(treeContextMenuEvent(const QPoint & )));
  //for trapping selections in the tree
  connect(treeExperiment, SIGNAL(currentItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)), 
          this, SLOT(currentItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)));
  treeExperiment->expandAll();

  return;
}

void OmgMainWindow::makeExperimentNode()
{
  //make a node for the experiment
  QTreeWidgetItem * mypExperimentItem = new QTreeWidgetItem(treeExperiment);
  mypExperimentItem->setText(0,tr("Experiment"));
  mypExperimentItem->setText(1,"Experiment"); //not user visible
  QIcon myExperimentIcon;
  myExperimentIcon.addFile(":/filenewExperiment.png");
  mypExperimentItem->setIcon(0,myExperimentIcon);
  treeExperiment->setItemExpanded(mypExperimentItem,true);
  //select the experiemnt as active item 
  treeExperiment->setItemSelected(mypExperimentItem,true);
  currentItemChanged(mypExperimentItem,mypExperimentItem);
}

void OmgMainWindow::populatePreAnalysisTree()
{
  if (!mpCurrentPreAnalysis)
  {
    return ;
  }

  disconnect(treeExperiment, SIGNAL(currentItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)), 0, 0);
  treeExperiment->clear();
  makePreAnalysisNode();
  makePreAnalysisTreeNodes();

  //for trapping selections in the tree
  connect(treeExperiment, SIGNAL(currentItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)), 
          this, SLOT(currentPreAnalysisItemChanged(QTreeWidgetItem * ,QTreeWidgetItem *)));
  treeExperiment->expandAll();
  return;
}

void OmgMainWindow::makePreAnalysisNode()
{
  //make a node for the experiment
  QTreeWidgetItem * mypExperimentItem = new QTreeWidgetItem(treeExperiment);
  mypExperimentItem->setText(0,tr("Pre-Analysis"));
  mypExperimentItem->setText(1,"Pre-Analysis"); //not user visible
  QIcon myExperimentIcon;
  myExperimentIcon.addFile(":/filenewExperiment.png");
  mypExperimentItem->setIcon(0,myExperimentIcon);
  treeExperiment->setItemExpanded(mypExperimentItem,true);
  //select the experiemnt as active item 
  treeExperiment->setItemSelected(mypExperimentItem,true);
  currentPreAnalysisItemChanged(mypExperimentItem,mypExperimentItem);
}

QTreeWidgetItem * OmgMainWindow::experimentNode()
{
  int myColumn = 0;
  if (0==treeExperiment->findItems(tr("Experiment"),Qt::MatchCaseSensitive,myColumn).size())
  {
    makeExperimentNode();
  }
  QTreeWidgetItem * mypExperimentItem = treeExperiment->findItems(tr("Experiment"),Qt::MatchCaseSensitive,myColumn).at(0);
  return mypExperimentItem;
}

QTreeWidgetItem * OmgMainWindow::preAnalysisNode()
{
  int myColumn = 0;
  if (0==treeExperiment->findItems(tr("Pre-Analysis"),Qt::MatchCaseSensitive,myColumn).size())
  {
    makePreAnalysisNode();
  }
  QTreeWidgetItem * mypPreAnalysisItem = 
    treeExperiment->findItems(tr("Pre-Analysis"),Qt::MatchCaseSensitive,myColumn).at(0);
  return mypPreAnalysisItem;
}

void OmgMainWindow::makeTaxonTreeNodes()
{
  QString myType = "Taxon";
  QString myIconFileName = ":/taxon.png";
  QIcon myIcon;
  myIcon.addFile(myIconFileName);
  QStringList myNameList = mpCurrentExperiment->taxonNames();
  QStringListIterator myIterator(myNameList);
  while (myIterator.hasNext())
  {
    QString myName = myIterator.next();
    QTreeWidgetItem * mypItem = new QTreeWidgetItem(experimentNode());
    mypItem->setIcon(0,myIcon);
    mypItem->setText(0,myName);
    //used later to detect if an alg/taxon node was clicked
    mypItem->setText(1,myType); 
    //expand the tree
    treeExperiment->setItemExpanded(mypItem,true);
  }
}

QTreeWidgetItem * OmgMainWindow::taxonNode(QString theLabel)
{
  QTreeWidgetItemIterator myIterator(treeExperiment);
  while (*myIterator) 
  {
    QTreeWidgetItem * mypItem = *myIterator;
    if (mypItem->text(0) == theLabel && mypItem->text(1) == "Taxon")
    {
      return mypItem;
    }
    ++myIterator;
  }
  return NULL;;
}

void OmgMainWindow::makeAlgorithmTreeNodes()
{
  QString myType = "Algorithm";
  QString myIconFileName = ":/algorithmmanager.png";
  QIcon myIcon;
  myIcon.addFile(myIconFileName);
  QStringList myNameList = mpCurrentExperiment->algorithmNames();
  QStringListIterator myIterator(myNameList);
  while (myIterator.hasNext())
  {
    QString myName = myIterator.next();
    QTreeWidgetItem * mypItem = new QTreeWidgetItem(experimentNode());
    mypItem->setIcon(0,myIcon);
    mypItem->setText(0,myName);
    //used later to detect if an alg/taxon node was clicked
    mypItem->setText(1,myType); 
    //expand the tree
    treeExperiment->setItemExpanded(mypItem,true);
  }
}

QTreeWidgetItem * OmgMainWindow::algorithmNode(QString theLabel)
{
  QTreeWidgetItemIterator myIterator(treeExperiment);
  while (*myIterator) 
  {
    QTreeWidgetItem * mypItem = *myIterator;
    if (mypItem->text(0) == theLabel && mypItem->text(1) == "Algorithm")
    {
      return mypItem;
    }
    ++myIterator;
  }
  return NULL;;
}

void OmgMainWindow::makePreAnalysisTreeNodes()
{
  QString myType = "PreAnalysis";
  QString myIconFileName = ":/algorithmmanager.png";
  QIcon myIcon;
  myIcon.addFile(myIconFileName);
  QString myName;
  if (mSortMode==BY_TAXON)
  {
    myName = mpCurrentPreAnalysis->taxonName();
  }
  else
  {
    myName = mpCurrentPreAnalysis->name();
  }
  QTreeWidgetItem * mypItem = new QTreeWidgetItem(preAnalysisNode());
  mypItem->setIcon(0,myIcon);
  mypItem->setText(0,myName);
  //used later to detect if an alg/taxon node was clicked
  mypItem->setText(1,myType);

  //fill in the other type
  if (mSortMode==BY_ALGORITHM)
  {
    myName = mpCurrentPreAnalysis->taxonName();
  }
  else
  {
    myName = mpCurrentPreAnalysis->name();
  }
  QTreeWidgetItem * mypOtherItem = new QTreeWidgetItem(mypItem);
  mypOtherItem->setIcon(0,myIcon);
  mypOtherItem->setText(0,myName);

  //expand the tree
  treeExperiment->setItemExpanded(mypItem,true);
}

bool OmgMainWindow::makeModelTreeNode(int theModelNumber)
{
  OmgModel * mypModel = mpCurrentExperiment->getModel(theModelNumber);
  if (mypModel==NULL)
  {
    emit logMessage("Model is NULL, skipping");
    return false;;
  }
  //determine the correct parent for this model in the tree
  QTreeWidgetItem * mypParentItem;
  QString myItemText;
  if (mSortMode==BY_TAXON)
  {
    mypParentItem = taxonNode(mypModel->taxonName());
    if (NULL == mypParentItem) 
    {
      emit logMessage("Model parent is NULL for taxonNode");
    }
    myItemText = mypModel->algorithm().name();
  }
  else // BY_ALGORITHM
  {
    mypParentItem = algorithmNode(mypModel->algorithm().name());
    if (NULL == mypParentItem) 
    {
      emit logMessage("Model parent is NULL for algorithmNode");
    }
    myItemText = mypModel->taxonName();
  }
  //now the model
  QTreeWidgetItem * mypItem = new QTreeWidgetItem(mypParentItem);
  mypItem->setText(0,myItemText);
  //add the item to the items hash so we can find it easily later
  if (mypModel->isCompleted() && !mypModel->hasError())
  {
    QIcon myIcon;
    myIcon.addFile(":/status_complete.png");
    mypItem->setIcon(0,myIcon);
  }
  else if (mypModel->hasError())
  {
    QIcon myIcon;
    myIcon.addFile(":/status_aborted.png");
    mypItem->setIcon(0,myIcon);
  }
  else
  {
    QIcon myIcon;
    myIcon.addFile(":/status_queued.png");
    mypItem->setIcon(0,myIcon);
  }
  mypItem->setText(1,"Model");
  mypItem->setData(0,Qt::UserRole,mypModel->guid());
  QListIterator<OmgProjection *> myIterator(mypModel->projectionList());
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    makeProjectionTreeNode(mypProjection->guid());
  }
  treeExperiment->setItemExpanded(mypItem,true);
  return true;
}

QTreeWidgetItem * OmgMainWindow::modelNode(QString theGuid)
{
  QTreeWidgetItemIterator myIterator(treeExperiment);
  while (*myIterator) 
  {
    QTreeWidgetItem * mypItem = *myIterator;
    if (theGuid == mypItem->data(0,Qt::UserRole).toString())
    {
      return mypItem;
    }
    ++myIterator;
  }
  return NULL;
}

bool OmgMainWindow::makeProjectionTreeNode(QString theGuid)
{
  OmgProjection const * mypProjection = mpCurrentExperiment->getProjection(theGuid);
  if (NULL == mypProjection) 
  {
    emit logMessage("Projection is NULL for " + theGuid);
    return false;
  }
  //determine the correct parent for this model in the tree
  QTreeWidgetItem * mypParentItem = modelNode(mypProjection->modelGuid());
  if (NULL == mypParentItem) 
  {
    emit logMessage("Projection parent is NULL for modelNode " + mypProjection->modelGuid());
    return false;
  }
  QString myItemText = mypProjection->layerSet().name();
  //now the model
  QTreeWidgetItem * mypItem = new QTreeWidgetItem(mypParentItem);
  mypItem->setText(0,myItemText);
  //add the item to the items hash so we can find it easily later
  if (mypProjection->isCompleted() && !mypProjection->hasError())
  {
    QIcon myIcon;
    myIcon.addFile(":/status_complete.png");
    mypItem->setIcon(0,myIcon);
  }
  else if (mypProjection->hasError())
  {
    QIcon myIcon;
    myIcon.addFile(":/status_aborted.png");
    mypItem->setIcon(0,myIcon);
  }
  else
  {
    QIcon myIcon;
    myIcon.addFile(":/status_queued.png");
    mypItem->setIcon(0,myIcon);
  }
  mypItem->setText(1,"Projection");
  mypItem->setData(0,Qt::UserRole,mypProjection->guid());
  return true;
}

QTreeWidgetItem * OmgMainWindow::projectionNode(QString theGuid)
{
  QTreeWidgetItemIterator myIterator(treeExperiment);
  while (*myIterator) 
  {
    QTreeWidgetItem * mypItem = *myIterator;
    if (theGuid == mypItem->data(0,Qt::UserRole).toString())
    {
      return mypItem;
    }
    ++myIterator;
  }
  return NULL;
}
void OmgMainWindow::showAlgorithmSummary(QString theAlgorithmName)
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  //disconnect(listThumbnails, SIGNAL(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)), 0, 0);
  tbReport->hide();
  listThumbnails->show();
  listThumbnails->clear();
  int myCount = mpCurrentExperiment->count();
  //
  // Loop through all models now, adding a list widget item for each
  //
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpCurrentExperiment->getModel(i);
    if (mypModel==NULL)
    {
      emit logMessage("Model is null is NULL, skipping");
      continue;
    }
    //determine the correct parent for this model in the tree
    QString myItemText;
    myItemText = mypModel->algorithm().name();
    if (myItemText==theAlgorithmName)
    {
      QListIterator<OmgProjection *> myIterator(mypModel->projectionList());
      while (myIterator.hasNext())
      {
        OmgProjection * mypProjection = myIterator.next();
        QFileInfo myFileInfo(mypProjection->workDir() + mypProjection->thumbnailFileName());
        if (myFileInfo.exists() && !mypProjection->thumbnailFileName().isEmpty())
        {
          //now the model
          QListWidgetItem * mypItem = new QListWidgetItem(listThumbnails);
          QIcon myIcon;
          myIcon.addFile(mypProjection->workDir() + mypProjection->thumbnailFileName());
          mypItem->setIcon(myIcon);
        }
        else
        {
          emit logMessage(mypProjection->thumbnailFileName() + " is missing, not added to thumbnail view");
        }
      }
    }
  }
  //connect(listThumbnails, SIGNAL(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)), 
  //       this, SLOT(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)));
}

void OmgMainWindow::showTaxonSummary(QString theTaxonName)
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  //disconnect(listThumbnails, SIGNAL(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)), 0, 0);
  tbReport->hide();
  listThumbnails->show();
  listThumbnails->clear();
  int myCount = mpCurrentExperiment->count();
  //
  // Loop through all models now, adding a list widget item for each
  //
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpCurrentExperiment->getModel(i);
    if (mypModel==NULL)
    {
      emit logMessage("Model is null is NULL, skipping");
      continue;
    }
    //determine the correct parent for this model in the tree
    QString myItemText = mypModel->taxonName();
    if (myItemText==theTaxonName)
    {
      QListIterator<OmgProjection *> myIterator(mypModel->projectionList());
      while (myIterator.hasNext())
      {
        OmgProjection * mypProjection = myIterator.next();
        QFileInfo myFileInfo(mypProjection->workDir() + mypProjection->thumbnailFileName());
        if (myFileInfo.exists() && !mypProjection->thumbnailFileName().isEmpty())
        {
          //now the model
          QListWidgetItem * mypItem = new QListWidgetItem(listThumbnails);
          QIcon myIcon;
          myIcon.addFile(mypProjection->workDir() + mypProjection->thumbnailFileName());
          mypItem->setIcon(myIcon);
        }
      }
    }
  }
  //connect(listThumbnails, SIGNAL(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)), 
  //       this, SLOT(currentItemChanged(QListWidgetItem * ,QListWidgetItem *)));
}

void OmgMainWindow::currentItemChanged(QTreeWidgetItem * thepCurrentItem, QTreeWidgetItem * thepPreviousItem)
{
  if ((!mpCurrentExperiment)&&(!mpCurrentPreAnalysis))
  {
    return;
  }
  //is this the top level experiement node? if yes then show an experiment wide report
  if (thepCurrentItem->text(1) == "Experiment")
  {
    tabModelViewDetails->setTabEnabled(0, true);
    tabModelViewDetails->setTabEnabled(1, false);
    tabModelViewDetails->setTabEnabled(2, false);
    tabModelViewDetails->setTabEnabled(3, true);
    listThumbnails->hide();
    tbReport->show();
    QString myUrl = mpCurrentExperiment->toSummaryHtml();
    //logMessage(myUrl);
    setReportStyleSheet();
    tbReport->setSource(QUrl::fromLocalFile( myUrl ));
    tabModelViewDetails->setCurrentIndex(0);
    return;
  }
  else if (thepCurrentItem->text(1) == "Algorithm")
  {
    tabModelViewDetails->setTabEnabled(0, true);
    tabModelViewDetails->setTabEnabled(1, false);
    tabModelViewDetails->setTabEnabled(2, false);
    tabModelViewDetails->setTabEnabled(3, true);
    qDebug("Algorithm node clicked");
    qDebug(thepCurrentItem->text(0));
    tabModelViewDetails->setCurrentIndex(0);
    showAlgorithmSummary(thepCurrentItem->text(0));
    return;
  }
  else if (thepCurrentItem->text(1) == "Taxon")
  {
    if( mSortMode == BY_ALGORITHM)
    {
      tabModelViewDetails->setTabEnabled(0, true);
      tabModelViewDetails->setTabEnabled(1, true);
      tabModelViewDetails->setTabEnabled(2, false);
      tabModelViewDetails->setTabEnabled(3, true);
    }
    else
    {
      tabModelViewDetails->setTabEnabled(0, true);
      tabModelViewDetails->setTabEnabled(1, false);
      tabModelViewDetails->setTabEnabled(2, false);
      tabModelViewDetails->setTabEnabled(3, true);
    }
    qDebug("Taxon node clicked");
    qDebug(thepCurrentItem->text(0));
    //tbReport->setSource(QUrl(mpCurrentExperiment->toTaxonSummaryHtml(thepCurrentItem->text(0))));
    tabModelViewDetails->setCurrentIndex(0);
    showTaxonSummary(thepCurrentItem->text(0));
    return;
  }
  else if (thepCurrentItem->text(1) == "Model")
  {
    listThumbnails->hide();
    tbReport->show();
    //This is a model node. So render its report
    OmgModel * mypModel =
      mpCurrentExperiment->getModel(thepCurrentItem->data(0,Qt::UserRole).toString());
    //qDebug(thepCurrentItem->data(0,Qt::UserRole).toString().toLocal8Bit());
    if (NULL == mypModel)
    {
      logMessage("Model node could not be found in experiment, doing nothing!");
    }
    else
    {
      setCurrentModel(mypModel);
      return;
    }
  }
  else if (thepCurrentItem->text(1) == "Projection")
  {
    listThumbnails->hide();
    tbReport->show();
    //This is a projection node. So render its report
    OmgProjection const * mypProjection =
      mpCurrentExperiment->getProjection(thepCurrentItem->data(0,Qt::UserRole).toString());
    //qDebug(thepCurrentItem->data(0,Qt::UserRole).toString().toLocal8Bit());
    if (NULL == mypProjection)
    {
      logMessage("Projection node could not be found in experiment, doing nothing!");
    }
    else
    {
      setCurrentProjection(mypProjection);
      return;
    }
  }
  else //undefined
  {
    logMessage("Undefined node type clicked, doing nothing!");
  }

}

void OmgMainWindow::currentPreAnalysisItemChanged(QTreeWidgetItem * thepCurrentItem, QTreeWidgetItem * thepPreviousItem)
{
  listThumbnails->hide();
  tbReport->show();
  if (!mpCurrentPreAnalysis)
  {
    return;
  }

  if (thepCurrentItem->text(1) == "Pre-Analysis")
  {
    //This is a pre-analysis node. So render its report
    setCurrentPreAnalysisReport();
  }
  else //undefined
  {
    //Anywhere you click, you get the same Report
    setCurrentPreAnalysisReport();
    logMessage("Undefined node type clicked, doing nothing!");
  }

}

void OmgMainWindow::treeContextMenuEvent(const QPoint & thePoint)
{
  QTreeWidgetItem* mypItem = treeExperiment->itemAt(thePoint);
  currentItemChanged (mypItem,mypItem);
  //check if its a model item and if so 
  //we can let the user remove / rerun just that model
  //by providing a context menu
  if (mypItem->text(1) == "Experiment")
  {
    qDebug("Experiment context");
    return;
  }
  else if (mypItem->text(1) == "Algorithm")
  {
    qDebug("Algorithm context");
    mpAlgorithmContextMenu->popup(treeExperiment->mapToGlobal(thePoint));
    return;
  }
  else if (mypItem->text(1) == "Taxon")
  {
    qDebug("Taxon context");
    mpTaxonContextMenu->popup(treeExperiment->mapToGlobal(thePoint));
    return;
  }
  else if (mypItem->text(1) == "Model")
  {
    qDebug("Model context");
    mpModelContextMenu->popup(treeExperiment->mapToGlobal(thePoint));
    return;
  }
  else //only thing left it can be is a projection
  {
    //do clever stuff here
  }
}
void OmgMainWindow::removeModelFromExperiment()
{
  QString myGuid = mpCurrentModel->guid();
  mpCurrentExperiment->removeModel(myGuid);
  mpCurrentExperiment->save();
  mpCurrentExperiment->toSummaryHtml(true);
  populateTree();
}
// called from context menu to run a single model
void OmgMainWindow::runModel()
{
  logMessage("Running model: " + mpCurrentModel->guid());
  mpCurrentModel->setWorkDir(mpCurrentExperiment->workDir());
  mpCurrentModel->reset();
  OmgModellerPluginInterface * mypModellerPlugin = 
    OmgModellerPluginRegistry::instance()->getPlugin();
  mpCurrentModel->setModellerPlugin(mypModellerPlugin);
  // Get the messenger instance that arbitrates comms 
  // between the plugin and our gui
  //
  const OmgPluginMessenger * mypMessenger = 
    mpCurrentExperiment->modellerPlugin()->getMessenger();

  //
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
  // Connect the signals from the modeller adapter
  // used for monitoring the progress of individual models
  //
  connect(mypMessenger, SIGNAL(modelCreationProgress(QString,int )),
      this, SLOT(setModelCreationProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelProjectionProgress(QString,int )),
      this, SLOT(setModelProjectionProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(logMessage(QString,QString)));
  connect(mypMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(logError(QString,QString)));

  mpModelProgress->show();
  mpExperimentProgress->hide();
  //mpCurrentModel->start(); //in thread
  mpCurrentModel->run(); //not in thread
  mpCurrentExperiment->save();
  mpCurrentExperiment->toSummaryHtml(true);
}
void OmgMainWindow::removeAlgorithmFromExperiment()
{
  QList<QTreeWidgetItem *> myList = treeExperiment->selectedItems();
  QTreeWidgetItem * mypItem = myList.at(0);
  if (mypItem->text(1) != "Algorithm")
  {
    return;
  }
  QString myAlgorithmName = mypItem->text(0);
  mpCurrentExperiment->removeAlgorithm(myAlgorithmName);
  mpCurrentExperiment->save();
  mpCurrentExperiment->toSummaryHtml(true);
  populateTree();
}
void OmgMainWindow::removeTaxonFromExperiment()
{
  QList<QTreeWidgetItem *> myList = treeExperiment->selectedItems();
  QTreeWidgetItem * mypItem = myList.at(0);
  if (mypItem->text(1) != "Taxon")
  {
    return;
  }
  QString myTaxonName = mypItem->text(0);
  mpCurrentExperiment->removeTaxon(myTaxonName);
  mpCurrentExperiment->save();
  mpCurrentExperiment->toSummaryHtml(true);
  populateTree();

}
void OmgMainWindow::reportModeChanged(QString theReportMode)
{
  //qDebug("Report mode changed to : "+theReportMode.toLocal8Bit());
  showModelReport(theReportMode);
}

void OmgMainWindow::setTreeColumnSizes(int position,int index)
{
  QString myWidth = QString::number(treeExperiment->width());
  //qDebug("setTreeColumnSizes function called when tree width = "+myWidth.toLocal8Bit());
  treeExperiment->header()->resizeSection(0,treeExperiment->width());
  treeExperiment->header()->resizeSection(1,0);  
}

void OmgMainWindow::resizeEvent ( QResizeEvent * theEvent )
{
  setTreeColumnSizes(0,0);
}

void OmgMainWindow::showModelReport(QString theReportMode)
{
  //qDebug("The report mode = "+theReportMode.toLocal8Bit());
  if (theReportMode==QString("HTML"))
  {
    tbReport->clear();
    setReportStyleSheet();
    tbReport->setSource(QUrl::fromLocalFile(mpCurrentModel->toHtml()));
  }
  else if(theReportMode=="Text")
  {
    tbReport->clear();
    tbReport->insertPlainText(mpCurrentModel->toString());
  }
  else if(theReportMode=="XML")
  {
    tbReport->clear();
    //qDebug ("Getting xml report for " + mpCurrentModel->taxonName().toLocal8Bit());
    tbReport->insertPlainText(mpCurrentModel->toXml());
    // next two lines for testing only
    OmgAlgorithm myAlgorithm=mpCurrentModel->algorithm();
    //qDebug(myAlgorithm.toString().toLocal8Bit());
  }
  else
  {
    //qDebug("Invalid report mode");
  }
}

void OmgMainWindow::showPreAnalysisReport(QString theReportMode)
{
  //qDebug("The report mode = "+theReportMode.toLocal8Bit());
  if (theReportMode==QString("HTML"))
  {
    tbReport->clear();
    setReportStyleSheet();
    tbReport->setSource(QUrl::fromLocalFile(mpCurrentPreAnalysis->toSummaryHtml()));
  }
  else if(theReportMode=="Text")
  {
    tbReport->clear();
    /* not implemented yet */
  }
  else if(theReportMode=="XML")
  {
    tbReport->clear();
    /* not implemented yet */
  }
  else
  {
    //qDebug("Invalid report mode");
  }
}

void OmgMainWindow::showProjectionReport(OmgProjection const * thepProjection, QString theReportMode)
{
  //qDebug("The report mode = "+theReportMode.toLocal8Bit());
  if (theReportMode==QString("HTML"))
  {
    tbReport->clear();
    setReportStyleSheet();
    tbReport->setSource(QUrl::fromLocalFile(thepProjection->toHtml()));
  }
  else if(theReportMode=="Text")
  {
    tbReport->clear();
    tbReport->insertPlainText(thepProjection->toString());
  }
  else if(theReportMode=="XML")
  {
    tbReport->clear();
    //qDebug ("Getting xml report for " + thepProjection->taxonName().toLocal8Bit());
    tbReport->insertPlainText(thepProjection->toXml());
    // next two lines for testing only
    OmgAlgorithm myAlgorithm=thepProjection->algorithm();
    //qDebug(myAlgorithm.toString().toLocal8Bit());
  }
  else
  {
    //qDebug("Invalid report mode");
  }
}

void OmgMainWindow::setEnableModelDetailWidgets(bool theBool)
{
    //qDebug("Enable/Disable model detail widgets");
    //Enable/Disable widgets that display model details
    treeExperiment->setEnabled(theBool);
    tabModelViewDetails->setEnabled(theBool);
    cboSortBy->setEnabled(theBool);

    if(theBool)
    {
       treeExperiment->showColumn(1);
       setTreeColumnSizes(0,0);
    }
    else
    {
       treeExperiment->hideColumn(1);
    }
}

void OmgMainWindow::clearCurrentExperiment()
{
  
 //qDebug("Clear experiment widgets");
 //Empty widgets
 treeExperiment->clear();
 lblOutputMap->clear();
 tbReport->clear();
 
 //Turn off all the model details widgets
 setEnableModelDetailWidgets(false);

}

void OmgMainWindow::showLayerSetManager()
{
 OmgLayerSetManager myManager;
 myManager.show();
 myManager.exec();
      
}


void OmgMainWindow::showPreAnalysisAlgorithmManager()
{
  //fist check we have some layersets available and if not 
  //suggest to the user to do that first!
  if ( Omgui::getAvailableLayerSets().count() < 1)
  {
    QMessageBox myMessage;
    myMessage.setText(tr("You have not created any LayerSets yet. Before attempting to create a") +
                      tr(" pre-analysis you need to have at least one LayerSet defined. Would") +
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

 OmgPreAnalysisAlgorithmManager myManager(this,Qt::Dialog);
  connect(&myManager, SIGNAL(preAnalysisCreated(OmgPreAnalysis *)), 
      this, SLOT(setCurrentPreAnalysis(OmgPreAnalysis *)));
 myManager.exec();
}

void OmgMainWindow::showAlgorithmManager()
{
 //OmgAlgorithmManager myManager(this,Qt::Tool);
 OmgAlgorithmManager myManager(this,Qt::Dialog);
 myManager.exec();
      
}


void OmgMainWindow::setViewMode(QString theMode)
{
//Set the mode of the main window by hiding/showing relevent widgets
  if(theMode=="Browse")
  { 
    //qDebug("Setting view to browse mode");
    setEnableModelDetailWidgets(true);
    //frameBrowseExperimentWidgets->show();
    //frameProgressDetails->hide();
  }
  else if (theMode=="Process")
  {
    //qDebug("Setting view to process mode");
    setEnableModelDetailWidgets(true);
    //frameBrowseExperimentWidgets->hide();
    //frameProgressDetails->show();
  }
  else
  {
    //qDebug("Error setting view mode -unknown type");
  }

}

void OmgMainWindow::printAlgorithmProfilesReport()
{
  QTextDocument myTextDocument;
  QString myReport;
  myReport += "<center><h1>openModeller Algorithm Profiles</h1>";
  myReport += "<hr/>";
  OmgAlgorithmSet myAlgorithmSet = OmgAlgorithmSet::getFromActivePlugin();
  myAlgorithmSet.loadAlgorithms(Omgui::userAlgorithmProfilesDirPath());
  myReport += myAlgorithmSet.toHtml();
  myTextDocument.setHtml(myReport);
  statusBar()->showMessage(tr("Printing Algorithm Profiles Report..."));
  myTextDocument.setHtml(myReport);
  QPrinter myPrinter;
  //pdf (not used)
  //myPrinter.setOutputFormat(QPrinter::PdfFormat);
  //myPrinter.setOutputFileName(myOutputFileName);
  
  //print device
  myPrinter.setOutputFormat(QPrinter::NativeFormat);
  myPrinter.setDocName("openModellerDesktopExperimentReport");
  myTextDocument.print(&myPrinter);
}

void OmgMainWindow::printExperimentReport()
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  QTextDocument myTextDocument;
  QString myReport;
  myReport += "<center><h1>openModeller Experiment Report</h1>";
  myReport += "<hr/>";
  myReport += Omgui::readTextFile(mpCurrentExperiment->toSummaryHtml());
  int myCount = mpCurrentExperiment->count();
  for (int i=0; i<myCount; i++)
  {
    OmgModel *  mypModel=mpCurrentExperiment->getModel(i);
    if (mypModel==NULL)
    {
      emit logMessage("Model is null is NULL, skipping");
      continue;
    }
    myReport += Omgui::readTextFile(mypModel->toHtml());  
  }
  myTextDocument.setHtml(myReport);
  QPrinter myPrinter;
  //pdf (not used)
  //myPrinter.setOutputFormat(QPrinter::PdfFormat);
  //myPrinter.setOutputFileName(myOutputFileName);
  
  //print device
  myPrinter.setOutputFormat(QPrinter::NativeFormat);
  myPrinter.setDocName("openModellerDesktopExperimentReport");
  myTextDocument.print(&myPrinter);
}

void OmgMainWindow::printPreAnalysisSummaryPdf()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsPdfDir",".").toString();
  //create a file dialog 
  std::auto_ptr < QFileDialog > myFileDialog
  (
    new QFileDialog(
      this,
      QFileDialog::tr("Save experiment report to portable document format (.pdf)"),
      myLastUsedDir,
      tr("Portable Document Format (*.pdf)")
      )
  );
  myFileDialog->setFileMode(QFileDialog::AnyFile);
  myFileDialog->setAcceptMode(QFileDialog::AcceptSave);

  //prompt the user for a filename
  QString myOutputFileName; 
  if (myFileDialog->exec() == QDialog::Accepted)
  {
    QStringList myFiles = myFileDialog->selectedFiles();
    if (!myFiles.isEmpty())
    {
      myOutputFileName = myFiles[0];
    }
  }
  if (!myOutputFileName.isEmpty())
  {
    if (myFileDialog->selectedFilter()==tr("Portable Document Format (*.pdf)"))
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
      myPrinter.setDocName("openModellerDesktopPreAnalysisReport");
      QTextDocument myTextDocument;
      myTextDocument.setDefaultStyleSheet(Omgui::defaultStyleSheet());
      QString myReport;
      myReport += Omgui::getHtmlHeader();
      myReport += mpCurrentPreAnalysis->toPrintHtml();
      myReport += Omgui::getHtmlFooter();
      myTextDocument.setHtml(myReport);
      myTextDocument.print(&myPrinter);
    }
  }
}

void OmgMainWindow::printExperimentPdf()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedDir = myQSettings.value("openModeller/lastSaveAsPdfDir",".").toString();

  //create a file dialog 
  std::auto_ptr < QFileDialog > myFileDialog
  (
    new QFileDialog(
      this,
      QFileDialog::tr("Save experiment report to portable document format (.pdf)"),
      myLastUsedDir,
      tr("Portable Document Format (*.pdf)")
      )
  );
  myFileDialog->setFileMode(QFileDialog::AnyFile);
  myFileDialog->setAcceptMode(QFileDialog::AcceptSave);

  //prompt the user for a filename
  QString myOutputFileName; 
  if (myFileDialog->exec() == QDialog::Accepted)
  {
    QStringList myFiles = myFileDialog->selectedFiles();
    if (!myFiles.isEmpty())
    {
      myOutputFileName = myFiles[0];
    }
  }

  if (!myOutputFileName.isEmpty())
  {
    if (myFileDialog->selectedFilter()==tr("Portable Document Format (*.pdf)"))
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
      if (!mpExperimentPrinter)
      {
        mpExperimentPrinter = new OmgExperimentPrinter();
      }
      mProgressDialog.setValue(0);
      mProgressDialog.setLabelText(tr("Please wait while your report is generated"));
      mProgressDialog.show();
      mProgressDialog.setWindowModality(Qt::WindowModal);
      mProgressDialog.setAutoClose(true);
      connect(mpExperimentPrinter, SIGNAL(progress(int)), &mProgressDialog, SLOT(setValue(int)));
      connect(mpExperimentPrinter, SIGNAL(maximum(int)), &mProgressDialog, SLOT(setMaximum(int)));
      mpExperimentPrinter->setExperiment(mpCurrentExperiment);
      mpExperimentPrinter->setPrinter(myPrinter);
      //crashes when I use it in thread mode..
      //mpExperimentPrinter->start();
      mpExperimentPrinter->run();
      //new to qt4.3 - bounce dock icon or flash menu bar entry
      //only does anything if this window is not focussed
      QApplication::alert(this);
    }
    else
    {
      QMessageBox::warning( this,tr("openModeller Desktop"),tr("Unknown delimited format: ") +
             myFileDialog->selectedFilter());

    }
    myQSettings.setValue("openModeller/lastSaveAsPdfDir", myFileDialog->directory().absolutePath());
  }
}

void OmgMainWindow::printPdf()
{
  if(mpCurrentExperiment)
    printExperimentPdf();
  else if(mpCurrentPreAnalysis)
    printPreAnalysisSummaryPdf();
}

void OmgMainWindow::on_tabModelViewDetails_currentChanged(int theIndex)
{
#ifdef WITH_QGIS
  //disable gis tools based on context
  if (theIndex==0)
  {
    disableMapControls();
  }
  else
  {
    enableMapControls();
  }
#endif
}
void OmgMainWindow::disableControlsWhileRunning()
{
  mpSaveAsImageAct->setEnabled(false);
  mpNewAct->setEnabled(false);
  mpResetAct->setEnabled(false);
  mpOpenAct->setEnabled(false);
  mpCloseAct->setEnabled(false);
  mpSaveAct->setEnabled(false);
  mpPrintPdfAct->setEnabled(false);
  mpRefreshReportsAct->setEnabled(false);
  mpLayerSetManagerAct->setEnabled(false);
  mpAlgorithmManagerAct->setEnabled(false);
  mpOptionsAct->setEnabled(false);
  //mpToolsConvertAct->setEnabled(false);
  mpToolsClimateConvertAct->setEnabled(false);
  mpAvailableAlgorithmsReportAct->setEnabled(false);
  mpExperimentReportAct->setEnabled(false);
  mpToolsRasterThresholdAct->setEnabled(false);

  mpFetchAct->setEnabled(false);
  mpPrintPdfAct->setEnabled(false);
  mpToolsExternalTestsAct->setEnabled(false);
  mpAvailableAlgorithmsReportAct->setEnabled(false);
  mpExperimentReportAct->setEnabled(false);
  mpPreAnalysisAlgorithmManagerAct->setEnabled(false);
  mpToolsFileSplitterAct->setEnabled(false);
  mpExportToCsvAct->setEnabled(false);

#ifndef OMG_NO_EXPERIMENTAL
  mpToolsContourAct->setEnabled(false);
#endif
}
void OmgMainWindow::enableControlsAfterRunning()
{
  mpSaveAsImageAct->setEnabled(true);
  mpNewAct->setEnabled(true);
  mpResetAct->setEnabled(true);
  mpOpenAct->setEnabled(true);
  mpCloseAct->setEnabled(true);
  mpSaveAct->setEnabled(true);
  mpPrintPdfAct->setEnabled(true);
  mpRefreshReportsAct->setEnabled(true);
  mpLayerSetManagerAct->setEnabled(true);
  mpAlgorithmManagerAct->setEnabled(true);
  mpOptionsAct->setEnabled(true);
  //mpToolsConvertAct->setEnabled(true);
  mpToolsClimateConvertAct->setEnabled(true);
  mpAvailableAlgorithmsReportAct->setEnabled(true);
  mpExperimentReportAct->setEnabled(true);
  mpToolsRasterThresholdAct->setEnabled(true);

  mpFetchAct->setEnabled(true);
  mpPrintPdfAct->setEnabled(true);
  mpToolsExternalTestsAct->setEnabled(true);
  mpAvailableAlgorithmsReportAct->setEnabled(true);
  mpExperimentReportAct->setEnabled(true);
  mpPreAnalysisAlgorithmManagerAct->setEnabled(true);
  mpToolsFileSplitterAct->setEnabled(true);
  mpExportToCsvAct->setEnabled(true);


#ifndef OMG_NO_EXPERIMENTAL
  mpToolsContourAct->setEnabled(true);
#endif
}
void OmgMainWindow::disableMapControls()
{
#ifdef WITH_QGIS
  mpSaveAsImageAct->setEnabled(false);
  mpZoomInAct->setEnabled(false);
  mpZoomOutAct->setEnabled(false);
  mpPanAct->setEnabled(false);
  mpZoomFullAct->setEnabled(false);
  mpZoomPreviousAct->setEnabled(false);
#endif
}
void OmgMainWindow::enableMapControls()
{
#ifdef WITH_QGIS
  // Disabled if clause while implementing multiprojection support
  //if (QFile::exists(mpCurrentModel->workDir() + mpCurrentModel->rawImageFileName()) && 
  //    tabModelViewDetails->currentIndex()==2 )
  //{
    mpSaveAsImageAct->setEnabled(true);
    mpZoomInAct->setEnabled(true);
    mpZoomOutAct->setEnabled(true);
    mpPanAct->setEnabled(true);
    mpZoomFullAct->setEnabled(true);
    mpZoomPreviousAct->setEnabled(true);
  //}
  //else
  //{
  //  disableMapControls();
  //}
#endif
}


#ifdef WITH_QGIS
void OmgMainWindow::zoomInMode()
{
  //qDebug("Setting map canvas to ZOOM IN MODE");
  mpMapCanvas->setMapTool(mpZoomInTool);
}
void OmgMainWindow::zoomOutMode()
{
  //qDebug("Setting map canvas to ZOOM OUT MODE");
  mpMapCanvas->setMapTool(mpZoomOutTool);
}
void OmgMainWindow::panMode()
{
  //qDebug("Setting map canvas to PAN mode");
  mpMapCanvas->setMapTool(mpPanTool);
}
void OmgMainWindow::zoomFull()
{
  //qDebug("Setting map canvas to ZOOM FULL EXTENT");
  mpMapCanvas->zoomToFullExtent();
}

void OmgMainWindow::zoomPrevious()
{
  //qDebug("Setting map canvas to ZOOM FULL PREVIOUS");
  mpMapCanvas->zoomToPreviousExtent();
}
/*
void OmgMainWindow::identifyMode()
{
  mpMapCanvas->setMapTool(mpIdentifyTool);
}

void OmgMainWindow::measureMode()
{
  mpMapCanvas->setMapTool(mpMeasureTool);
}

void OmgMainWindow::measureAreaMode()
{
  mpMapCanvas->setMapTool(mpMeasureAreaTool);
}
*/
void OmgMainWindow::addLayer()
{
}
#endif

#ifdef WITH_OSSIMPLANET_QT
void OmgMainWindow::initialiseOssimPlanet()
{
   QGLFormat myFormat;
   myFormat.setSwapInterval(1);
   mpPlanet = new ossimPlanetQtGlWidget(myFormat, planetWidget);
}
#endif

void OmgMainWindow::createTrayIcon()
{
  QSettings mySettings;
  bool myUseSytemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
  if ( QSystemTrayIcon::isSystemTrayAvailable() )
  {
    mTrayIconMenu = new QMenu(this);
    mTrayIconMenu->addAction(mMinimizeAct);
    mTrayIconMenu->addAction(mMaximizeAct);
    mTrayIconMenu->addAction(mRestoreAct);
    mTrayIconMenu->addAction(mpRunAct);
    mTrayIconMenu->addSeparator();
    mTrayIconMenu->addAction(mQuitAct);

    mTrayIcon = new QSystemTrayIcon(QIcon(":/om_logo.png"),this);
    mTrayIcon->setContextMenu(mTrayIconMenu);
    // only make the tray icon visible if the user wants it
    // user can change at run time so we need to do above
    // steps regardless
    if (myUseSytemTrayFlag)
    {
      mTrayIcon->show();
      bool myShowSystemTrayMessagesFlag = mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
      if (QSystemTrayIcon::supportsMessages() && myShowSystemTrayMessagesFlag)
      {
        mTrayIcon->showMessage("openModeller Desktop",
            Omgui::version(),QSystemTrayIcon::Information,1000);
      }
    }
  }
  else
  {
    logMessage(tr("No system tray available on this platform, tray icon disabled"));
  }
}

void OmgMainWindow::setModelCreationProgress (QString theModelGuid, int theProgress)
{
  mpModelProgress->setValue(theProgress);
  QSettings mySettings;
  bool myUseSytemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
  QTreeWidgetItem * mypTreeItem = modelNode(theModelGuid);
  if (NULL==mypTreeItem) return;
  if (0 == theProgress)
  {
    mypTreeItem->setIcon(0,mRunningIcon);
    if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
    {
      mTrayIcon->setIcon(mRunningIcon);
    }
    //only need to do this when creation of model starts
    //set custom colours for the progress bar
    QPalette myPalette = mpModelProgress->palette();
    // Set the bar color
    myPalette.setColor(QPalette::Highlight, QColor(Qt::darkGreen));
    // Set the percentage text Color
    myPalette.setColor(QPalette::Button, QColor(Qt::black));
    // Set the background green
    //myPalette.setColor(QPalette::Window, QColor("green"));
    mpModelProgress->setPalette(myPalette);
    statusBar()->showMessage(mpCurrentExperiment->currentStatus());
  }
  else if (100 > theProgress)
  {
    mypTreeItem->setIcon(0,mRunningIcon);
    if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
    {
      mTrayIcon->setIcon(mRunningIcon);
    }
    statusBar()->showMessage(mpCurrentExperiment->currentStatus());
  }
  else // model is done
  {
    modelCompleted(theModelGuid);
  }
}

void OmgMainWindow::setModelProjectionProgress (QString theGuid, int theProgress)
{
  mpModelProgress->setValue(theProgress);
  if (!mpCurrentExperiment)
  {
    return;
  }
  QSettings mySettings;
  bool myUseSytemTrayFlag = 
    mySettings.value("openModeller/useSystemTray", false).toBool();
  //bool myShowSystemTrayMessagesFlag = 
  //  mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
  QTreeWidgetItem * mypTreeItem = projectionNode(theGuid);
  if (NULL==mypTreeItem)
  {
    logMessage(tr("setModelProjectionProgress could not find projection node to update:"));
    logMessage(theGuid);
    return;
  }

  if (theProgress >= 100)
  {
    //show user we are starting postprocessing now
    QIcon myIcon;
    myIcon.addFile(":/status_complete.png");
    mypTreeItem->setIcon(0,myIcon);
  }
  else if (theProgress == 0)
  {
    mypTreeItem->setIcon(0,mRunningIcon);
    QSettings mySettings;
    if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
    {
      mTrayIcon->setIcon(mRunningIcon);
    }
    QPalette myPalette = mpModelProgress->palette();
    // Set the bar color
  myPalette.setColor(QPalette::Highlight, QColor(Qt::darkGreen));
    // Set the percentage text color
  myPalette.setColor(QPalette::HighlightedText, QColor(Qt::black));
    // Set the background green
    //myPalette.setColor(QPalette::Window, QColor("green"));
    mpModelProgress->setPalette(myPalette);
    statusBar()->showMessage(mpCurrentExperiment->currentStatus());
  }
  else //progress is somewhere between 0 and 100
  {
    mypTreeItem->setIcon(0,mRunningIcon);
    if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
    {
      mTrayIcon->setIcon(mRunningIcon);
      statusBar()->showMessage(mpCurrentExperiment->currentStatus());
    }
  }
}

//slot called after each model completes all processing
//(including post processing)
void OmgMainWindow::modelCompleted(QString theModelGuid)
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  //
  // First update the report for the current node in the experiment tree
  //
  QTreeWidgetItem * mypItem = treeExperiment->currentItem();
  if (mypItem)
  {
    if (theModelGuid==mypItem->data(0,Qt::UserRole).toString())
    {
      OmgModel * mypModel =
        mpCurrentExperiment->getModel(mypItem->data(0,Qt::UserRole).toString());

      //qDebug(thepCurrentItem->data(0,Qt::UserRole).toString().toLocal8Bit());
      if (mypModel)
      {
        setCurrentModel(mypModel);
      }
    }
    //
    // Note that the following three if clauses duplicate
    // logic from currentItemChanged() but I didnt 
    // want to call currentItemChanged directly since it
    // will have the side effect of refreshing the focussed
    // model report if it is not the one which just completed
    // (i.e. the logic above is different in this case for model
    // nodes)
    // There are also some different behaviours e.g.
    // list controls dont force focus is another tab is 
    // active
    else if (mypItem->text(0) == tr("Experiment"))
    {
      listThumbnails->hide();
      tbReport->show();
      QString myUrl = mpCurrentExperiment->toSummaryHtml();
      setReportStyleSheet();
      tbReport->setSource(QUrl::fromLocalFile( myUrl ));
      tabModelViewDetails->setCurrentIndex(0);
      logMessage("Experiment report updated as a model just completed");
      logMessage(myUrl);
    }
    else if (mypItem->text(1) == "Algorithm")
    {
      showAlgorithmSummary(mypItem->text(0));
    }
    else if (mypItem->text(1) == "Taxon")
    {
      showTaxonSummary(mypItem->text(0));
    }
  }
  //
  // Now show task bar notifications if the user has enabled them
  //
  QSettings mySettings;
  bool myUseSytemTrayFlag = 
    mySettings.value("openModeller/useSystemTray", false).toBool();
  bool myShowSystemTrayMessagesFlag = 
    mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
  QTreeWidgetItem * mypTreeItem = modelNode(theModelGuid);
  if (NULL==mypTreeItem) return;
  QIcon myIcon;
  myIcon.addFile(":/status_complete.png");
  mypTreeItem->setIcon(0,myIcon);

  if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
  {
    mTrayIcon->setIcon(myIcon);
    if (QSystemTrayIcon::supportsMessages() && myShowSystemTrayMessagesFlag)
    {
      QString myAlgorithmName = 
        mpCurrentExperiment->getModel(theModelGuid)->algorithm().name();
      mTrayIcon->showMessage("openModeller Desktop",myAlgorithmName + "\n" 
          +mypTreeItem->text(0) + 
          tr(" model completed."),QSystemTrayIcon::Information, 1000);
    }
  }
}

void OmgMainWindow::refreshExperiment()
{
  populateTree();
  treeExperiment->setFocus(Qt::OtherFocusReason);
}

void OmgMainWindow::preAnalysisCompleted()
{
  if (!mpCurrentPreAnalysis)
  {
    return;
  }
  mpRunningMovie->stop();
  //
  // First update the report for the current node in the experiment tree
  //
  QTreeWidgetItem * mypItem = treeExperiment->currentItem();
  if (mypItem)
  {
    /* all this logic was put here for future use (help) */
    if (mypItem->text(1) == "Pre-Analysis")
    {
      //This is a pre-analysis node. So render its report
      setCurrentPreAnalysisReport();
    } else if (mypItem->text(1) == mpCurrentPreAnalysis->algorithmName())
    {
      //This is a pre-analysis algorithm node. So render its report
      setCurrentPreAnalysisReport();
    } else if (mypItem->text(1) == mpCurrentPreAnalysis->taxonName())
    {
      //This is a pre-analysis taxon node. So render its report
      setCurrentPreAnalysisReport();
    }
    else //show blank
    {
      tbReport->clear();
      logMessage("Undefined node type clicked, doing nothing! [" + mypItem->text(1) + "]");
    }
  } else
  {
      setCurrentPreAnalysisReport();
  }
  //
  // Now show task bar notifications if the user has enabled them
  //
  QSettings mySettings;
  bool myUseSytemTrayFlag = 
    mySettings.value("openModeller/useSystemTray", false).toBool();
  bool myShowSystemTrayMessagesFlag = 
    mySettings.value("openModeller/showSystemTrayMessages", false).toBool();

  QTreeWidgetItem * mypTreeItem = preAnalysisNode();
  if (NULL==mypTreeItem) return;
  QIcon myIcon;
  myIcon.addFile(":/status_complete.png");
  mypTreeItem->setIcon(0,myIcon);

  if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
  {
    mTrayIcon->setIcon(myIcon);
    if (QSystemTrayIcon::supportsMessages() && myShowSystemTrayMessagesFlag)
    {
      QString myAlgorithmName = 
        mpCurrentPreAnalysis->algorithmName();
      mTrayIcon->showMessage("openModeller Desktop",myAlgorithmName + "\n" 
          +mypTreeItem->text(0) + 
          tr(" model completed."),QSystemTrayIcon::Information, 1000);
    }
  }
  treeExperiment->setFocus(Qt::OtherFocusReason);
}

void OmgMainWindow::setExperimentProgress (int theProgress)
{
  mpExperimentProgressLabel->setText(tr("Experiment Progress (")
      + QString::number(theProgress) 
      + "/"
      + QString::number(mpExperimentProgress->maximum()) +")");
  mpExperimentProgress->setValue(theProgress);
}

void OmgMainWindow::setExperimentMaximum(int theMaximum)
{
  mpExperimentProgressLabel->setText(tr("Experiment Progress (") 
      +"0/" 
      + QString::number(theMaximum) +")");
  mpExperimentProgress->setMaximum(theMaximum);
  mpExperimentProgress->setValue(0);
}
void OmgMainWindow::logMessage(QString theMessage)
{
  tbLog->append(theMessage);
  tbLog->ensureCursorVisible();
  //
  // log to file too if needed
  //
  QSettings mySettings;
  bool myLogToFileFlag = mySettings.value("openModeller/logToFile", false).toBool();
  if (myLogToFileFlag)
  {
    QString myFileName = mySettings.value("openModeller/logFile","/tmp/omglog.txt").toString();
    QFile myFile( myFileName );
    if ( myFile.open( QIODevice::Append ) )
    {
      QTextStream myQTextStream( &myFile );
      myQTextStream << theMessage << "\n";
      myFile.close();
    }
  }
}
void OmgMainWindow::logMessage(QString theModelGuid, QString theMessage)
{
  //logMessage("For " + theModelGuid);
  logMessage(theMessage);
}
void OmgMainWindow::logError(QString theModelGuid, QString theMessage)
{
  //since we want to insert html and there is no append option
  //for html, we need to manually make sure we are at the end of the file
  tbLog->textCursor().movePosition(QTextCursor::End);
  tbLog->insertHtml("<br/><font color=\"red\">" + theMessage + "</font><font color=\"black\"> * </font><br/>");
  tbLog->ensureCursorVisible();

  //
  // log to file too if needed
  //
  QSettings mySettings;
  bool myLogToFileFlag = mySettings.value("openModeller/logToFile", false).toBool();
  if (myLogToFileFlag)
  {
    QString myFileName = mySettings.value("openModeller/logFile","/tmp/omglog.txt").toString();
    QFile myFile( myFileName );
    if ( myFile.open( QIODevice::Append) )
    {
      QTextStream myQTextStream( &myFile );
      //myQTextStream << theModelGuid << "\n";
      myQTextStream << theMessage;
      myFile.close();
    }
  }

  //
  // Update teh icon of the model entry in the 
  // experiment tree 
  //
  mpModelProgress->setValue(100);
  QTreeWidgetItem * mypTreeItem = modelNode(theModelGuid);
  if (NULL==mypTreeItem) return;
  QIcon myIcon;
  myIcon.addFile(":/status_aborted.png");
  mypTreeItem->setIcon(0,myIcon);
  treeExperiment->setItemWidget(mypTreeItem,0,NULL);

  //
  // Show systray icon
  //
  bool myUseSytemTrayFlag = 
    mySettings.value("openModeller/useSystemTray", false).toBool();
  bool myShowSystemTrayMessagesFlag = 
    mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
  if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
  {
    mTrayIcon->setIcon(myIcon);
    if (QSystemTrayIcon::supportsMessages() && myShowSystemTrayMessagesFlag)
    {
      QString myAlgorithmName = 
        mpCurrentExperiment->getModel(theModelGuid)->algorithm().name();
      mTrayIcon->showMessage("openModeller Desktop",myAlgorithmName + "\n" 
          +mypTreeItem->text(0) + 
          tr(" model and projection failed!"),QSystemTrayIcon::Warning, 1000);
    }
  }
}

void OmgMainWindow::stopExperiment()
{
  if (!mpCurrentExperiment)
  {
    return;
  }
  mpCurrentExperiment->abort();
  //now we need to wait for the stopped signal to be sure the
  //experiment is really stopped (see next method below)
}
void OmgMainWindow::experimentStopped()
{
  mpModelProgress->hide();
  mpExperimentProgress->hide();
  statusBar()->showMessage(tr("Experiment processing completed!"));
  //reconfigure the run tool
  mpRunAct->setIcon(QIcon(":/status_restart.png"));
  mpRunAct->setText(tr("&Run / Resume current experiment"));
  mpRunAct->setShortcut(tr("Ctrl+R"));
  mpRunAct->setStatusTip(tr("Run / Resume current experiment"));
  disconnect(mpRunAct);
  connect(mpRunAct, SIGNAL(triggered()), this, SLOT(runExperiment()));
  mpRunningMovie->stop();
  enableControlsAfterRunning();
  //new to qt4.3 - bounce dock icon or flash menu bar entry
  //only does anything if this window is not focussed
  QApplication::alert(this);
  //update teh systray icon
  QSettings mySettings;
  bool myUseSytemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
  if (QSystemTrayIcon::isSystemTrayAvailable() && myUseSytemTrayFlag)
  {
    mTrayIcon->setIcon(QIcon(":/om_logo.png"));
    bool myShowSystemTrayMessagesFlag = 
      mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
    if (QSystemTrayIcon::supportsMessages() && myShowSystemTrayMessagesFlag)
    {
      mTrayIcon->showMessage("openModeller Desktop",
          tr("Experiment completed"),QSystemTrayIcon::Information,1000);
    }
  }
  // Refresh Report Tab
  treeExperiment->setFocus(Qt::OtherFocusReason);
  refreshReports();
}
void OmgMainWindow::updateRunningIcon(const QRect & theRect )
{
  //logMessage("Updating running icon...");
  mRunningIcon = QIcon(mpRunningMovie->currentPixmap());
}


void OmgMainWindow::refreshReports()
{
  if (!mpCurrentExperiment) return;
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  mpCurrentExperiment->toSummaryHtml(true);
  int myCount = mpCurrentExperiment->count();
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpCurrentExperiment->getModel(i);
    mypModel->postProcess();
  }
  QApplication::restoreOverrideCursor();
}

void OmgMainWindow::setReportStyleSheet()
{
  QString myStyle = Omgui::defaultStyleSheet(); 
  tbReport->document()->setDefaultStyleSheet(myStyle);
}

QString OmgMainWindow::helpText()
{
  //
  // Show some help info in the textbrowser until the user loads an 
  // experiment.
  //
  QString myString;
  myString += "<h1><center>";
  myString += ("openModeller Desktop - Documentation");
  myString += "</center></h1>";
  myString += "<p>";  
  myString += tr("This is the help documentation for"
      " openModeller Desktop. You can return to this help"
      " document at any stage by using Help->Documentation"
      " from the menu. Please visit the openModeller home"
      " page at http://openmodeller.sf.net for additional"
      " information about openModeller.");
  myString += "</p>";  
  myString += "<br/>";  
  myString += "<table>";
  // input file format help 
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/localities_file.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Important notes about the occurrences file format");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("As of openModellerDesktop version 1.0.6 your occurrence "
                "files need to be in the following format:" 
                );
  myString += "<p><pre>";
  myString +=tr("#ID-tab-Taxon Name-tab-Longitude-tab-Latitude-tab-Abundance (optional)");
  myString += "</p></pre>";
  myString += "<ul>";
  myString += "<li>";
  myString +=tr("Lines starting with # are ignored."
                );
  myString += "<li>";
  myString +=tr("The -tab-  markers above should be the inserted by pressing the tab key "
                "on your keyboard." 
                );
  myString += "<li>";
  myString +=tr("The Id can be a number, text or combination there-of."
                );
  myString += "<li>";
  myString +=tr("The taxon name should be separated with spaces and may include "
                "specific and subspecific epithet. Note that names should be identical "
                "for the ths same taxon otherwise openModeller Desktop will treat "
                "each name variation as a separate taxon."
             );
  myString += "<li>";
  myString +=tr("The abundance column is optional. An abundance of 0 is used "
                "to indicate an absence record. Note that not all openModeller "
                "algorithms support the use of absence data."
             );
  myString += "<li>";
  myString +=tr("You can automatically retrieve occurrence data using the "
                "openModeller Desktop localities search tool (see below). "
                "We recommend using the 'GBIF Rest' option for best results."
                );
  myString += "</ul>";
  myString +=tr("Here is a worked example of a typical occurrences file:"
                );
  myString += "<p><pre>";
  myString +=tr("#ID    Taxon Name  Longitude   Latitude    Abundance\n");
  myString += "23842684 Acacia cyclops  115.76  -31.98  1\n"
              "23842770 Acacia cyclops  120.867 -33.7166    1\n"
              "23842772 Acacia cyclops  121.33  -33.8   1\n"
              "23842910 Acacia cyclops  115.767 -32.115 0\n"
              "23843184 Acacia cyclops  115.733 -32.05  1\n";
  myString += "</p></pre>";
  myString += "</td>";
  myString += "</tr>";
  // localities search help 
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/filefetch.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Localities Search Tool");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("You can use the localities search tool to"
                " automatically retrieve species occurrence data."
                " The localities search tool is plugin based so"
                " we provide search functionality against various"
                " online resources such as GBIF and speciesLink."
                " More data sources will be added over time."
                " If you need a custom search plugin for your "
                " institution please contact us (tim@linfiniti.com)"
                " - we may be able to help"
                );
  myString += "</td>";
  myString += "</tr>";
  // layerset manager help
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/layersetmanager.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Layer Set Manager");
  myString += "</h2>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "</td>";
  myString += "<td>";
  myString +=tr("Use the layer set manager to define layer sets."
                " Layer sets are named groups of layers. In openModeller"
                " Desktop you use layer sets to specify the raster"
                " layers that should be used for creating models and "
                " for projecting models."
                );
  myString += "</td>";
  myString += "</tr>";
  // algorithm manager
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/algorithmmanager.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString +=tr("Algorithm Manager");
  myString += "</h2>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "</td>";
  myString += "<td>";
  myString += tr("The authors of openModeller algorithms"
      " provide sensible defaults for each algorithm. With the"
      " algorithm manager you can cutomise these defaults to"
      " make the algorithm perform in a way more suited to your needs."
      " The default algorithm parameters are called 'System Profiles'."
      " You can clone any profile using the algorithm manager to create"
      " a 'User Profile', and then customise the user profile as you like."
      );
  myString += "</td>";
  myString += "</tr>";
  // experiment designer
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/filenewExperiment.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Experiment Designer");
  myString += "</h2>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "</td>";
  myString += "<td>";
  myString +=tr("Once you have defined at least one layer set"
      " and have some occurrence data available, you can use"
      " the experiment designer to prepare an experiment. An"
      " experiment is a collection of models. Once the"
      " experiment has been created, it will be loaded"
      " in the experiment tree on the left of this window, and"
      " you will be asked if you wish to start running the experiment."
     );
  // threshold tool
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/threshold.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Threshold Tool");
  myString += "</h2>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "</td>";
  myString += "<td>";
  myString +=tr("The threshold tool creates boolean model projection"
      " rasters from probability range rasters using a Minimum Cut-off "
      " Threshold Percentage. Using this tool you "
      " can:");
  myString += "<ul>";
  myString += "<li>";
  myString +=tr("Create a model projection map that shows only cells"
      " that were above a specific probabilty of occurrence.");
  myString += "</li>";
  myString += "<li>";
  myString +=tr("Create a 'hotspot' map showing predicted alpha"
      " biodiversity based on the number of thresholded probability"
      " layers for different taxa predicting presence in each cell."
      );
  myString += "</li>";
  myString += "<li>";
  myString +=tr("Create a 'consensus' model showing "
      " number of thresholded models predicting presence in each cell."
      );
  myString += "</li>";
  myString += "</ul>";
  myString += tr("To perform a threshold analysis, you can select any "
    "number of models from the experiment tree. Only succaessfully "
    "completed model projections are shown in the tree. A thumbnail "
    "preview of each model is drawn next to each entry in the tree. "
    "By sorting the tree by taxon or by algorithm, you can easily "
    "select a group of model projections.");
  myString += "\n";
  myString += tr("Selecting one or more taxa from the same algorithm "
    "group (sort by algorithm view) will effectively perform a "
    "'hotspot' analysis, where each cell in the output raster "
    "represents the number of taxa predicted presence in that cell "
    "based on the defined Minimum Cut-off Threshold Percentage.");
  myString += "\n";
  myString += tr("Selecting one or more algorithms for a given taxon "
      "(sort by taxon view) will allow you to create a 'consensus model'."
      "In a consensus model, the value of each cell in the output raster "
      "indicates the number of algorithms that predict presence for that "
      "cell - based on the Minimum Cut-off Threshold Percentage selected.");
  myString += "\n";
  myString += tr("Optionally you can write out each model projection "
      "as a boolean layer where 0 indicates absence and 1 indicates "
      "presence, based on the Minimum Cut-off Threshold Percentage selected.");
  myString += "</td>";
  myString += "</tr>";
  //nest the experiment help table in this cell
  myString += "</td>";
  myString += "</tr>";
  myString += experimentHelpText();
  /* template
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("");
  myString += "</h2>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "</td>";
  myString += "<td>";
  myString +=tr(""
                );
  myString += "</td>";
  myString += "</tr>";
  */

  
  myString += "<table>";
  return myString;
}

QString OmgMainWindow::experimentHelpText()
{
  QString myString;
  // experiment progress help
  myString += "<tr>";
  myString += "<td>";
  myString += "</td>";
  myString += "<td>";
  myString +=tr("While the "
      " experiment is running you can browse the results "
      " of models as they are completed. Click on "
      " a taxon node (when sort by taxon is enabled) "
      " or an algorithm node (when sort by algorithm is "
      " enabled) for a thumbnail overview of completed "
      " model projections. Model nodes in the tree are "
      " marked with the following symbols according to "
      " the model status:");
  myString += "</td>";
  myString += "</tr>";
  // queued
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/status_queued.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Model queued for execution ...");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("When you see this icon next to a model node "
      " it means the model is queued, awaiting execution."
      );
  myString += "</td>";
  myString += "</tr>";
  // queued
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/status_running.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Model is running ...");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("When you see this icon next to a model node "
      " it means the model is busy running. "
      " The status bar help and the progress bars at the "
      " bottom of this windows will update you with the "
      " progress of the model. For more detailed information "
      " use the 'log' tab."
      );
  myString += "</td>";
  myString += "</tr>";
  // completed
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/status_complete.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Model is complete ...");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("When you see this icon next to a model node "
      " it means the model is finished running, "
      " and it has been projected. "
      " If you click on the model node you will be "
      " able to view a detailed report for the model."
      );
  myString += "</td>";
  myString += "</tr>";
  // aborted
  myString += "<tr>";
  myString += "<td rowspan=\"2\">";
  myString +="<img src=\":/status_aborted.png\">\n";
  myString += "</td>";
  myString += "<td>";
  myString += "<h2>";
  myString += tr("Model is aborted ...");
  myString += "</h2>";
  myString += "</td>";
  myString += "</tr>";
  myString += "<tr>";
  myString += "<td>";
  myString +=tr("When you see this icon next to a model node "
      " it means the model is finished running, "
      " but it had errors and did not complete successfully. "
      " You may be able to find more information as "
      " to the cause of the model failure by consulting "
      " the logs in the log tab."
      );
  myString += "</td>";
  myString += "</tr>";

  return myString;
}

void OmgMainWindow::showSurvey()
{
  OmgSurveyWizard myWizard;
  myWizard.exec();
}
void OmgMainWindow::showHelp()
{
  //ensure the tab is not disabled since we are 
  //showing some help info on first load
  tabModelViewDetails->setEnabled(true);
  tbReport->setSearchPaths(QStringList(":/"));
  setReportStyleSheet();
  QString myHelpText;
  QSettings mySettings;
  if (mySettings.value("tipOfTheDay/tipOfTheDay",true).toBool()==true)
  {
    OmgTipFactory myFactory;
    OmgTip myTip = myFactory.getTip();
    QString myTipString; 
    myTipString += "<h1><center>";
    myTipString += tr("Tip of the day: ") + myTip.title();
    myTipString += "</center></h1><br/>";
    myTipString +="<p><img src=\":/tip.png\" align=\"left\">\n";
    myTipString += myTip.content() + "</p>";
    myTipString += "<hr/>";
    myHelpText += myTipString;
  }
  myHelpText += helpText();
  tbReport->setHtml(myHelpText);  
}

void OmgMainWindow::supportedFormats()
{
  OmgSupportedFileFormats mySupportedFileFormats;
  mySupportedFileFormats.exec();
}
