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
#include "omgmodellerlocalplugin.h"
#include "omgomlogcallback.h"
#include <omgalgorithm.h>
#include <omgmodeltest.h>
#include <omgui.h> //ancilliary helper methods
#include <omggdal.h>
//qt includes
#include <QApplication>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>
#include <QSettings>
#include <QTimer>
//needed for Q_EXPORT_PLUGIN macro at the end of this file
#include <QtPlugin>

//external lib options
#ifdef Q_WS_WIN
#include <cpl_conv.h> // for setting gdal options
#include <proj_api.h> // for setting proj options
#endif

//om includes
#include <openmodeller/om.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sampler.hh> //split sampler global fn

//
//Standard includes
#include <istream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>  // I/O 
#include <fstream>   // file I/O
#include <sstream>   // treat string as a stream
#include <string>

#include <stdexcept>

//
// A global variable needed to set up the call back for loggin from openModeller
//
QTextStream* output;
bool OmgModellerLocalPlugin::mAlgFactoryStartedFlag=false;

OmgModellerLocalPlugin::OmgModellerLocalPlugin( QObject * parent) : 
      QObject (parent),
      OmgModellerPluginInterface(), 
      mpOpenModeller(new OpenModeller()),
      mpLogCallBack (new OmgOmLogCallback(*(output)))
{
  //qDebug("Creating modeller local plugin");
  initialise();
}

OmgModellerLocalPlugin::~OmgModellerLocalPlugin()
{
    disconnect (mpLogCallBack);
    //!@note setting the callback to 0 will delete mpLogCallBack so there is no need to delete mpLogCallBack manually
    Log::instance()->setCallback( 0 );
    delete mpOpenModeller;
    //qDebug("Destroying modeller local plugin");
}

bool OmgModellerLocalPlugin::initialise()
{
  // qDebug(tr("-------Modeller Plugin Initialising---------").toLocal8Bit());
  QSettings mySettings;
  bool myVerboseFlag = mySettings.value("openModeller/modellerVerboseLogFlag",false).toBool();
  if (myVerboseFlag)
  {
    Log::instance()->setLevel( Log::Debug );
  }
  else
  {
    Log::instance()->setLevel( Log::Info);
  }
  // Note this function searches for om plugins - and should only
  // ever be run once in the life of the openModeller desktop application session
  //
  if (!OmgModellerLocalPlugin::mAlgFactoryStartedFlag)
  {
    //qDebug("Initialising openmodeller local plugin...calling AlgorithmFactory");
#ifdef Q_WS_WIN
	  AlgorithmFactory::addDir(std::string(QApplication::applicationDirPath() + QDir::separator() + "algs"));
#else
    AlgorithmFactory::searchDefaultDirs();
#endif
    OmgModellerLocalPlugin::mAlgFactoryStartedFlag=true;
  }
  else
  {
    //qDebug("Initialising openmodeller local plugin...NOT calling AlgorithmFactory");
    //do nothing
  }

#ifdef Q_WS_WIN
  // Set GDAL_DATA for openModeller lib
  QString myGdalDataPath = QApplication::applicationDirPath() + QDir::separator() + "data";
  CPLSetConfigOption("GDAL_DATA", myGdalDataPath.toLocal8Bit()); 

  // Set PROJ_LIB for openModeller lib
  QString myProjLibPath = QApplication::applicationDirPath() + QDir::separator() + "nad";
  const char * path = myProjLibPath.ascii();
  const char ** paths;
  paths = &path;
  pj_set_searchpath(1, paths);
#endif

  //set up callbacks
  Log::instance()->setCallback(mpLogCallBack );
  //Log::Debug for verbose, Log::Info for NB only
  //connect up our messenger class which delegates signals
  //since a plugin in Qt cannot directly inherit from qobject
  connect(mpLogCallBack, SIGNAL(omLogMessage(QString,QString)),
      &mMessenger, SLOT(emitModelMessage(QString,QString)));
  connect(mpLogCallBack, SIGNAL(omLogError(QString,QString)),
      &mMessenger, SLOT(emitModelError(QString,QString)));
  //connect to this class's logger too so we can insert logs into OmgModel
  //when create and project are called
  connect(mpLogCallBack, SIGNAL(omLogMessage(QString,QString)),
      this, SLOT(appendToLog(QString,QString)));
  connect(mpLogCallBack, SIGNAL(omLogError(QString,QString)),
      this, SLOT(appendToLog(QString,QString)));
  mpOpenModeller->setModelCallback( modelCallback, this );
  mpOpenModeller->setMapCallback( mapCallback, this );
  return true;

}

