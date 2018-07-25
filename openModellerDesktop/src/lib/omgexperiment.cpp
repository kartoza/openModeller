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
#include "omgexperiment.h"
#include "omglocality.h" //defines OmgLocalities too!
#include "omgui.h" //ancilliary helper methods
#include "omggraph.h"
#include "omgdataseries.h"
#include <omgconfig.h> //software version no e.g. 1.0.6
//QT includes
#include <QFileInfo>
#include <QDir>
#include <QImage>
#include <QDateTime>
#include <QDomDocument>
#include <QDomElement>
#include <QStringList>
#include <QMap>
#include <QMapIterator>
#include <QMutexLocker>
#include <QPainter>
OmgExperiment::OmgExperiment() : QThread(),
  mAbortFlag(false),
  mWorkDir(""),
  mRunningFlag(false)
{
  //ensure this experiment has a valid guid	
  setGuid();
  //default the name to the guid
  setName(guid());
}
OmgExperiment::~OmgExperiment()
{
  //mCondition.wakeOne();
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while ( myIterator.hasNext() )
  {
    OmgModel *  mypModel=myIterator.next();
    delete mypModel;
  }
  // note the OmgModellerPlugin  pointer should *not* be 
  // deleted because its managed by the Omgui utility class!!
  wait();
}

QString OmgExperiment::name()
{
  return mName;
}

void OmgExperiment::setName(QString theName)
{
  mName=theName;
}


QString OmgExperiment::description()
{
  return mDescription;
}

void OmgExperiment::setDescription(QString theDescription)
{
  mDescription=theDescription;
}

QString OmgExperiment::workDir()
{
  return QDir::toNativeSeparators(mWorkDir);
}

void OmgExperiment::setWorkDir(QString theWorkDir)
{
  mWorkDir=theWorkDir;
  if (!mWorkDir.endsWith(QDir::separator()))
  {
    mWorkDir += QDir::separator();
  }
  //now update all models to use this dir
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel=myIterator.next();
    QString mySubDirName = mypModel->algorithm().name();
    mySubDirName.replace(" ","_");
    mypModel->setWorkDir(mWorkDir + QDir::separator() 
                        + mySubDirName + QDir::separator());
  }
}

void OmgExperiment::addModel(OmgModel * thepModel)
{
  emit logMessage(tr("Adding model ") 
      + thepModel->algorithm().name() + " : " 
      + thepModel->taxonName());

  QString mySubDirName = thepModel->algorithm().name();
  mySubDirName.replace(" ","_");
  thepModel->setWorkDir(mWorkDir + QDir::separator() 
      + mySubDirName + QDir::separator());
  mModelVector.push_back(thepModel);
}

OmgModel * OmgExperiment::getModel(int theModelId)
{
  if (theModelId >= 0 && theModelId < mModelVector.count())
  {
    return mModelVector.at(theModelId);
  }
  return NULL;
}

OmgModel * OmgExperiment::getModel(QString theGuid)
{
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    if (mypModel->guid()==theGuid)
    {
      return mypModel;
    }
  }
  return NULL;
}

OmgModel * OmgExperiment::getModel(QString theLabel, QString theAlgorithm)
{
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    const OmgAlgorithm myAlgorithm =  mypModel->algorithm();
    if (mypModel->taxonName()==theLabel && myAlgorithm.name()==theAlgorithm)
    {
      return mypModel;
    }
  }
  return NULL;
}

OmgProjection const * OmgExperiment::getProjection(QString theGuid) const
{
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    QList<OmgProjection *> myProjectionList = mypModel->projectionList();
    QListIterator<OmgProjection *> myIterator(myProjectionList);
    while (myIterator.hasNext())
    {
      OmgProjection * mypProjection = myIterator.next();
      if (mypProjection->guid()==theGuid)
      {
        return mypProjection;
      }
    }
  }
  return NULL;
}

void OmgExperiment::removeModel(QString theGuid)
{
  QMutableVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    if (mypModel->guid()==theGuid)
    {
      delete mypModel;
      myIterator.remove();
      return ;
    }
  }
  return ;
}
void OmgExperiment::removeAlgorithm(QString theName)
{
  QMutableVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    if (mypModel->algorithm().name()==theName)
    {
      delete mypModel;
      myIterator.remove();
    }
  }
  return ;
}
void OmgExperiment::removeTaxon(QString theName)
{
  QMutableVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    if (mypModel->taxonName()==theName)
    {
      delete mypModel;
      myIterator.remove();
    }
  }
  return ;
}

