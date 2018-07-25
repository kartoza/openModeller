/***************************************************************************
 *   Copyright (C) 2009 by Pedro Cavalcante                                *
 *   pedro.cavalcante@poli.usp.br                                          *
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
#include "omgpreanalysis.h"
#include "omglocality.h" //defines OmgLocalities too!
#include "omgui.h" //ancilliary helper methods
#include "omggraph.h"
#include "omgdataseries.h"
#include <omgconfig.h> //software version no e.g. 1.0.6
#include <omgpreanalysisalgorithmset.h>
#include <openmodeller/pre/PreParameters.hh>
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
#include <QSettings>
#include <QTextStream>

#include <openmodeller/Sampler.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/pre/PreAlgorithmFactory.hh>
#include <openmodeller/pre/PreAlgorithm.hh>

OmgPreAnalysis::OmgPreAnalysis() : QThread(),
  mWorkDir("")
{
  //ensure this experiment has a valid guid	
  setGuid();
  //default the name to the guid
  setName(guid());
  params = new PreParameters();
  mCompleted = false;
}
OmgPreAnalysis::~OmgPreAnalysis()
{
}

QString OmgPreAnalysis::name()
{
  return mName;
}

void OmgPreAnalysis::setName(QString theName)
{
  mName=theName;
}


QString OmgPreAnalysis::description()
{
  return mDescription;
}

void OmgPreAnalysis::setDescription(QString theDescription)
{
  mDescription=theDescription;
}

QString OmgPreAnalysis::taxonName()
{
  return mTaxonName;
}

QString OmgPreAnalysis::algorithmName()
{
  return mAlgorithmName;
}

void OmgPreAnalysis::setAlgorithmName(QString theAlgorithmName)
{
  mAlgorithmName=theAlgorithmName;
}

QString OmgPreAnalysis::workDir()
{
  if (!mWorkDir.isEmpty())
  {
    return mWorkDir;
  }

  QString myParentWorkDirStr = QDir::convertSeparators(Omgui::userSettingsDirPath())
    + QDir::separator()
    + "preOutputs"
    + QDir::separator();

  QDir myParentWorkDir( myParentWorkDirStr );
  if (!myParentWorkDir.exists())
  {
    emit logMessage(tr("Pre-Analysis parent directory does not exist...creating"));
    QDir myPDir(myParentWorkDirStr);
    if (!myPDir.mkdir(myParentWorkDirStr))
    {
      emit logMessage(tr("Pre-Analysis failed to create parent directory\n") + myParentWorkDirStr );
      return "";
    }
    else
    {
      emit logMessage(tr("Pre-Analysis created parent directory\n") + myParentWorkDirStr );
    }
  }

  QString myWorkDirStr = myParentWorkDirStr + name() + QDir::separator();

  QDir myWorkDir( myWorkDirStr );
  if (!myWorkDir.exists())
  {
    emit logMessage(tr("Pre-Analysis working directory does not exist...creating"));
    QDir myDir(myWorkDirStr);
    if (!myDir.mkdir(myWorkDirStr))
    {
      emit logMessage(tr("Pre-Analysis failed to create working directory\n") + myWorkDirStr );
      return "";
    }
    else
    {
      emit logMessage(tr("Pre-Analysis created working directory\n") + myWorkDirStr );
    }
  }

  mWorkDir = myWorkDirStr;

  return myWorkDirStr;
}

void OmgPreAnalysis::setWorkDir(QString theWorkDir)
{
  if (!theWorkDir.endsWith(QDir::separator()))
  {
    theWorkDir += QDir::separator();
  }

  QDir myWorkDir( theWorkDir );
  if (!myWorkDir.exists())
  {
    emit logMessage(tr("Pre-Analysis specified working directory does not exist!"));
    return;
  }

  mWorkDir = theWorkDir;
}

bool OmgPreAnalysis::isCompleted()
{
  return mCompleted;
}

void OmgPreAnalysis::setCompleted(bool newCompleted)
{
  mCompleted = newCompleted;
}

void OmgPreAnalysis::run()
{
  emit logMessage(tr("Pre-Analysis started"));
  QString myWorkDir = workDir();
  emit logMessage(tr("Working Dir: ") + myWorkDir);
  if (myWorkDir.isEmpty())
  {
    // Failed to create parent or working dir
    return;
  }

  /* 0) Parameters are already filled with SetParameters, except for SamplePtr */
  emit logMessage(tr("Filling SamplePtr..."));
  QString samplerPtrParamName("Sampler");
  setParameterValue(samplerPtrParamName, sampler);

  try
  {
    /* 1) Instantiate Pre-Analysis Algorithm */
    emit logMessage(tr("Pre-Analysis Algorithm: ") + mAlgorithmName);
    emit logMessage(tr("Instantiating Pre-Analysis Algorithm"));
    mAlgorithm = PreAlgorithmFactory::make(mAlgorithmName.toStdString(), *params);

    /* 3) Run Algorithm */
    emit logMessage(tr("Pre-Analysis is about to run..."));
    mAlgorithm->apply();
    emit logMessage(tr("Pre-Analysis has finished"));

    emit logMessage(tr("Reseting state of the algorithm"));
    mAlgorithm->resetState(*params);

    toSummaryHtml(true);//true = force recreate
    mCompleted = true;
    emit preAnalysisCompleted();
    save();
  } catch(OmException e)
  {
    emit logMessage(tr("Error while trying to execute pre-analysis: ") + e.what());
    mCompleted = true;
    emit preAnalysisCompleted();
    save();
  }

}