QString OmgModellerLocalPlugin::createModel(OmgModel * thepModel)
{
  QSettings mySettings;
  bool myVerboseFlag = mySettings.value("openModeller/modellerVerboseLogFlag",false).toBool();
  if (myVerboseFlag)
  {
    Log::instance()->setLevel( Log::Debug );
  }
  else
  {
    Log::instance()->setLevel( Log::Info); 
  }
  thepModel->setError(false);
  mModelLog="";
  QString myModelResult;
  QString myFileName = thepModel->taxonName();
  mModelGuid = thepModel->guid();
  mpLogCallBack->setModelGuid(thepModel->guid());
  myFileName = myFileName.replace(" ","_");
  mMessenger.emitModelMessage(mModelGuid,tr("Running model for : ") + myFileName.toLocal8Bit());
  mMessenger.emitModelCreationProgress(mModelGuid,0);
  //get the file path in the format of the host OS as we will be passing this
  //over to om which wont automatically convert (qt classes do)
  QString myWorkDir = QDir::toNativeSeparators(thepModel->workDir());
  //std::string myFileNameString(myFileName.toLocal8Bit());
  // now use the local om lib to run the model
  qDebug("About to run model...");
  try 
  {
    std::ostringstream myOutputStream ;
    QString myModelXml = thepModel->toModelCreationXml().trimmed();
    //for local models we need to append the modelparameters outer tag
    myModelXml = "<ModelParameters>\n"+ myModelXml + "\n</ModelParameters>";
    //first write the model xml to a file - this is just for user reference / debugging
    if (!Omgui::createTextFile(myWorkDir + QDir::separator() + myFileName+"_create.xml",myModelXml))
    {
      QString myWarning(tr("Unable to open ") + myFileName.toLocal8Bit() + tr(" for writing!"));
      mMessenger.emitModelError(mModelGuid,myWarning);
      thepModel->setLog(mModelLog);
      thepModel->setError(true);
      return QString("");
    }    
    mMessenger.emitModelMessage(mModelGuid,tr("Reading creation xml"));
    std::string myXml(myModelXml.toLocal8Bit().data());
    std::istringstream myStream;
    myStream.str(myXml);
    mMessenger.emitModelMessage(mModelGuid,tr("Reading openModeller configuration from xml"));
    ConfigurationPtr c = Configuration::readXml( myStream );
    mMessenger.emitModelMessage(mModelGuid,tr("Setting openModeller configuration from xml"));
    // This next line is where the crash is occurring for Athene cunicularia!!
    mpOpenModeller->setModelConfiguration(c);
    mMessenger.emitModelMessage(mModelGuid,tr("About to run the model"));

    // Create the model!!
    mpOpenModeller->createModel();
    mMessenger.emitModelMessage(mModelGuid,tr("Completed model creation for : ") + myFileName.toLocal8Bit());

    ConfigurationPtr myConfiguration = mpOpenModeller->getModelConfiguration();
    Configuration::writeXml( myConfiguration, myOutputStream);
    mMessenger.emitModelMessage(mModelGuid,tr("Configuration obtained for : ") + myFileName.toLocal8Bit());
    mMessenger.emitModelDone(mModelGuid);
    myModelResult=QString::fromStdString(myOutputStream.str());
    //qDebug( myModelResult.toLocal8Bit());
    mModelProgress=0;
    mMessenger.emitModelCreationProgress(mModelGuid,100);
    thepModel->setLog(mModelLog);
    return myModelResult;
  }
  catch( std::exception& e ) 
  {
    QString myError = tr("Exception caught!\n") + 
        QString::fromStdString(e.what());
    mMessenger.emitModelError(mModelGuid,myError);
    thepModel->appendToLog(mModelLog);
    thepModel->appendToLog(myError);
    thepModel->setError(true);
    return QString("");
  }
}