QStringList OmgExperiment::taxonNames()
{
  QStringList myNameList;
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    QString myName = mypModel->taxonName();
    if (!myNameList.contains(myName)) myNameList << myName;
  }
  return myNameList;
}

QStringList OmgExperiment::algorithmNames()
{
  QStringList myAlgorithmNameList;
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel *  mypModel=myIterator.next();
    QString myAlgorithmName = mypModel->algorithm().name();
    if (!myAlgorithmNameList.contains(myAlgorithmName)) 
    {
      myAlgorithmNameList << myAlgorithmName;
    }
  }
  return myAlgorithmNameList;
}

void OmgExperiment::run()
{
  //A QMutexLocker automatically handles locking and unlocking a mutex
  //QMutexLocker myLocker(&mMutex);
  mAbortFlag=false;
  mRunningFlag=true;
  emit logMessage(tr("Experiment started"));
  mLastPos=0;
  int myTotal=count();
  emit experimentMaximum(myTotal);
  //set the working directory - all output files will be saved here
  if (mWorkDir.isEmpty())
  {
    emit logMessage(tr("Experiment working directory not defined explicity, defaulting to:"));
    mWorkDir = QDir::convertSeparators(Omgui::getModelOutputDir()) 
    + QDir::separator() 
    + name()
    + QDir::separator();
    emit logMessage(mWorkDir);
  }
  //make the working dir for the expreiment
  if (!mWorkDir.endsWith(QDir::separator()))
  {
    mWorkDir += QDir::separator();
  }
      
  QDir myWorkDir( mWorkDir );
  if (!myWorkDir.exists())
  {
    emit logMessage(tr("Experiment working directory does not exist...creating"));
    QDir myDir(mWorkDir);
    if (!myDir.mkdir(mWorkDir))
    {
      emit logMessage(tr("Experiment failed to create working directory\n") + mWorkDir );
      emit experimentProgress(myTotal);
      mRunningFlag=false;
      emit experimentStopped();
      return;
    }
    else
    {
      emit logMessage(tr("Experiment created working directory\n") + mWorkDir );
    }
  }
  emit logMessage(tr("Saving before we start experiment"));
  save();
  emit logMessage(tr("Experiment saved ") + mWorkDir );
  //here we use the new Qt4 java style iterator!
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext() && mAbortFlag==false)
  {
    //regular check for experiment cancelation
    if (mAbortFlag)
    {
      emit logMessage(tr("Saving Experiment"));
      save();
      mRunningFlag=false;
      emit experimentStopped();
      return;
    }
    //end cancellation check
    OmgModel *  mypModel=myIterator.next();
    if (mypModel==NULL)
    {
      emit logMessage(tr("Model is NULL, skipping"));
      continue;
    }
    mypModel->setModellerPlugin(mpModellerPlugin);
    mypModel->setWorkDir(mWorkDir);
    mypModel->run();
    emit logMessage (mypModel->modelLog());
    //Im saving experiment each iteration now in case something goes wrong...
    emit logMessage(tr("Saving Experiment"));
    save();
    emit logMessage(tr("Experiment saved"));
    emit logMessage(mypModel->taxonName() + " done");
    if (mypModel->isCompleted())
    {
      emit modelCompleted(mypModel->guid());
    }
    mLastPos++;
    // we create teh experiment report every model so the gui can always
    // show the latests info
    toSummaryHtml(true);//true = force recreate
    emit experimentProgress(mLastPos);
  }
  toSummaryHtml(true);//true = force recreate
  //say that all models are done
  emit experimentProgress(myTotal);
  mCurrentStatus = tr("Experiment completed");
  emit logMessage(tr("Experiment stopped"));
  mRunningFlag=false;
  emit experimentStopped();
}

void OmgExperiment::save()
{
  Omgui::createTextFile(mWorkDir + QDir::separator() + guid() + ".xml" , toXml());
}

void OmgExperiment::setModellerPlugin(OmgModellerPluginInterface * thePlugin)
{
  mpModellerPlugin = thePlugin;
}

int OmgExperiment::count()
{
  return mModelVector.count();
}

void OmgExperiment::clear()
{
  mModelVector.clear();
}