QString OmgPreAnalysis::toSummaryHtml(bool theForceFlag) 
{
  // Don't generate report before running the pre-analysis!
  if (!mCompleted)
  {
    return "";
  }
  bool myShowGraphsFlag = false;
  QString myWorkDir = workDir();
  if (myWorkDir.isEmpty())
  {
    // Failed to create parent or working dir
    return "";
  }
  QString myOutputFileName;
  emit logMessage(tr("Saving pre-analysis report as html"));
  myOutputFileName = myWorkDir + "preanalysis_report.html";
  if (QFile::exists(myOutputFileName) && !theForceFlag)
  {
    emit logMessage(tr("The report already exists and was not regenerated."));
    //if the report already exists and forceflag is not true
    //we can just return the cache one from the file system
    return myOutputFileName;
  }

  //
  // Now the actual report generation part
  //
  QString myReport = Omgui::getHtmlHeader();
  myReport += getStatsHtml();
  myReport += Omgui::getHtmlFooter();
  Omgui::createTextFile(myOutputFileName , myReport);
  return myOutputFileName;
} //toSummaryHtml

QString OmgPreAnalysis::toPrintHtml(bool theForceFlag) 
{
  // this method differes from getSummaryHtml in that toPrintHtml
  // does not use file cached data and it will use absolute
  // paths for graph image files
  QString myReport = getStatsHtml();
  return myReport;
}