void OmgModellerLocalPlugin::testModel(OmgModel * thepModel, OmgModelTest * thepTest)
{
  QSettings mySettings;
  bool myVerboseFlag = mySettings.value("openModeller/modellerVerboseLogFlag",false).toBool();
  if (myVerboseFlag)
  {
    Log::instance()->setLevel( Log::Debug );
  }
  else
  {
    Log::instance()->setLevel( Log::Info); 
  }

  //OpenModeller* myOpenModeller = new OpenModeller();

  OpenModeller* myOpenModeller = mpOpenModeller;

  thepTest->setError(false);
  QString myLog="";
  // now use the local om lib to test the latest model
  qDebug("About to run model test: " + thepTest->label());
  try 
  {
    QString myModelDefinitionXml = thepModel->toSerializedModelXml().trimmed();
    std::string myModelDefinitionXmlStr(myModelDefinitionXml.toLocal8Bit().data());
    std::istringstream myModelDefinitionStream;
    myModelDefinitionStream.str(myModelDefinitionXmlStr);
    ConfigurationPtr myModelDefinitionConfig = Configuration::readXml(myModelDefinitionStream);
    myOpenModeller->setModelConfiguration(myModelDefinitionConfig);

    // Create sampler with test data
    QString mySamplerXml = thepTest->toSamplerXml();

    std::string myXml(mySamplerXml.toLocal8Bit().data());
    std::istringstream myStream;
    myStream.str(myXml);

    ConfigurationPtr c = Configuration::readXml(myStream);

    SamplerPtr myTestSampler = createSampler(c);

    // 
    // Test the model!!
    // 

    // Get confusion matrix statistics
    ConfusionMatrix myMatrix;
    if (thepTest->isInternal() && thepTest->useLowestThreshold())
    {
      myMatrix.setLowestTrainingThreshold(myOpenModeller->getModel(), myTestSampler);
      thepTest->setThreshold(myMatrix.getThreshold());
    }
    else
    {
      myMatrix.reset(thepTest->threshold());
    }
    myMatrix.calculate(myOpenModeller->getModel(), myTestSampler);
    thepTest->setAccuracy(myMatrix.getAccuracy() * 100 );
    thepTest->setOmission(myMatrix.getOmissionError() * 100 );
    thepTest->setCommission(myMatrix.getCommissionError() * 100 );
    thepTest->setTrueNegativeCount(myMatrix.getValue(0,0));
    thepTest->setFalseNegativeCount(myMatrix.getValue(0,1));
    thepTest->setFalsePositiveCount(myMatrix.getValue(1,0));
    thepTest->setTruePositiveCount(myMatrix.getValue(1,1));
    if (thepTest->useLowestThreshold())
    {
      thepTest->setThreshold(myMatrix.getThreshold());
    }

    // Get the Roc Curve Statistics
    RocCurve myRoc;
    if (thepTest->numBackgroundPoints() > 0)
    {
      //myRoc.reset(ROC_DEFAULT_RESOLUTION, thepTest->numBackgroundPoints());
      myRoc.reset();
    }
    myRoc.calculate(myOpenModeller->getModel(), myTestSampler);
    double myRocArea = myRoc.getTotalArea();
    thepTest->setRocScore(myRocArea);
    /** Get the roc graph */
    thepTest->clearRocPoints();
    int myPointCount= myRoc.numPoints();
    for ( int i = 0; i < myPointCount ; ++i ) 
    {
      QPair<double,double> myPair;
      myPair.first = myRoc.getX( i );
      myPair.second = myRoc.getY( i );
      thepTest->addRocPoint(myPair);
    }
    /** Partial ROC (optional) */
    if (thepTest->maxOmission() >= 0.0)
    {
      thepTest->setAreaRatio(myRoc.getPartialAreaRatio(thepTest->maxOmission()));
    }

    thepTest->setLog(myLog);
    //delete myOpenModeller;
  }
  catch( std::exception& e ) 
  {
    //delete myOpenModeller;
    QString myError = tr("Exception caught!\n") + 
        QString::fromStdString(e.what());
    qDebug(myError);
    mMessenger.emitModelError(mModelGuid,myError);
    thepTest->appendToLog(myLog);
    thepTest->appendToLog(myError);
    thepTest->setError(true);
  }
}