void OmgExperiment::removeCompleted()
{
  int myPos=0;
  //here we use the new Qt4 java style iterator!
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel=myIterator.next();
    //! TODO Check if this logic is correct! NB!!!!
    if (mypModel->isCompleted())
    {
      mModelVector.remove(myPos);
      delete mypModel;
    }
    else
    {
      myPos++;
    }
  }
}
bool OmgExperiment::isRunning()
{
  return mRunningFlag;
}

bool OmgExperiment::isAborted()
{
  return mAbortFlag;
}

void OmgExperiment::abort()
{
  mAbortFlag=true;
  mCurrentStatus = tr("Experiment aborting...");
}

OmgModellerPluginInterface * OmgExperiment::modellerPlugin()
{
  return mpModellerPlugin;
}


void OmgExperiment::reset()
{
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    mypModel->reset();
  }
}

bool OmgExperiment::isCompleted()
{
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    if (!mypModel->isCompleted() && ! mypModel->hasError()) return false;
  }
  return true;
}

const QString OmgExperiment::toString()
{
  QString myString("Experiment details:\n=========================\n\n");
  //snazzy new java style iterators for qt4
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    myString += mypModel->toString();
    myString += "\n --- \n";
  }
  myString += "\n========================\n";
  return myString;
}
QString OmgExperiment::toSummaryHtml(bool theForceFlag) 
{
  bool myShowGraphsFlag = false;
  if (mWorkDir.isEmpty())
  {
    mWorkDir  = QDir::convertSeparators(Omgui::getModelOutputDir()) 
    + name();
  }
  //make sure the working dir for the expreiment exists
  QDir myWorkDir( mWorkDir );
  QString myOutputFileName;
  if (!myWorkDir.exists())
  {
    emit logMessage(tr("Working directory does not exist...html not saved"));
    emit logMessage("Working dir: " + mWorkDir);
    qDebug("Working dir: " + mWorkDir.toLocal8Bit());
    return "";
  }
  else //save it out to file
  {
    emit logMessage(tr("Saving experiment report as html"));
    myOutputFileName = mWorkDir + QDir::separator() + "experiment_report.html";
    if (QFile::exists(myOutputFileName) && !theForceFlag)
    {
      //if the report already exists and forceflag is not true
      //we can just return the cache one from the file system
      return myOutputFileName;
    }
  }

  //
  // Now the actual report generation part
  //
  QString myReport = Omgui::getHtmlHeader();
  myReport += getStatsHtml();
  if (myShowGraphsFlag)
  {
    myReport += getGraphsHtml("",theForceFlag);
  }
  myReport += Omgui::getHtmlFooter();
  Omgui::createTextFile(myOutputFileName , myReport);
  return myOutputFileName;
} //toSummaryHtml

QString OmgExperiment::toPrintHtml(bool theForceFlag) 
{
 bool myShowGraphsFlag = false;
  // this method differes from getSummaryHtml in that toPrintHtml
  // does not use file cached data and it will use absolute
  // paths for graph image files
  QString myReport = getStatsHtml();
  if (myShowGraphsFlag)
  {
    myReport += getGraphsHtml(mWorkDir + QDir::separator(), theForceFlag);
  }
  return myReport;
}