QString OmgPreAnalysis::getStatsHtml()
{
  /* 4) Extract the results */

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
  myString += "<h2>" + tr("Pre-analysis Report") + "</h2>\n";
  myString += "<h2>" + Omgui::xmlEncode(mName) + "</h2>\n";
  myString += "</p>\n";
  myString += "</td>\n";
  myString += "</tr>\n";
  myString += "</table>\n";

  /* Input parameters information */
  myString += "<h3 class=\"glossyBlue\">" + 
              tr("Parameters") + "</h3>\n";
  myString += "<table width=\"100%\">\n";
  PreAlgorithm::stringMap info;
  mAlgorithm->getAcceptedParameters(info);
  PreAlgorithm::stringMap::iterator iterator;
  for(iterator = info.begin(); iterator != info.end(); iterator++)
  {
    myString += "<tr>\n";
    myString += "<td>\n";
    myString += "<b> " + QString::fromStdString(iterator->first) + "</b>";
    myString += "</td>";
    if(iterator->second == "double")
    {
      myString += "<td>\n";
      double value;
      getParameterValue(iterator->first, value);
      myString += QString::number(value, 'g', 6);
      myString += "</td>\n";
    } else if(iterator->second == "algorithmPtr")
    {
      myString += "<td>\n";
      AlgorithmPtr value;
      getParameterValue(iterator->first, value);
      const AlgMetadata *algMetadata = value->getMetadata();
      myString += QString::fromStdString(algMetadata->name);
      myString += "</td></tr>\n";
      int n = algMetadata->nparam;
      Configuration::subsection_list param = value->getConfiguration()->getSubsection("Parameters")->getAllSubsections();
      if(param.size() == 0)
        continue;
      myString += "<tr><td></td><td>";
      myString += "<h4>" + tr("Algorithm parameters") + "</h4></td></tr>\n";
      for(int i = 0; i < param.size(); i++)
      {
        QString paramId = QString::fromStdString(param[i]->getAttribute("Id"));
        AlgParamMetadata *paramsPtr = algMetadata->param;
        for(; QString::fromStdString(paramsPtr->id) != paramId; paramsPtr++);
        QString paramName = QString::fromStdString(paramsPtr->name);
        QString paramValue = QString::fromStdString(param[i]->getAttribute("Value"));
        myString += "<tr><td><td></td>";
        myString += paramName;
        myString += "</td><td>";
        myString += paramValue;
        myString += "</td></tr>";
      }
    } else if(iterator->second == "samplerPtr")
    {
      myString += "<td>Taxon Name</td>\n";
      myString += "<td>" + mTaxonName + "</td></tr>\n";
      myString += "<tr><td></td><td>Layer Set</td>\n";
      myString += "<td>" + mLayerSet + "</td></tr>\n";
      myString += "<tr><td></td><td>Mask Layer</td>\n";
      myString += "<td>" + mMaskLayerName + "</td></tr>\n";
    } else
    {
      myString += "[ERROR: unrecognized parameter datatype! (" + QString::fromStdString(iterator->second) + ")]";
    }
    myString += "</tr>\n";
  } 
  myString += "</table>\n";

  /* Output parameters by Layer */
  myString += "<h3 class=\"glossyBlue\">" + 
              tr("Results for each Layer") + "</h3>\n";
  myString += "<table width=\"100%\">\n";
  info.clear();
  mAlgorithm->getLayerResultSpec(info);
  myString += "<tr><td><b>Layer</b></td>";
  for(iterator = info.begin(); iterator != info.end(); iterator++)
  {
    myString += "<td><b>" + QString::fromStdString(iterator->first) + "</b></td>";
  }
  myString += "</tr>";
  int layerNum;
  unsigned int numLayers = sampler->numIndependent();
  // qDebug("numLayers: " + QString::number(numLayers));
  for(layerNum = 0; layerNum < numLayers; layerNum++)
  {
    QString layerId = QString::fromStdString(sampler->getEnvironment()->getLayerPath(layerNum));
    myString += "<tr>\n";
    myString += "<td>";
    myString += layerId;
    myString += "</td>";
    for(iterator = info.begin(); iterator != info.end(); iterator++)
    {
      // qDebug("====\nfirst: " + QString::fromStdString(iterator->first) + "\nsecond: " + QString::fromStdString(iterator->second));
      if(iterator->second == "int")
      {
        myString += "<td>";
        int value;
        getLayerResult(layerId, iterator->first, value);
        myString += QString::number(value);
        myString += "</td>";
      }
      else if(iterator->second == "double")
      {
        double value;
        myString += "<td>";
        getLayerResult(layerId, iterator->first, value);
        myString += QString::number(value);
        myString += "</td>";
      }
      else
      {
        myString += "<td>[not recognized]</td>";
      }
    }
    myString += "</tr>\n";
  }
  myString += "</table>\n";

  /* Output parameters by LayerSet information */
  info.clear();
  mAlgorithm->getLayersetResultSpec(info);
  if(info.begin() == info.end())
  {
    // No results for Layerset
  }
  else
  {
    myString += "<h3 class=\"glossyBlue\">" + 
                tr("Results for the layerset ") + "</h3>\n";
    myString += "<table width=\"100%\">\n";
    for(iterator = info.begin(); iterator != info.end(); iterator++)
    {
      myString += "<tr>\n";
      myString += "<td>\n";
      myString += "<b> " + QString::fromStdString(iterator->first) + "</b>";
      myString += "</td>";
      if(iterator->second == "double")
      {
        myString += "<td>\n";
        double value;
         getParameterValue(iterator->first, value);
        myString += QString::number(value, 'g', 6);
        myString += "</td>\n";
      }
      myString += "</tr>\n";
    }
  }
  myString += "</table>\n";

    /**** DEBUG ****/
    /* Testing the mean output */
    /* double mean = 0.1;
    double variance = 0.1;
    double std_deviation = 0.1;
    double jackknife_estimate = 0.1;
    double jackknife_bias = 0.1;
  QString paramName("out_Mean");
  getParameterValue(paramName, mean);
  paramName = "out_Variance";
  getParameterValue(paramName, variance);
  paramName = "out_Deviation";
  getParameterValue(paramName, std_deviation);
  paramName = "out_Estimate";
  getParameterValue(paramName, jackknife_estimate);
  paramName = "out_Bias";
  getParameterValue(paramName, jackknife_bias);
  myString += "<h2>Mean:" + QString::number(mean) + "</h2>";
  myString += "<h2>Variance:" + QString::number(variance) + "</h2>";
  myString += "<h2>Standard deviation:" + QString::number(std_deviation) + "</h2>";
  myString += "<h2>Jackknife estimation:" + QString::number(jackknife_estimate) + "</h2>";
  myString += "<h2>Jackknife bias:" + QString::number(jackknife_bias) + "</h2>"; */
  /**** DEBUG ***/
  return myString;

}

void OmgPreAnalysis::setPreAnalysisAlgorithm(PreAlgorithm *theAlgorithm)
{
  mAlgorithm = theAlgorithm;
}