void  OmgModellerLocalPlugin::projectModel(OmgProjection * thepProjection)
{
  QSettings mySettings;
  bool myVerboseFlag = mySettings.value("openModeller/modellerVerboseLogFlag",false).toBool();
  if (myVerboseFlag)
  {
    Log::instance()->setLevel( Log::Debug );
  }
  else
  {
    Log::instance()->setLevel( Log::Info); 
  }
  mModelLog="";
  QString myFileName = thepProjection->taxonName();
  //@todo - check if this should be using thepProjection->modelGuid() rather
  mModelGuid = thepProjection->guid();
  mpLogCallBack->setModelGuid(thepProjection->guid());
  myFileName = myFileName.replace(" ","_");
  mMessenger.emitModelMessage(mModelGuid,tr("Projecting model for : ") + myFileName.toLocal8Bit());
  //get the file path in the format of the host OS as we will be passing this
  //over to om which wont automatically convert (qt classes do)
  QString myWorkDir = QDir::toNativeSeparators(thepProjection->workDir());
  //std::string myFileNameString(myFileName.toLocal8Bit());
  // now use the local om lib to run the model
  try 
  {
    std::ostringstream myOutputStream ;
    QString myModelXml = thepProjection->toProjectionXml().trimmed();
    //for local models we need to append the modelparameters outer tag
    myModelXml = "<ProjectionParameters xmlns=\"http://openmodeller.cria.org.br/xml/1.0\">\n"
      + myModelXml + "\n</ProjectionParameters>";
    //first write the model xml to a file - this is just for user reference / debugging
    if (!Omgui::createTextFile(myWorkDir + QDir::separator() + myFileName+"_project.xml",myModelXml))
    {
      QString myWarning(tr("Unable to open ") + myFileName.toLocal8Bit() + tr(" for writing!"));
      mMessenger.emitModelError(mModelGuid,myWarning);
    }
    std::string myXml(myModelXml.toLocal8Bit().data());
    std::istringstream myModelStream;
    myModelStream.str(myXml);
    ConfigurationPtr mypConfiguration = Configuration::readXml( myModelStream );
    mpOpenModeller->setProjectionConfiguration(mypConfiguration);
    //cout << "Loading Projection Environment " << theEnvironment << endl;
    std::string myEnvironmentXml = thepProjection->projectionLayersXml().toLocal8Bit().data();
    std::istringstream myEnvironmentStream;
    myEnvironmentStream.str(myEnvironmentXml);
    mypConfiguration =  Configuration::readXml( myEnvironmentStream );
    EnvironmentPtr mypEnvironment = createEnvironment( mypConfiguration );
    //cout << "Projecting to file " << theOutput << endl;
    QString myTaxonName = thepProjection->taxonName() ;
    myTaxonName.replace(" ","_");
    QString myLayerSetName = thepProjection->layerSet().name();
    myLayerSetName.replace(" ","_");
    QString myRawImageFileName =
      myTaxonName + "_" 
      + myLayerSetName + "_projection."
      + Omgui::getOutputFormatExtension();
    qDebug("About to preject model");
    qDebug(thepProjection->workDir() + QDir::separator() +
        myRawImageFileName);
    mpOpenModeller->createMap( mypEnvironment, 
        (thepProjection->workDir() + QDir::separator() +
        myRawImageFileName).toLocal8Bit().data() );
    //store the model image file name in the model object
    thepProjection->setRawImageFileName(myRawImageFileName);

    //Area stats
    //we must delete this after using - the returned pointer is NOT managed by mpOpenModeller 
    AreaStats * mypStats = mpOpenModeller->getActualAreaStats();
    thepProjection->setPercentCellsPresent(mypStats->getAreaPredictedPresent() / (double) mypStats->getTotalArea() * 100);
    thepProjection->setTotalCells(  mypStats->getTotalArea() );

    delete mypStats;

    //finish up
    mModelProgress=0;
    mMessenger.emitModelProjectionProgress(mModelGuid,100);
    thepProjection->appendToLog(mModelLog);
  }
  catch( std::exception& e ) 
  {
    QString myError = tr("Exception caught!\n") + 
        QString::fromStdString(e.what());
    mMessenger.emitModelError(mModelGuid,myError);
    thepProjection->appendToLog(mModelLog);
    thepProjection->appendToLog(myError);
    thepProjection->setError(true);
    return;
  }
}