QString OmgExperiment::getStatsHtml()
{
  int mySuccessCount = 0;
  int myFailureCount = 0;
  int myNotRunCount = 0;
  long myTotalTime = 0; //in seconds
  //   alg name, cumulative value
  QMap<QString,double>  myAlgorithmSuccessMap;
  QMap<QString,double>  myRocScoreMap;
  QMap<QString,double>  myAccuracyMap;
  QMap<QString,double>  myOmissionMap;
  QMap<QString,double>  myCommissionMap;
  QMap<QString,double>  myPercentCellsPresentMap;
  QMap<QString,double>  myTimeMap; //all times in seconds
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    if (!mypModel->isCompleted())
    {
      if (mypModel->hasError())
      {
        ++myFailureCount;
      }
      else
      {
        ++myNotRunCount;
      }
    }
    else
    {
      //model ran  to completion
      if (mypModel->hasError())
      {
        ++myFailureCount;
      }
      else
      {
        ++mySuccessCount;
        int myElapsedTime = mypModel->startDateTimeStamp().secsTo(mypModel->endDateTimeStamp());
        //we just need to check of one of the stats exits for this alg
        if (myAccuracyMap.contains(mypModel->algorithm().name()))
        {
          //qDebug("Appending " + mypModel->algorithm().name().toLocal8Bit() 
          //    + " to experiment stats");
          myAlgorithmSuccessMap[mypModel->algorithm().name()] = 
            myAlgorithmSuccessMap.value(mypModel->algorithm().name()) + 1;
          myRocScoreMap[mypModel->algorithm().name()] = 
            myRocScoreMap.value(mypModel->algorithm().name()) +
            mypModel->averageRocScore();
          /*
          myAccuracyMap[mypModel->algorithm().name()] = 
            myAccuracyMap.value(mypModel->algorithm().name()) +
            mypModel->accuracy();
          myOmissionMap[mypModel->algorithm().name()] = 
            myOmissionMap.value(mypModel->algorithm().name()) +
            mypModel->omission();
          myCommissionMap[mypModel->algorithm().name()] = 
            myCommissionMap.value(mypModel->algorithm().name()) +
            mypModel->commission();
          myPercentCellsPresentMap[mypModel->algorithm().name()] = 
            myPercentCellsPresentMap.value(mypModel->algorithm().name()) +
            mypModel->percentCellsPresent();
          */
          myTimeMap[mypModel->algorithm().name()] = 
            myTimeMap.value(mypModel->algorithm().name()) +
            myElapsedTime;

          myTotalTime += myElapsedTime;
        }
        else
        {
          //qDebug("Adding " + mypModel->algorithm().name().toLocal8Bit() 
          //    + " to experiment stats");
          myAlgorithmSuccessMap[mypModel->algorithm().name()] = 1;
          myRocScoreMap[mypModel->algorithm().name()] = mypModel->averageRocScore();
          /*
          myAccuracyMap[mypModel->algorithm().name()] = mypModel->accuracy();
          myOmissionMap[mypModel->algorithm().name()] = mypModel->omission();
          myCommissionMap[mypModel->algorithm().name()] = mypModel->commission();
          myPercentCellsPresentMap[mypModel->algorithm().name()] =
            mypModel->percentCellsPresent();
          */
          myTimeMap[mypModel->algorithm().name()] = myElapsedTime;
          myTotalTime += myElapsedTime;
        }
      }
    }
  }
  
  QString myString;
  myString += "<table width=\"100%\">\n";
  myString += "<tr>\n";
  myString += "<td>\n";
  myString += "<p><img src=\":/om_icon_big.png\"></p>";
  myString += "</td>\n";
  myString += "<td>\n";
  myString += "<p align=\"right\">\n";
  myString += "<h1>openModeller Desktop</h1>\n";
  myString += tr("openModeller is open source software") + "<br />";
  myString += "<a href=\"http://openmodeller.sf.net\">http://openmodeller.sf.net</a><br />";
  myString += "<h2>" + tr("Experiment Report") + "</h2>\n";
  myString += "<h2>" + Omgui::xmlEncode(mName) + "</h2>\n";
  myString += "</p>\n";
  myString += "</td>\n";
  myString += "</tr>\n";
  myString += "</table>\n";
  myString += "<h3>" + 
              tr("Description")  + "</h3>\n";
  myString += "<p>" + Omgui::xmlEncode(mDescription) + "</p>\n";
  myString += "<h3 class=\"glossyBlue\">" + 
              tr("Summary of experiment")  + "</h3>\n";
  myString += "<table width=\"100%\">\n";
  myString += "<tr>\n";
  myString += "<td></td>";
  myString += "<td><b>" + QObject::tr("Score")  + "</b></td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Total number of models:") + "</b>"; 
  myString += "</td><td>";
  myString += QString::number(mModelVector.size()); 
  myString += "<td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Succesfully completed models:") + "</b> "; 
  myString += "</td><td>";
  myString += QString::number(mySuccessCount);
  myString += "<td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Failed models:") + "</b> ";
  myString += "</td><td>";
  myString += QString::number(myFailureCount);
  myString += "<td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Models not started:") + "</b> "; 
  myString += "</td><td>";
  myString += QString::number(myNotRunCount);
  myString += "</td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Elapsed time for all models (excludes report generation time):") + "</b> "; 
  myString += "</td><td>";
  myString += Omgui::secondsToString <long> (myTotalTime, Omgui::shortTimeForm);
  myString += "<td>";
  myString += "</tr>\n";
  myString += "<tr>\n";
  myString += "<td>";
  myString += "<b>" + tr("Number of failed models:") + "</b> ";
  myString += "</td><td>";
  myString += QString::number(myFailureCount);
  myString += "<td>";
  myString += "</tr>\n";
  myString += "</table>\n";
  
  //print a table summarisng accuracy, ommision and cells predicted for each alg
  //@TODO calculate stddev for experiment summary stats too
  /*
  myString += "<h3 class=\"glossyBlue\">" + tr("Algorithm Scores")  + "</h3>\n";
  myString += "<p><table width=\"100%\">\n";
  myString += "<tr>\n";
  myString += "<td><b>" + QObject::tr("Algorithm") + "</b></td>";
  myString += "<td><b>" + QObject::tr("Avg. AUC")  + "</b></td>";
  myString += "<td><b>" + QObject::tr("Avg. Accuracy")  + "</b></td>";
  myString += "<td><b>" + QObject::tr("Avg. Omission")  + "</b></td>";
  myString += "<td><b>" + QObject::tr("Avg. Commission")  + "</b></td>";
  myString += "<td><b>" + QObject::tr("Avg. Time")  + "</b></td>";
  myString += "</tr>\n";
  QStringList myKeys = static_cast<QStringList> (myAccuracyMap.keys());
  QStringListIterator myListIterator(myKeys);
  while (myListIterator.hasNext())
  {
    QString myKey = myListIterator.next();
    myString += "<tr>\n";
    myString += "<td>" + myKey + "</td>";
    double myAverageRocScore = myRocScoreMap.value(myKey) / myAlgorithmSuccessMap.value(myKey);
    myString += "<td>" + QString::number(myAverageRocScore,'f',3) + "</td>";
    double myAverageAccuracy = myAccuracyMap.value(myKey) / myAlgorithmSuccessMap.value(myKey);
    myString += "<td>" + QString::number(myAverageAccuracy,'f',3) + "%</td>";
    double myAverageOmission = myOmissionMap.value(myKey) / myAlgorithmSuccessMap.value(myKey);
    myString += "<td>" + QString::number(myAverageOmission,'f',3) + "%</td>";
    double myAverageCommission = myCommissionMap.value(myKey) / myAlgorithmSuccessMap.value(myKey);
    if (myAverageCommission < 0)
    {
      myString += "<td> n/a </td>";
    }
    else
    {
      myString += "<td>" + QString::number(myAverageCommission,'f',3) + "%</td>";
    }
    long myAverageDuration = static_cast<long> (myTimeMap.value(myKey) / myAlgorithmSuccessMap.value(myKey));
    myString += "<td>" + Omgui::secondsToString (myAverageDuration,Omgui::shortTimeForm) +  "</td>";
    myString += "</tr>\n";
  }*/
  myString += "</table>\n";
  myString += "</p>\n";
  return myString;
}

