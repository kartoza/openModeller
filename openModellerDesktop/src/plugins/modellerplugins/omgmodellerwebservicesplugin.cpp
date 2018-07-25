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
#include <omgmodellerwebservicesplugin.h>
#include <omgalgorithm.h>
#include <omgalgorithmset.h>
#include <omgui.h>
//qt includes
#include <QStringList>
#include <QSettings>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QTimer>
// needed for Q_EXPORT_PLUGIN macro at the end of this file
#include <QtPlugin>

//gsoap includes
#include <openModeller.nsmap>
//#include <soapH.h>
typedef char *xsd__string;
typedef int xsd__int; 

//time in seconds between getProgress requests 
const int SLEEP_INTERVAL=5;

OmgModellerWebServicesPlugin::OmgModellerWebServicesPlugin( QObject * parent) : 
      QObject (parent), 
      OmgModellerPluginInterface() 
{
  initialise();
}

OmgModellerWebServicesPlugin::~OmgModellerWebServicesPlugin()
{
}

bool OmgModellerWebServicesPlugin::initialise()
{

  return true;

} //initialise

QString OmgModellerWebServicesPlugin::createModel(OmgModel * thepModel)
{
  mMessenger.emitModelMessage(thepModel->guid(),tr("WS: creating model"));
  mMessenger.emitModelCreationProgress(thepModel->guid(),0);
  thepModel->setError(false);

  //get the xml for the model creation process
  QString myXmlString = thepModel->toModelCreationXml().trimmed();
  //first write the model creation request xml to a file - I may remove this in future - its just for debug purposes
  QString myFileName = thepModel->taxonName();
  myFileName = myFileName.replace(" ","_");
  QString myWorkDir = QDir::convertSeparators(thepModel->workDir());
  if (!Omgui::createTextFile(myWorkDir + myFileName+"_create.xml",myXmlString))
  {
    QString myWarning(tr("Unable to open ") + myFileName.toLocal8Bit() + tr(" for writing!"));
    mMessenger.emitModelError(thepModel->guid(),myWarning);
  }
  //create a wchar array to hold the xml, using the size of the above xml
  wchar_t * myXml = convertToWideChar(myXmlString.toStdString().c_str()) ; 
  struct soap * mypSoap = getSoapConnection();
  xsd__string myWSResponse ; 
  QString myTicketQString;
  QSettings mySettings;
  QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
  //send the model request and get teh ticket id back into myWSResponse
  if ( soap_call_omws__createModel(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL, 
        myXml,
        myWSResponse) == 0 ) 
  {
    mMessenger.emitModelMessage(thepModel->guid(),tr("Create model job requested ok, getting job id... "));
    // myWSResponse is a char** and we need to convert it to a QString	  
    std::string myTicketString (myWSResponse); 
    myTicketQString = QString::fromStdString(myTicketString);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Create model job scheduled with job id: ") + myTicketQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepModel->setError(true);
    return 0;
  }
  
  // poll the ws until the job is done
  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getProgress until job is done"));
  xsd__int myProgress=static_cast<xsd__int>(-999); //undefined state!
  
  //                            aborted                  completed
  while (myProgress!=-2 && myProgress!=100)
  {
    if ( soap_call_omws__getProgress(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL,
        myWSResponse,
        myProgress) == 0 ) 
    {   
      mMessenger.emitModelMessage(thepModel->guid(),tr("Progress of Job ") + QString::number(myProgress) + "%");
      mMessenger.emitModelCreationProgress(thepModel->guid(),myProgress);
#ifdef Q_WS_WIN
      //Sleep on windows takes milliseconds
      Sleep(SLEEP_INTERVAL*1000);
#else
      //sleep on mac / linux takes seconds
      sleep(SLEEP_INTERVAL);
#endif
    }
    else
    {
      soap_print_fault(mypSoap, stderr);
      thepModel->setError(true);
      return 0;
    }
  }
  //get the log messages for this job
  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getLog"));
  //wchar_t * myWSLog;
  xsd__string myWSLog; 
  std::string myLogString ;
  QString myLogQString;
  if ( soap_call_omws__getLog(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       myWSLog
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    myLogString = myWSLog[0]; 
    myLogQString = QString::fromStdString(myLogString);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Log of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepModel->guid(), myLogQString );
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    thepModel->appendToLog(myLogQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepModel->setError(true);
    return 0;
  }
  //check if the model failed / or aborted
  if (myProgress == -2)
  {
     //model was aborted, so bail out
     mMessenger.emitModelError(thepModel->guid(), tr("Model was aborted on server, quitting....." ));
     thepModel->setError(true);
     return 0;
  }

  // now that the model is done, we can get the model result

  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getModel"));
  struct omws__getModelResponse myModelEnvelope;
  //wchar_t * myModelEnvelope;
  QString myModelQString;
  if ( soap_call_omws__getModel(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       &myModelEnvelope
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    QString myModelQString = QString::fromWCharArray(myModelEnvelope.om__ModelEnvelope);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Model of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepModel->guid(), myModelQString );
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    return myModelQString;
  }
  else
  {
    mMessenger.emitModelError(thepModel->guid(),"Get model failed!") ;
    soap_print_fault(mypSoap, stderr);
    thepModel->setError(true);
    return 0;
  }

  //
  //  @TODO delete pointers etc
  //

  return QString(tr("No model created!"));
}//createModel

void OmgModellerWebServicesPlugin::testModel(OmgModel * thepModel, OmgModelTest * thepTest)
{
  thepTest->setError(false);
  QString myLog="";

  qDebug("About to test model...");

  //get the xml for the test model process
  QString myXmlString = thepTest->toSamplerXml();

  myXmlString += thepModel->algorithm().toModelProjectionXml(thepModel->modelDefinition(), thepModel->normalizationDefinition());

  myXmlString += "<Statistics><ConfusionMatrix";

  if (thepTest->useLowestThreshold())
  {
    myXmlString += " Threshold=\"-1.0\"";
  }
  else
  {
    myXmlString += " Threshold=\"" + QString::number(thepTest->threshold()) + "\"";
  }

  myXmlString += "/><RocCurve";

  int myNumBackgroundPoints = thepTest->numBackgroundPoints();

  if ( myNumBackgroundPoints > 0)
  {
    myXmlString += " NumBackgroundPoints=\"" + QString::number(myNumBackgroundPoints) + "\"";
  }

  double myMaxOmission = thepTest->maxOmission();

  if ( myMaxOmission >= 0.0)
  {
    myXmlString += " MaxOmission=\"" + QString::number(myMaxOmission) + "\"";
  }

  myXmlString += "/></Statistics>";

  //create a wchar array to hold the xml, using the size of the above xml
  wchar_t * myXml = convertToWideChar(myXmlString.toStdString().c_str()) ; 
  struct soap * mypSoap = getSoapConnection();
  xsd__string myWSResponse ; 
  QString myTicketQString;
  QSettings mySettings;
  QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
  //send the test model request and get the ticket id back into myWSResponse
  if ( soap_call_omws__testModel(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL, 
        myXml,
        myWSResponse) == 0 ) 
  {
    mMessenger.emitModelMessage(thepModel->guid(),tr("Test model job requested ok, getting job id... "));
    // myWSResponse is a char** and we need to convert it to a QString	  
    std::string myTicketString (myWSResponse); 
    myTicketQString = QString::fromStdString(myTicketString);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Test model job scheduled with job id: ") + myTicketQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepTest->setError(true);
    return;
  }

  // poll the ws until the job is done
  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getProgress until job is done"));
  xsd__int myProgress=static_cast<xsd__int>(-999); //undefined state!
  
  //  -2 = aborted, 100 = completed
  while (myProgress!=-2 && myProgress!=100)
  {
    if ( soap_call_omws__getProgress(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL,
        myWSResponse,
        myProgress) == 0 ) 
    {   
      mMessenger.emitModelMessage(thepModel->guid(),tr("Progress of Job ") + QString::number(myProgress) + "%");
      mMessenger.emitModelCreationProgress(thepModel->guid(),myProgress);
#ifdef Q_WS_WIN
      //Sleep on windows takes milliseconds
      Sleep(SLEEP_INTERVAL*1000);
#else
      //sleep on mac / linux takes seconds
      sleep(SLEEP_INTERVAL);
#endif
    }
    else
    {
      soap_print_fault(mypSoap, stderr);
      thepTest->setError(true);
      return;
    }
  }
  //get the log messages for this job
  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getLog"));
  xsd__string myWSLog; 
  std::string myLogString ;
  QString myLogQString;
  if ( soap_call_omws__getLog(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       myWSLog
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    myLogString = myWSLog[0]; 
    myLogQString = QString::fromStdString(myLogString);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Log of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepModel->guid(), myLogQString );
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    thepTest->appendToLog(myLogQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepTest->setError(true);
    return;
  }
  //check if the test failed or aborted
  if (myProgress == -2)
  {
     //model was aborted, so bail out
     mMessenger.emitModelError(thepModel->guid(), tr("Model test was aborted on server, quitting....." ));
     thepTest->setError(true);
     return;
  }

  // now that the test is done, we can get the test result

  mMessenger.emitModelMessage(thepModel->guid(),tr("Calling getTestResult"));
  struct omws__testResponse myTestEnvelope;
  //wchar_t * myModelEnvelope;
  QString myTestResultQString;
  if ( soap_call_omws__getTestResult(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       &myTestEnvelope
       ) == 0 ) 
  {
    // myWSLogis a char** and we need to convert it to a QString	  
    QString myTestResultQString = QString::fromWCharArray(myTestEnvelope.om__TestResultEnvelope);
    mMessenger.emitModelMessage(thepModel->guid(),tr("Result of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepModel->guid(), myTestResultQString );
    mMessenger.emitModelMessage(thepModel->guid(),"-----------------------------------------------") ;

    //parse result and store in the test object
    QDomDocument myDocument("test_result");
    myDocument.setContent(myTestResultQString);
    QDomElement myStatisticsElement = myDocument.firstChildElement("Statistics");
    if (myStatisticsElement.isNull())
    {
      mMessenger.emitModelError(thepModel->guid(),"Test Result has no statistics element!");
      thepTest->setError(true);
      return;
    }

    // confusion matrix
    QDomElement myConfusionMatrixElement = myStatisticsElement.firstChildElement("ConfusionMatrix");
    if (!myConfusionMatrixElement.isNull())
    {
      if (myConfusionMatrixElement.hasAttribute("Threshold"))
      {
        thepTest->setThreshold(myConfusionMatrixElement.attribute("Threshold").toDouble());
      }

      thepTest->setAccuracy(myConfusionMatrixElement.attribute("Accuracy").toDouble());
      thepTest->setOmission(myConfusionMatrixElement.attribute("OmissionError").toDouble());
      thepTest->setCommission(myConfusionMatrixElement.attribute("CommissionError").toDouble());

      if (myConfusionMatrixElement.hasAttribute("TrueNegatives"))
      {
        thepTest->setTrueNegativeCount(myConfusionMatrixElement.attribute("TrueNegatives").toInt());
      }
      if (myConfusionMatrixElement.hasAttribute("FalseNegatives"))
      {
        thepTest->setFalseNegativeCount(myConfusionMatrixElement.attribute("FalseNegatives").toInt());
      }
      if (myConfusionMatrixElement.hasAttribute("TruePositives"))
      {
        thepTest->setTruePositiveCount(myConfusionMatrixElement.attribute("TruePositives").toInt());
      }
      if (myConfusionMatrixElement.hasAttribute("FalsePositives"))
      {
        thepTest->setFalsePositiveCount(myConfusionMatrixElement.attribute("FalsePositives").toInt());
      }
    }

    // ROC curve
    QDomElement myRocCurveElement = myStatisticsElement.firstChildElement("RocCurve");
    if (!myRocCurveElement.isNull())
    {
      thepTest->setRocScore(myRocCurveElement.attribute("Auc").toDouble());
      if (!myRocCurveElement.hasAttribute("NumBackgroundPoints"))
      {
        thepTest->setNumBackgroundPoints(myRocCurveElement.attribute("NumBackgroundPoints").toInt());
      }
      thepTest->clearRocPoints();
      QString myPoints = myRocCurveElement.attribute("Points");
      QStringList myPointsList = myPoints.split(" ");
      int myPointCount= myPointsList.size()/2;
      for ( int i = 0; i < myPointCount ; ++i ) 
      {
        QPair<double,double> myPair;
        myPair.first = myPointsList.at(i*2).toDouble();
        myPair.second = myPointsList.at((i*2)+1).toDouble();
        thepTest->addRocPoint(myPair);
      }
      QDomElement myRatioElement = myRocCurveElement.firstChildElement("Ratio");
      if (!myRatioElement.isNull())
      {
        thepTest->setMaxOmission(myRatioElement.attribute("E").toDouble());
        thepTest->setAreaRatio(myRatioElement.attribute("Value").toDouble());
      }
    }
  }
  else
  {
    mMessenger.emitModelError(thepModel->guid(),"Get test result failed!") ;
    soap_print_fault(mypSoap, stderr);
    thepTest->setError(true);
  }
}//testModel

void  OmgModellerWebServicesPlugin::projectModel(OmgProjection * thepProjection)
{
  mMessenger.emitModelMessage(thepProjection->guid(),"WS: projecting model");
  mMessenger.emitModelProjectionProgress(thepProjection->guid(),0);

  //get the xml for the model creation process
  QString myXmlString = thepProjection->toProjectionXml().trimmed();
  //first write the model projection request xml to a file - I may remove this in future - its just for debug purposes
  QString myFileName = thepProjection->taxonName();
  myFileName = myFileName.replace(" ","_");
  QString myWorkDir = QDir::convertSeparators(thepProjection->workDir());
  if (!Omgui::createTextFile(myWorkDir + myFileName+"_project.xml",myXmlString))
  {
    QString myWarning(tr("Unable to open ") + myFileName.toLocal8Bit() + tr(" for writing!"));
    mMessenger.emitModelError(thepProjection->guid(),myWarning);
  }
  //create a wchar array to hold the xml, using the size of the above xml
  wchar_t * myXml = convertToWideChar(myXmlString.toStdString().c_str()) ; 
  struct soap * mypSoap = getSoapConnection();
  xsd__string myWSResponse ; 
  QString myTicketQString;
  QSettings mySettings;
  QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
  //send the model request and get teh ticket id back into myWSResponse
  if ( soap_call_omws__projectModel(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL, 
	      myXml,
	      myWSResponse) == 0 ) 
  {
    mMessenger.emitModelMessage(thepProjection->guid(),tr("Project model job requested ok, getting job id... "));
    // myWSResponse is a char** and we need to convert it to a QString	  
    std::string myTicketString (myWSResponse); 
    myTicketQString = QString::fromStdString(myTicketString);
    mMessenger.emitModelMessage(thepProjection->guid(),tr("Project model job scheduled with job id: ") + myTicketQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepProjection->setError(true);
    return ;
  }
  
  // poll the ws until the job is done
  mMessenger.emitModelMessage(thepProjection->guid(),tr("Calling getProgress until job is done"));
  xsd__int myProgress=static_cast<xsd__int>(-999); //undefined state!
  
  //                            aborted                  completed
  while (myProgress!=-2 && myProgress!=100)
  {
    if ( soap_call_omws__getProgress(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL,
        myWSResponse,
        myProgress) == 0 ) 
    {   
      mMessenger.emitModelMessage(thepProjection->guid(),tr("Progress of Job ") + QString::number(myProgress) + "%");
      mMessenger.emitModelProjectionProgress(thepProjection->guid(),myProgress);
#ifdef Q_WS_WIN
      //Sleep on windows takes milliseconds
      Sleep(SLEEP_INTERVAL*1000);
#else
      //sleep on mac / linux takes seconds
      sleep(SLEEP_INTERVAL);
#endif
    }
    else
    {
      soap_print_fault(mypSoap, stderr);
      thepProjection->setError(true);
      return ;
    }
  }
  //get the log messages for this job
  mMessenger.emitModelMessage(thepProjection->guid(),"Calling getLog");
  //wchar_t * myWSLog;
  xsd__string myWSLog; 
  std::string myLogString ;
  QString myLogQString;
  if ( soap_call_omws__getLog(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       myWSLog
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    myLogString = myWSLog[0]; 
    myLogQString = QString::fromStdString(myLogString);
    mMessenger.emitModelMessage(thepProjection->guid(),tr("Log of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepProjection->guid(), myLogQString );
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
    thepProjection->appendToLog(myLogQString);
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepProjection->setError(true);
    return ;
  }
  //check if the model failed / or aborted
  if (myProgress == -2)
  {
     //model was aborted, so bail out
     mMessenger.emitModelError(thepProjection->guid(), tr("Model was aborted on server, quitting....." ));
     return ;
  }

  // now that the model is done, we can get the model result

  mMessenger.emitModelMessage(thepProjection->guid(),tr("Calling getLayerAsUrl"));
  xsd__string myModelUrl ;
  QString myModelUrlString;
  if ( soap_call_omws__getLayerAsUrl(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       myModelUrl
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    myModelUrlString = QString::fromStdString(myModelUrl);
    mMessenger.emitModelMessage(thepProjection->guid(),tr("Model Url of Job ") + myTicketQString + tr(" is: "));
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
    mMessenger.emitModelMessage(thepProjection->guid(), myModelUrlString );
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    thepProjection->setError(true);
    return ;
  }

  //
  // Now fetch the actual image using QHttp stuff
  //
  mMessenger.emitModelMessage(thepProjection->guid(),tr("Calling getLayerAsAttachment"));
  xsd__base64Binary myProjectionFile ;
  if ( soap_call_omws__getLayerAsAttachment(mypSoap, 
       myWSUrl.toLocal8Bit(),
       NULL,
       myWSResponse,
       myProjectionFile
       ) == 0 ) 
  {   
    // myWSLogis a char** and we need to convert it to a QString	  
    mMessenger.emitModelMessage(thepProjection->guid(),tr("Model Projection for Job ") + myTicketQString + tr(" downloaded: "));
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
    //mMessenger.emitModelMessage(thepProjection->guid(), myModelUrlString );
    mMessenger.emitModelMessage(thepProjection->guid(),"-----------------------------------------------") ;
  }
  else
  {
    soap_print_fault(mypSoap, stderr);
    //char * myReason = mypSoap->fault->SOAP_ENV__Reason->SOAP_ENV__Text;
    //std::string myReasonString ( myReason );
    //mMessenger.emitModelError(thepProjection->guid(),tr("A soap error occurred in getLayerAsAttachment call:\n") + myReasonString.c_str());
    mMessenger.emitModelError(thepProjection->guid(),tr("A soap error occurred in getLayerAsAttachment call:\nMap was probably unavailable") );
    thepProjection->setError(true);
    return ;
  }

  QString myRawImageFileName =  myWorkDir + myFileName+"_projection." + Omgui::getOutputFormatExtension();
  mMessenger.emitModelMessage(thepProjection->guid(),tr("Downloading model image file to : ") + myRawImageFileName);
  QFile myFile(myRawImageFileName);
  if (!myFile.open(QFile::WriteOnly)) 
  {
    mMessenger.emitModelError(thepProjection->guid(),
        tr("Cannot write file %1:\n%2")
        .arg(myFile.fileName())
        .arg(myFile.errorString()));
    return;
  }
  QDataStream myStream(&myFile);   // we will serialize the data into the file
  int i;
  for ( i = 0; i < myProjectionFile.__size; i++ ) 
  {
    unsigned char myValue;
    myValue = myProjectionFile.__ptr[i];
    myStream  << myValue ;   
  }
  myFile.close();
  //store the model image file name in the model object
  thepProjection->setRawImageFileName(myRawImageFileName);
  mMessenger.emitModelProjectionProgress(thepProjection->guid(),100);


  //
  //  @TODO delete pointers etc
  //

}//projectModel


const QStringList OmgModellerWebServicesPlugin::getAlgorithmList()
{ 
  //qDebug("WS: Getting om Algorithm List");
  QStringList myAlgList;
  OmgAlgorithmSet myAlgorithmSet = getAlgorithmSet();
  myAlgList = myAlgorithmSet.nameList();
  return myAlgList;
}//getAlgorithmList

const OmgAlgorithmSet OmgModellerWebServicesPlugin::getAlgorithmSet()
{
  //qDebug("WS: Getting om Algorithm Set");
  struct soap * mypSoap = getSoapConnection();
  QSettings mySettings;
  QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
  if (myWSUrl.isEmpty())
  {
    mMessenger.emitError("Web Service URL not set in getAlgorithmSet!");
  }
  OmgAlgorithmSet myAlgorithmSet;
  //omws__getAlgorithmsResponse* myWSResponse;
  struct omws__getAlgorithmsResponse myWSResponse; 
  if ( soap_call_omws__getAlgorithms(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL, NULL, &myWSResponse) == 0 ) 
  {
    wchar_t * myXmlLiteral = myWSResponse.om__AvailableAlgorithms;
    QString myXml = QString::fromWCharArray(myXmlLiteral);
    myXml = Omgui::xmlEncodeAmpersands(myXml);
    Omgui::createTextFile("/tmp/getAlgorithmSet.xml", myXml);
    myAlgorithmSet.fromXml(myXml.trimmed());
    myAlgorithmSet.setName("WebService Plugin Algorithms");
    myAlgorithmSet.setDescription("These are the algorithms available on " + myWSUrl);
  }
  else 
  {
    soap_print_fault(mypSoap, stderr);
  }
  soap_destroy(mypSoap); // delete deserialized class instances (for C++ only)
  soap_end(mypSoap);     // remove deserialized data and clean up
  soap_done(mypSoap);    // detach the gSOAP environment 
  return myAlgorithmSet; 
}//getAlgorithmSet
 
const QString OmgModellerWebServicesPlugin::getLayers(QString theBaseDir)
{
  //qDebug("WS: getLayers called");
  //emit regular refresh events
  QTimer *mypTimer = new QTimer(this);
  connect(mypTimer, SIGNAL(timeout()), &mMessenger, SLOT(refresh()));
  mypTimer->start(1000);

  //now go ahead and call getLayers on WS
  struct soap * mypSoap = getSoapConnection();
  struct omws__getLayersResponse myWSResponse;
  //wchar_t * myWSResponse;
  QSettings mySettings;
  QString myWSUrl=mySettings.value("openModeller/webServicesPlugin/url", "").toString();
  QString myXml;
  
  if ( soap_call_omws__getLayers(mypSoap, 
        myWSUrl.toLocal8Bit(),
        NULL, NULL,&myWSResponse) == 0 ) 
  {
    //myXml = "<AvailableLayers>" + QString::fromWCharArray(myWSResponse) + "</AvailableLayers>";
    myXml = QString::fromWCharArray(myWSResponse.om__AvailableLayers); 
  }
  else 
  {
    soap_print_fault(mypSoap, stderr);
  }
  soap_destroy(mypSoap); // delete deserialized class instances (for C++ only)
  soap_end(mypSoap);     // remove deserialized data and clean up
  soap_done(mypSoap);    // detach the gSOAP environment 
  mypTimer->stop();
  delete mypTimer;
  return myXml; 
}//getLayers

struct soap * OmgModellerWebServicesPlugin::getSoapConnection()
{
  QSettings mySettings;
  struct soap * mypSoap = soap_new();
  
  if (mySettings.value("proxy/proxyEnabled","0").toBool())
  {
    mypSoap->proxy_host=mySettings.value("proxy/proxyHost").toString().toLocal8Bit();
    mypSoap->proxy_port=mySettings.value("proxy/proxyPort").toInt();
    mypSoap->proxy_userid=mySettings.value("proxy/proxyUser").toString().toLocal8Bit();
    mypSoap->proxy_passwd=mySettings.value("proxy/proxyPassword").toString().toLocal8Bit();
  }
  soap_set_omode(mypSoap, SOAP_XML_TREE);
  bool myWSLoggingFlag=mySettings.value("openModeller/webServicesPlugin/doLoggingFlag", false).toBool();
  if (!myWSLoggingFlag)
  {
    //using NULL instead of "/path/to/file/" will create no logs
    // NOTE: this will only compile if line 540 in stdsoap2.h is uncommented (#define DEBUG)
    //soap_set_recv_logfile(mypSoap, NULL); 
    //soap_set_sent_logfile(mypSoap, NULL);
    //soap_set_test_logfile(mypSoap, NULL);
  }
  else
  {
    //gSoap will automatically log to RECV.log, SEND.log and ERROR.log
    //unless we have disabled them above
  }
  return mypSoap;
}//getSoapConnection

wchar_t* OmgModellerWebServicesPlugin::convertToWideChar( const char* p )
{
  wchar_t *r;
  r = new wchar_t[strlen(p)+1];
  const char *tempsource = p;
  wchar_t *tempdest = r;
  while ( (*tempdest++ = *tempsource++) );
  return r;
}

// Entry point for the Qt plugin:
Q_EXPORT_PLUGIN2(webservices_modeller_plugin, OmgModellerWebServicesPlugin);
 