const QStringList OmgModellerLocalPlugin::getAlgorithmList()
{ 
  //qDebug("OmgModellerLocalPlugin::getAlgorithmList() called");
  const AlgMetadata **mypAlgorithmMetadataArray = mpOpenModeller->availableAlgorithms();
  const AlgMetadata *mypAlgorithmMetadata = *mypAlgorithmMetadataArray;
  QStringList myAlgList;

  while ( mypAlgorithmMetadata  )
  {
    myAlgList.append(mypAlgorithmMetadata->name.c_str());
    *mypAlgorithmMetadataArray++;
    mypAlgorithmMetadata = *mypAlgorithmMetadataArray;
  }
  myAlgList.sort();
  return myAlgList;
}

const OmgAlgorithmSet OmgModellerLocalPlugin::getAlgorithmSet()
{
  const AlgMetadata **mypAlgorithmMetadataArray = mpOpenModeller->availableAlgorithms();
  const AlgMetadata *mypAlgorithmMetadata = *mypAlgorithmMetadataArray;

  OmgAlgorithmSet myAlgorithmSet;
  myAlgorithmSet.setName("Local Plugin Algorithms");
  myAlgorithmSet.setDescription("These are the algorithms available on your local machine ");
  
  while ( mypAlgorithmMetadata  )
  {
    QString myName(mypAlgorithmMetadata->name.c_str());
    QString myId(mypAlgorithmMetadata->id.c_str());
    OmgAlgorithm myAlgorithm = getAlgorithm(myId);
    myAlgorithmSet.addAlgorithm(myAlgorithm);
    *mypAlgorithmMetadataArray++;
    mypAlgorithmMetadata = *mypAlgorithmMetadataArray;
  }
  return myAlgorithmSet; 
}

const OmgAlgorithm OmgModellerLocalPlugin::getAlgorithm( QString theId )
{	
  const AlgMetadata **mypAlgorithmMetadataArray = mpOpenModeller->availableAlgorithms();
  const AlgMetadata *mypAlgorithmMetadata = *mypAlgorithmMetadataArray;

  while ( mypAlgorithmMetadata  )
  {
    QString myId(mypAlgorithmMetadata->id.c_str());
    if (myId==theId)
    {
      OmgAlgorithm myAlgorithm;
      myAlgorithm.setId( mypAlgorithmMetadata->id.c_str() );
      myAlgorithm.setName( mypAlgorithmMetadata->name.c_str() );
      myAlgorithm.setVersion(  mypAlgorithmMetadata->version.c_str() );
      myAlgorithm.setGuid();
      myAlgorithm.setCategorical(  mypAlgorithmMetadata->categorical);
      myAlgorithm.setAbsence( mypAlgorithmMetadata->absence );
      myAlgorithm.setAuthor( mypAlgorithmMetadata->author.c_str() );
      myAlgorithm.setCodeAuthor(  mypAlgorithmMetadata->code_author.c_str() );
      myAlgorithm.setContact(  mypAlgorithmMetadata->contact.c_str() );
      myAlgorithm.setOverview(  mypAlgorithmMetadata->overview.c_str() );
      myAlgorithm.setDescription( mypAlgorithmMetadata->description.c_str() );
      myAlgorithm.setBibliography(  mypAlgorithmMetadata->biblio.c_str() );

      //qDebug ("getting algorithm parameters from metadata");
      int myParameterCountInt = mypAlgorithmMetadata->nparam;
      AlgParamMetadata * myParameter = mypAlgorithmMetadata->param;
      //interate through parameters adding the correct control type
      for ( int i = 0; i < myParameterCountInt; i++, myParameter++ )
      {
        //qDebug("Getting next parameter");
        OmgAlgorithmParameter myOmgParameter;
        AlgParamDatatype myType = myParameter->type;
        switch (myType)
        {
          case Integer:
            myOmgParameter.setType ( "Integer" );
            break;
          case Real:
            myOmgParameter.setType ( "Real" );
            break;
          case String:
            myOmgParameter.setType ( "String" );
            break;
          default:
            myOmgParameter.setType ( "Integer" );
            break;
        }

        myOmgParameter.setName( myParameter->name.c_str() );
        myOmgParameter.setId( myParameter->id.c_str() );
        if (!myParameter->has_min==0)
        {
          myOmgParameter.setMinimum( QString::number (myParameter->min_val ));
        }
        else
        {
          //! @Note Remember I have only dealt with ints, real nos to come....
          myOmgParameter.setMinimum( QString::number(INT_MIN) );
        }
        if (!myParameter->has_max==0)
        {
          myOmgParameter.setMaximum( QString::number (myParameter->max_val ));
        }
        else
        {
          //! @Note Remember I have only dealt with ints, real nos to come....
          //myOmgParameter.setMaximum( QString::number(INT_MAX ));
          // I (Alexandre Jardim) set maximum value to 999 because QtDoubleSpinBox
          // fail if this value is greater than 999.
          myOmgParameter.setMaximum( QString::number( 999 ));
        }
        myOmgParameter.setDefault( myParameter->typical.c_str() );
        myOmgParameter.setOverview( myParameter->overview.c_str() );
        myOmgParameter.setDescription( myParameter->description.c_str() );
        myAlgorithm.addParameter(myOmgParameter);
        //qDebug(myOmgParameter.toString().toLocal8Bit());
      }
      //define the origin of the algorithm - this is used by e.g. the algmanager
      //in the gui to differentiate between user profiles and profiles 
      //obtained from the plugin
      myAlgorithm.setOrigin(OmgAlgorithm::ADAPTERPROFILE);
      //Exit loop because we have found the correct algorithm
      return myAlgorithm;
    }
    *mypAlgorithmMetadataArray++;
    mypAlgorithmMetadata = *mypAlgorithmMetadataArray;
  }
  //not found
  //qDebug("Algorithm not found for id"+theId.toLocal8Bit());
  return OmgAlgorithm();
}//getAlgorithm( QString theId )