QString OmgExperiment::getGraphsHtml(QString theImagePrefix /*=""*/, bool theForceFlag /*=false*/)
{
  if (theForceFlag)
  {
    makeAlgorithmSummaryGraphs( mWorkDir );
  }
  QString myString = "<table width=\"100%\">";
  myString += "<tr><td>";
  myString += "<h3>" + tr("Roc Score Summary") + "</h3>";
  myString += "</td><td>";
  myString += "<h3>" + tr("Accuracy Summary") + "</h3>";
  myString += "</td></tr>";
  //dummy row for pagination issues with printing
  myString += "<tr><td>";
  myString += "</td><td>";
  myString += "</td></tr>";
  //end of dummy row
  myString += "<tr><td>";
  myString += "<img src=\""  + theImagePrefix +  "roc.png\">";
  myString += "</td><td>";
  myString += "<img src=\""  + theImagePrefix +  "accuracy.png\">";
  myString += "</td></tr>";
  myString += "<tr><td>";
  myString += "<h3>" + tr("Omission Summary") + "</h3>";
  myString += "</td><td>";
  myString += "<h3>" + tr("Commission Summary") + "</h3>";
  myString += "</td></tr>";
  //dummy row for pagination issues with printing
  myString += "<tr><td>";
  myString += "</td><td>";
  myString += "</td></tr>";
  //end of dummy row
  myString += "<tr><td>";
  myString += "<img src=\""  + theImagePrefix +  "omission.png\">";
  myString += "</td><td>";
  myString += "<img src=\""  + theImagePrefix +  "commission.png\">";
  myString += "</td></tr>";
  myString += "<tr><td>";
  myString += "<h3>" + tr("Percent Cells Present Summary") + "</h3>";
  myString += "</td><td>";
  myString += "<h3>" + tr("Run Time Summary") + "</h3>";
  myString += "</td></tr>";
  //dummy row for pagination issues with printing
  myString += "<tr><td>";
  myString += "</td><td>";
  myString += "</td></tr>";
  //end of dummy row
  myString += "<tr><td>";
  myString += "<img src=\""  + theImagePrefix +  "cellspresent.png\">";
  myString += "</td><td>";
  myString += "<img src=\""  + theImagePrefix +  "time.png\"></p>";
  myString += "</td></tr>";
  myString += "</table>";
  return myString;
} //getGraphsHtml

  
//implement the serialiseable interface
QString OmgExperiment::toXml() const
{
  QString myOmgVersion = 
    QString::number(OMGMAJORVERSION) +
    QString::number(OMGMINORVERSION) + 
    QString::number(OMGRELEASEVERSION);
  QString myString("<Experiment Guid=\"" + guid() + "\" Name=\"" + Omgui::xmlEncode(mName) + "\""
		  + " Description=\"" + Omgui::xmlEncode(mDescription) + "\""
      + " openModellerDesktopVersion=\"" + myOmgVersion + "\">\n");
  myString+=QString("  <Models>\n");
  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    myString += mypModel->toXml()+"\n";
  }
  myString+=QString("  </Models>\n");
  myString+=QString("</Experiment>\n");
  return myString;
}