void OmgPreAnalysis::setOccurrenceDataFile(QString occurrenceDataFile)
{
  mOccurrenceDataFile = occurrenceDataFile;
}

void OmgPreAnalysis::setLayerSet(QString layerSet)
{
  mLayerSet = layerSet;
}

void OmgPreAnalysis::setTaxonName(QString name)
{
  mTaxonName = name;
}

void OmgPreAnalysis::configureSamplePtr()
{
  if(mOccurrenceDataFile.isEmpty() || mTaxonName.isEmpty() || mLayerSet.isEmpty())
      return;

  emit logMessage(tr("Creating Occurrences object..."));
  OccurrencesPtr pres = new OccurrencesImpl(0);
  OccurrencesPtr abs = new OccurrencesImpl(0);
  /* parse the Occurrence Data file */
  QFile myQFile( mOccurrenceDataFile );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
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
        emit logMessage(tr("Ignoring line with less than 4 columns in localities file."));
        continue;
      }
      else //new file format with at least 4 columns
      {
        QString myId=myList.at(0).simplified();
        QString myTaxonName=myList.at(1).simplified();
        double longitude=myList.at(2).simplified().toDouble();
        double latitude=myList.at(3).simplified().toDouble();
        double abundance=1.0; // default value
        if (myList.size() >= 5)
        {
          abundance=myList.at(4).simplified().toDouble();
        }
        if (myTaxonName!= "")
        {
          //make sure there are only single spaces separating words.
          myTaxonName=myTaxonName.replace( QRegExp(" {2,}"), " " );
          if(myTaxonName == mTaxonName)
          {
            if(abundance > 0)
              pres->createOccurrence(myId.toStdString(), longitude, latitude, -1.0, abundance);
            else
              abs->createOccurrence(myId.toStdString(), longitude, latitude, -1.0, abundance);
          }
        }
      }
    }
    myQFile.close();
  }

  emit logMessage(tr("Occurrences object created."));

  // no categories
  Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
  OmgLayerSet layer = myLayerSetMap.value(mLayerSet);
  layersFileNames = layer.nameList();
  std::vector<std::string> names, nocategs;
  for(QStringList::Iterator it = layersFileNames.begin(); it != layersFileNames.end(); ++it)
  { 
    // qDebug("-> layer found: %s", (*it).toStdString().c_str());
    names.push_back((*it).toStdString());
  }

  mMaskLayerName = layer.maskName();
  EnvironmentPtr env = createEnvironment(nocategs, names, mMaskLayerName.toStdString());

  sampler = createSampler(env, pres, abs);
}

void OmgPreAnalysis::save()
{
  Omgui::createTextFile(workDir() + "preanalysis.xml" , toXml());
}

bool OmgPreAnalysis::fromXml(const QString theXml) 
{
  qDebug("Loading Pre-Analysis from XML file");

  QDomDocument myDocument("mydocument");
  myDocument.setContent(theXml);
  QDomElement myRootElement = myDocument.firstChildElement("PreAnalysis");
  if (myRootElement.isNull())
  {
    //TODO - just make this a warning
    qDebug("top element could not be found!");
    return false;
  }
  mName = Omgui::xmlDecode(myRootElement.attribute("Name"));
  setGuid( myRootElement.attribute("Guid") );
  QString myActualVersion = 
    QString::number(OMGMAJORVERSION) +
    QString::number(OMGMINORVERSION) + 
    QString::number(OMGRELEASEVERSION);
  QString myFileVersion = Omgui::xmlDecode(myRootElement.attribute("openModellerDesktopVersion"));

  bool myOldFileFlag = myFileVersion.toInt() < myActualVersion.toInt();
  if(myOldFileFlag)
    qDebug("Xml was generated in an old version of OpenModeller. This shouldn't cause any problem.");

  setTaxonName(myRootElement.firstChildElement("TaxonName").text());
  qDebug("Taxon: " + taxonName());
  setAlgorithmName(myRootElement.firstChildElement("PreAnalysisAlgorithmName").text());
  qDebug("Pre-Analysis Algorithm: " + algorithmName());

}

QString OmgPreAnalysis::toXml() const
{
  QString myOmgVersion = 
    QString::number(OMGMAJORVERSION) +
    QString::number(OMGMINORVERSION) + 
    QString::number(OMGRELEASEVERSION);
  QString myString = "<PreAnalysis Name=\"" + mName + "\">\n";
  myString += "\t<TaxonName>" + mTaxonName + "</TaxonName>\n";
  myString += "\t<PreAnalysisAlgorithmName>" + mAlgorithmName + "</PreAnalysisAlgorithmName>\n";
  myString += "</PreAnalysis>";
  return myString;
}