void OmgModellerLocalPlugin::setCreationProgress (int theProgress)
{
  //qDebug("setCreationProgress: " + mModelGuid.toLocal8Bit() + " " + QString::number(theProgress).toLocal8Bit());
  if (mModelProgress != theProgress)
  {
    mModelProgress=theProgress;
    mMessenger.emitModelCreationProgress(mModelGuid,theProgress);
  }
}
void OmgModellerLocalPlugin::setProjectionProgress (int theProgress)
{
  //qDebug("setProjectionProgress: " + mModelGuid.toLocal8Bit() + " " +  QString::number(theProgress).toLocal8Bit());
  if ( theProgress > 1 )
  {
    theProgress = 1;
  }
  if (mModelProgress != theProgress)
  {
    mMapProgress=theProgress;
    mMessenger.emitModelProjectionProgress(mModelGuid,theProgress);
  }
}

void OmgModellerLocalPlugin::appendToLog( QString theGuid, QString theMessage)
{
  mModelLog += "\n" + theMessage;
}
 
const QString OmgModellerLocalPlugin::getLayers(QString theBaseDir)
{
  QTimer *mypTimer = new QTimer(this);
  connect(mypTimer, SIGNAL(timeout()), &mMessenger, SLOT(refresh()));
  mypTimer->start(1000);
  QString myLayers = Omgui::getLayers(theBaseDir);
  mypTimer->stop();
  delete mypTimer;
  return myLayers;
}


//
// non member callback functions
//
void modelCallback( float theProgress, void *thepPlugin )
{
 //cast the plugin to a local plugin
 //qDebug("modelCallBack: " + QString::number(theProgress).toLocal8Bit());
 OmgModellerLocalPlugin * mypPlugin = (OmgModellerLocalPlugin *) thepPlugin;
 mypPlugin->setCreationProgress(static_cast<int>(theProgress*100));
}
void mapCallback( float theProgress, void *thepPlugin )
{
 //qDebug("mapCallBack: " + QString::number(theProgress).toLocal8Bit());
 //cast the plugin to a local plugin
 OmgModellerLocalPlugin * mypPlugin = (OmgModellerLocalPlugin *) thepPlugin;
 int myProgress = static_cast<int>(theProgress*100);
 if ( myProgress > 100 )
 {
	 myProgress = 100;
 }
 mypPlugin->setProjectionProgress( myProgress );
}
    
// Entry point for the Qt plugin:
Q_EXPORT_PLUGIN2(local_modeller_plugin, OmgModellerLocalPlugin);