bool OmgExperiment::fromXml(const QString theXml) 
{
  QDomDocument myDocument("mydocument");
  myDocument.setContent(theXml);
  QDomElement myRootElement = myDocument.firstChildElement("Experiment");
  if (myRootElement.isNull())
  {
    //TODO - just make this a warning
    //qDebug("top element could not be found!");
    return false;
  }
  mName = Omgui::xmlDecode(myRootElement.attribute("Name"));
  mDescription = Omgui::xmlDecode(myRootElement.attribute("Description"));
  setGuid( myRootElement.attribute("Guid") );
  QString myActualVersion = 
    QString::number(OMGMAJORVERSION) +
    QString::number(OMGMINORVERSION) + 
    QString::number(OMGRELEASEVERSION);
  QString myFileVersion = Omgui::xmlDecode(myRootElement.attribute("openModellerDesktopVersion"));
  
  bool myOldFileFlag = myFileVersion.toInt() < myActualVersion.toInt();
  
  //get the models collection
  QDomElement myModelsElement= myRootElement.firstChildElement("Models");
  //iterate through the nested model elements 
  QDomElement myModelElement= myModelsElement.firstChildElement("Model");
  while(!myModelElement.isNull()) 
  {
    //this is needed to test nextSibling is a model element
    if (myModelElement.tagName()!="Model")
    {
      //
      // go on to the next element
      //
      myModelElement = myModelElement.nextSiblingElement();
      continue;
    }
    OmgModel * mypModel = new OmgModel();
    //get a textual xml representation of the param tag
    QDomDocument myModelDoc("model");
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myCopy = myModelElement.cloneNode().toElement();
    myModelDoc.appendChild(myCopy);
    QString myXml = myModelDoc.toString();
    //now hand over the xml snippet to the model class to be deserialised
    if (myOldFileFlag)
    {
      //try to read format from less than or equal to 1.0.7
      if (mypModel->fromXml_LTE107(myXml))
      {
        addModel(mypModel);
      }
    }
    else
    {
      if (mypModel->fromXml(myXml))
      {
        addModel(mypModel);
      }
    }
    //
    // go on to the next element
    //
    myModelElement = myModelElement.nextSiblingElement();
  }
  return true;
}

void OmgExperiment::makeAlgorithmSummaryGraphs(QString theWorkDir) 
{
  //   
  //   Iterate through the models and collect stats
  //   and add them to QMaps and QLists suitable
  //   to be passed to OmgGraph
  //   
  //   alg name, cumulative value
  QMap<QString,QList <double> > myRocScoreMap;
  QMap<QString,QList <double> > myAccuracyMap;
  /**
  QMap<QString,QList <double> > myOmissionMap;
  QMap<QString,QList <double> > myCommissionMap;
  QMap<QString,QList <double> > myPercentCellsPresentMap;
  */
  QMap<QString,QList <double> > myTimeMap;

  QVectorIterator<OmgModel *> myIterator(mModelVector);
  while (myIterator.hasNext())
  {
    OmgModel * mypModel = myIterator.next();
    QString myName = mypModel->algorithm().name(); 
    
    addValueToSummaryGraph(myRocScoreMap,myName,mypModel->averageRocScore());
    addValueToSummaryGraph(myAccuracyMap,myName,mypModel->averageAccuracy());
    //addValueToSummaryGraph(myOmissionMap,myName,mypModel->omission());
    //addValueToSummaryGraph(myCommissionMap,myName,mypModel->commission());
    //addValueToSummaryGraph(myPercentCellsPresentMap,myName,mypModel->percentCellsPresent());
    int myElapsedTime = mypModel->startDateTimeStamp().secsTo(mypModel->endDateTimeStamp());
    addValueToSummaryGraph(myTimeMap,myName,myElapsedTime);
  }

  //
  // Convert each QMap now and make it a graph
  //
  summaryMapToGraph( myRocScoreMap, theWorkDir, "roc" );
  summaryMapToGraph( myAccuracyMap, theWorkDir, "accuracy" );
  //summaryMapToGraph( myOmissionMap, theWorkDir, "omission" );
  //summaryMapToGraph( myCommissionMap, theWorkDir, "commission" );
  //summaryMapToGraph( myPercentCellsPresentMap, theWorkDir, "cellspresent" );
  summaryMapToGraph( myTimeMap, theWorkDir, "time" );
}
void OmgExperiment::addValueToSummaryGraph(
    // alg name                 item no, item value
    QMap<QString,QList <double> >  & theMap,
    QString theAlgorithmName,
    double theValue) 
{
  if (!theMap.contains(theAlgorithmName))
  {
    QList <double>  myList;
    //QString myString = "Creating " +
    //  theAlgorithmName + " << " +
    //  QString::number(1) + "," +
    //  QString::number(theValue);
    //qDebug (myString);
    myList << theValue;
    theMap[theAlgorithmName] = myList;
  }
  else
  {
    QList <double> myList = 
      theMap[theAlgorithmName]; 
    //QString myString = "Adding " + 
    //  theAlgorithmName + " << " +
    //  QString::number(myList.size()+1) + "," +
    //  QString::number(theValue);
    //qDebug (myString);
    myList << theValue;
    theMap[theAlgorithmName] = myList;
  }
}
  
void OmgExperiment::summaryMapToGraph( QMap<QString,QList <double> > &theMap, 
              QString theWorkDir, QString theImageSuffix )
{
  // create an image to paint on - use image rather than pixmap
  // as it can then be used in console only contexts
  QImage myImage(350,350,QImage::Format_ARGB32);
  myImage.fill(Qt::white);
  QPainter myPainter(&myImage);
  OmgGraph myGraph(&myPainter);
  int myCount=0;
  QMapIterator<QString,QList <double> >  myIterator(theMap);
  while (myIterator.hasNext())
  {
    myIterator.next();
    //algorithm name
    QString myName = myIterator.key();
    QList <double> myList = myIterator.value();
    OmgDataSeries mySeries;
    mySeries.setLabel( myName );
    mySeries.setYValuesList( myList );
    //mySeries.sortY();
    //use a predictable colour
    switch (myCount)
    {
      case 0: mySeries.setFillColor(Qt::red);
              break;
      case 1: mySeries.setFillColor(Qt::green);
              break;
      case 2: mySeries.setFillColor(Qt::blue);
              break;
      case 3: mySeries.setFillColor(Qt::darkMagenta);
              break;
      case 4: mySeries.setFillColor(Qt::yellow);
              break;
      case 5: mySeries.setFillColor(Qt::darkRed);
              break;
      case 6: mySeries.setFillColor(Qt::darkCyan);
              break;
      case 7: mySeries.setFillColor(Qt::darkGray);
              break;
      case 8: mySeries.setFillColor(Qt::magenta);
              break;
      default: mySeries.setFillColor(Omgui::randomColour());
    }
    mySeries.setLineColor(mySeries.fillColor().lighter());
    myGraph.addSeries( mySeries );
    ++myCount;
  }
  myGraph.setVertexLabelsEnabled(false);
  myGraph.setGridLinesEnabled(true);
  myGraph.setAreaFillEnabled(false);
  myGraph.clear();
  myGraph.render();
  myImage.save(  theWorkDir + QDir::separator() + theImageSuffix + ".png");
}
