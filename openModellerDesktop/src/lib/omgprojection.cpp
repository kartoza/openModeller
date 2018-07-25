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
#include "omgui.h"
#include "omggraph.h"
#include "omggdal.h"
#include "omgprojection.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QSettings>
#include <QVariant>
#include <QImage>
#include <QPainter>
#include <QRadialGradient>

#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsrasterbandstats.h>

OmgProjection::OmgProjection() : OmgSerialisable(), OmgGuid(),
  mModelDefinition(""),
  mNormalizationDefinition(""),
  mCompletedFlag(false),
  mErrorFlag(false),
  mThumbnailFileName(""),
  mPreviewFileName(""),
  mLegendFileName(""),
  mRawImageFileName(""),
  mColouredImageFileName(""),
  mOmRasterFormat("GreyTiff"),
  mWorkDir(""),
  mPercentCellsPresent(0.0),
  mTotalCells(0.0),
  mProjectionLog("")
{

}

OmgProjection::~OmgProjection()
{

}

const bool OmgProjection::isValid() const
{
  //! @todo implement proper check here!
  return true;
}

void OmgProjection::reset()
{
  mErrorFlag=false;
  mCompletedFlag=false;
}

QString OmgProjection::toString() const
{
  QString myProjection("Projection Description:\n---------------------\n");
  myProjection += "Output Format Layer:\n";
  myProjection += mOutputFormatLayer.name() + "\n";
  myProjection += "Layers used to project the model:\n";
  myProjection += mProjectionLayerSet.toString() + "\n";
  myProjection += "Output preview image file:\n";
  myProjection += mPreviewFileName+"\n";
  myProjection += "Output legend file:\n";
  myProjection += mLegendFileName+"\n";
  return myProjection;
}

bool OmgProjection::fromXml(const QString theXml) 
{
  QDomDocument myDocument("projection");
  myDocument.setContent(theXml);
  QDomElement myRootElement = myDocument.firstChildElement("Projection");
  if (myRootElement.isNull())
  {
    //TODO - just make this a warning
    //qDebug("top element could not be found!");
    return false;
  }
  mCompletedFlag = (myRootElement.attribute("Completed")=="true") ? true : false;
  mErrorFlag = (myRootElement.attribute("Error")=="true") ? true : false;
  QString myGuid = myRootElement.attribute("Guid");
  setGuid( myGuid );
  setWorkDir(myRootElement.firstChildElement("WorkingDirectory").text());
  setModelDefinition(myRootElement.firstChildElement("Definition").text());
  setNormalizationDefinition(myRootElement.firstChildElement("Normalization").text());
  mProjectionLog = myRootElement.firstChildElement("Log").text();
  //
  // Get and set the projection stats
  //
  QDomElement myStatsElement = myRootElement.firstChildElement("Stats");
  setPercentCellsPresent(myStatsElement.attribute("PercentCellsPresent").toDouble());
  setTotalCells(myStatsElement.attribute("TotalCells").toDouble());
  //get the timing data
  QDomElement myTimingElement = myRootElement.firstChildElement("Timing");
  mStartDateTimeStamp.setTime_t(myTimingElement.attribute("Start").toUInt());
  mEndDateTimeStamp.setTime_t(myTimingElement.attribute("End").toUInt());
  //get the output images
  QDomElement myOutputImagesElement=myRootElement.firstChildElement("OutputImages");
  QDomElement myThumbnailElement=myOutputImagesElement.firstChildElement("ThumbnailImage");
  QDomElement myPreviewElement=myOutputImagesElement.firstChildElement("PreviewImage");
  QDomElement myLegendElement=myOutputImagesElement.firstChildElement("LegendImage");
  QDomElement myRawImageElement=myOutputImagesElement.firstChildElement("RawImage");
  QDomElement myColouredImageElement=myOutputImagesElement.firstChildElement("ColouredImage");
  setThumbnailFileName( myThumbnailElement.text() );
  setPreviewFileName( myPreviewElement.text() );
  setLegendFileName( myLegendElement.text() );
  setRawImageFileName( myRawImageElement.text() );
  setColouredImageFileName( myColouredImageElement.text() );

  //
  // Restore the format specifier that om should write images to e.g. GrayTiff
  //
  QDomElement myOmRasterFormatElement=myOutputImagesElement.firstChildElement("OmRasterFormat");
  if (!myOmRasterFormatElement.isNull())
  {
    setOmRasterFormat(myOmRasterFormatElement.text());
  }
  //
  // Restore the output layer format
  //
  QDomElement myOutputFormatElement = myRootElement.firstChildElement("OutputFormatLayer").firstChildElement("Map");
  OmgLayer myLayer;
  myLayer.setName(myOutputFormatElement.attribute("Id"));
  QString myCategoricalString = myOutputFormatElement.attribute("IsCategorical" );
  myLayer.setCategorical((myCategoricalString=="0")? false : true);
  mOutputFormatLayer=myLayer;
  //
  // Restore the projections layerset
  //
  QDomElement myLayerSetElement2 = myRootElement.firstChildElement("ProjectionLayerSet").firstChildElement();
  QDomDocument myLayerSetDoc2("layerset");
  //note we need to do a deep copy here because the
  //element is shared otherwise and when we
  //reparent it the loop stops after the first node
  //as no more siblings are found!
  QDomElement myCopy2 = myLayerSetElement2.cloneNode().toElement();
  myLayerSetDoc2.appendChild(myCopy2);
  QString myXml2 = myLayerSetDoc2.toString();
  OmgLayerSet myLayerSet2;
  //now hand over the xml snippet to the layerset class to be deserialised
  if (myLayerSet2.fromXml(myXml2))
  {
    mProjectionLayerSet = myLayerSet2;
  }
  else
  {
    //return false;
  }
  //
  //restore the algorithm section
  //
  QDomElement myAlgorithmElement= myRootElement.firstChildElement("Algorithm");
  if (!myAlgorithmElement.isNull())
  {
    OmgAlgorithm myAlgorithm;
    //get a textual xml representation of the param tag
    QDomDocument myAlgorithmDoc("algorithm");
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myCopy = myAlgorithmElement.cloneNode().toElement();
    myAlgorithmDoc.appendChild(myCopy);
    QString myXml = myAlgorithmDoc.toString();
    //now hand over the xml snippet to the model class to be deserialised
    if (myAlgorithm.fromXml(myXml))
    {
      setAlgorithm(myAlgorithm);
    }
    else
    {
      //return false;
    }
  }

  return true;
}

QString OmgProjection::toXml() const
{
  //
  // This does not follow the same schema as libopenmodeller
  // since for the gui we have to serialise the extra properties
  // of the OmgProjection class. See 
  // toProjectionXml() for libom compliant representations
  //
  // Also note that the xml produced below is pretty ugly and 
  // will be completely redesigned in a future release
  //
  QString myString;
  myString += "<Projection Guid=\"" + guid() + "\" Completed=\"";
  mCompletedFlag ? myString +="true" : myString += "false";
  myString += "\" Error=\"";
  mErrorFlag ? myString += "true" : myString += "false";
  myString += "\">";
  myString += "<OutputFormatLayer>";
  myString +=   mOutputFormatLayer.toXml();
  myString += "</OutputFormatLayer>\n";
  myString += "<ProjectionLayerSet>";
  myString +=   mProjectionLayerSet.toXml();
  myString += "</ProjectionLayerSet>\n";
  myString += mAlgorithm.toXml();
  myString += "<Timing Start=\"" + QString::number(mStartDateTimeStamp.toTime_t()) + "\" "
              "End=\"" + QString::number(mEndDateTimeStamp.toTime_t()) + "\"/>\n";
  myString += "<OutputImages>\n";
  myString +=   "<ThumbnailImage>";
  myString +=     mThumbnailFileName;
  myString +=   "</ThumbnailImage>\n";
  myString +=   "<PreviewImage>";
  myString +=     mPreviewFileName;
  myString +=   "</PreviewImage>\n";
  myString +=   "<LegendImage>";
  myString +=     mLegendFileName;
  myString +=   "</LegendImage>\n";
  myString +=   "<RawImage>";
  myString +=     mRawImageFileName;
  myString +=   "</RawImage>\n";
  myString +=   "<ColouredImage>";
  myString +=     mColouredImageFileName;
  myString +=   "</ColouredImage>\n";
  myString +=   "<OmRasterFormat>";
  myString +=     mOmRasterFormat;
  myString +=   "</OmRasterFormat>\n";
  myString += "</OutputImages>\n";
  myString += "<WorkingDirectory>";
  myString +=   mWorkDir;
  myString += "</WorkingDirectory>\n";
  myString += "<Definition>";
  myString +=   mModelDefinition;
  myString += "</Definition>\n";
  myString += "<NormalizationDefinition>";
  myString +=   mNormalizationDefinition;
  myString += "</NormalizationDefinition>\n";
  myString += "<Stats ";
  myString +=   "PercentCellsPresent=\"" + QString::number(mPercentCellsPresent) + "\" " ;
  myString +=   "TotalCells=\"" + QString::number(mTotalCells) + "\"/>\n";
  myString += "<Log>\n";
  myString +=   mProjectionLog;
  myString += "</Log>\n";
  myString += "</Projection>\n";
  return myString;
}
QString OmgProjection::toProjectionXml()
{
  /* We are trying to make something like this
   * the outer projectionparameters tag will be added by the soap
   * libary, or the local modeler plugin.
   *
  <ProjectionParameters
    xmlns="http://openmodeller.cria.org.br/xml/1.0">
    <Algorithm Id="Bioclim" Version="0.2">
    <Parameters>
    <Parameter Id="StandardDeviationCutoff" Value="0.7"/></Parameters>
    <Model>
    <Bioclim Mean="221.6 2375.984008789062" StdDev="93.9951700886806
    184.4701277015361" Minimum="90 1911.670043945312" Maximum="413
    2565.010009765625"/></Model></Algorithm>
    <Environment NumLayers="2">
    <Map
    Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest"
    IsCategorical="0" Min="0" Max="2137"/>
    <Map Id="/home/renato/projects/openmodeller/examples/layers/temp_avg"
    IsCategorical="0" Min="-545.4199829101562" Max="3342.52001953125"/>
    <Mask
    Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest"/>
    </Environment>
    <OutputParameters FileType="FloatingTiff">
    <TemplateLayer
    Id="/home/renato/projects/openmodeller/examples/layers/rain_coolest"/>
    </OutputParameters>
   </ProjectionParameters>
   */
   QString myString = mAlgorithm.toModelProjectionXml(mModelDefinition,mNormalizationDefinition);
   bool myIncludeExtraNodes = false;
   myString += layerSet().toXml(myIncludeExtraNodes);
   QSettings mySettings;
   QString myFormat = mySettings.value("outputFormat","GreyTiff").toString();
   myString+="\n<OutputParameters FileType=\"" + myFormat + "\">";
   myString+="\n<TemplateLayer Id=\"" + mOutputFormatLayer.name() + "\"/>";  
   myString+="\n</OutputParameters>";
   return myString;
}  

QString OmgProjection::projectionLayersXml() 
{
  return mProjectionLayerSet.toXml();
}
QString OmgProjection::toSummaryHtml() const
{
  //
  // Main report starts here
  //
  QString myString;
  myString+=QString("  <center><table border=\"0\">\n");
  myString+=QString("  <tr>\n"); //1
  myString+=QString("    <td rowspan=\"10\"><img src=\""+mThumbnailFileName+"\"></td>\n");
  myString+=QString("    <td rowspan=\"10\">&nbsp;&nbsp;</td>\n");
  myString+=QString("    <td colspan=\"2\"><h2>" + 
      QObject::tr("openModeller Projection Report") + "</h2>\n</td>\n");
  myString+=QString("  </tr>\n\n");
  myString+=QString("  <tr>\n"); //2
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Scenario :") + "</td>\n");
  myString+=QString("    <td class=\"largeCell\">"+layerSet().name()+"</td>\n");
  myString+=QString("  </tr>\n\n");  
  myString+=QString("  <tr>\n"); //3
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Algorithm :") + "</td>\n");
  myString+=QString("    <td class=\"largeCell\">"+algorithm().name()+"</td>\n");
  myString+=QString("  </tr>\n\n");  
  myString+=QString("  <tr>\b"); //4
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Started: ") + "</td>\n");
  myString+=QString("    <td class=\"largeCell\">"+startDateTimeStamp().toString()+"</td>\n");
  myString+=QString("  </tr>\n\n");
  myString+=QString("  <tr>\b"); //5
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Completed : ") + "</td>\n");
  myString+=QString("    <td class=\"largeCell\">"+endDateTimeStamp().toString()+"</td>\n");
  myString+=QString("  </tr>\n\n");
  myString+=QString("  <tr>\b"); //6
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Processing time : ") + "</td>\n");
 
  if( startDateTimeStamp().secsTo(endDateTimeStamp()) < 1 )
  {
	  myString+=QString("    <td class=\"largeCell\"> &lt; 1s </td>\n");
	  myString+=QString("  </tr>\n\n");
  }
  else
  {
	myString+=QString("    <td class=\"largeCell\">"+
		Omgui::secondsToString <long> (startDateTimeStamp().secsTo(endDateTimeStamp()),Omgui::shortTimeForm) + "</td>\n");
	myString+=QString("  </tr>\n\n");
  }

  myString+=QString("  <tr>\b"); //7
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Percentage of cells predicted present : ") + "</td>\n");
  myString += "<td>" + QString::number ( mPercentCellsPresent ) + "%</td>\n";
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b"); //8
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Total number of cells : ") + "</td>\n");
  myString += "<td>" + QString::number( mTotalCells ) + "</td>\n";
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b"); //9
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Raster Format: ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">" +
    Omgui::getOutputFormatNotes().value(mOmRasterFormat) + 
      "</td>\n");
  myString += QString("  </tr>\n\n");

  myString+=QString("  <tr>\n"); //10
  myString+=QString("    <td class=\"headerCell\"> " + QObject::tr("Projection GUID:") + "</td>\n");
  myString+=QString("    <td class=\"largeCell\">"+guid()+"</td>\n");
  myString+=QString("  </tr>\n\n");  
  myString+=QString("  </table></center>\n");
  return myString;

}

QString OmgProjection::toHtml(bool theForceFlag) const
{
  //make sure the working dir for the expreiment exists
  QDir myWorkDir(mWorkDir);
  //QString.replace() is destructive so make a copy first
  QString myTaxonName = taxonName() ;
  myTaxonName.replace(" ","_");
  QString myLayerSetName = layerSet().name();
  myLayerSetName.replace(" ","_");
  QString myOutputFileName(mWorkDir + "/" + myTaxonName + "_" 
      + myLayerSetName + "_projection_report.html");
  if (!myWorkDir.exists())
  {
    qDebug(QObject::tr("Working directory does not exist...html not saved"));
    return "";
  }
  else //save it out to file
  {
    qDebug(QObject::tr("Saving projection report as html"));
    if (QFile::exists(myOutputFileName) && !theForceFlag)
    return myOutputFileName;
  }
  QString myString;
  myString+=Omgui::getHtmlHeader();
  myString+=toSummaryHtml();
  myString+=QString("  <a name=\"outputmap\"></a> ");
  myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\">" + QObject::tr("Output map:") + "</h3>\n");
  myString+=QString("  <p><center><img src=\""+mPreviewFileName+"\"></center></p>\n");
  myString+=QString("  <p><center><img src=\""+mLegendFileName+"\"></center></p>\n");
  myString+=QString("  <a name=\"algparameters\"></a>\n");  
  myString+=QString("  <p>" + algorithm().toHtml() + "</p>\n");
  myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\"><a name=\"projectionlayers\"></a> " + QObject::tr("Projection Layers:") + "</h3>\n");  
  myString+=layerSet().toHtml();
  myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\"><a name=\"formatlayer\"></a> " + QObject::tr("Format Layer:") + "</h3>\n");  
  myString += "<p>" + mOutputFormatLayer.name() + "</p>\n";
  myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\"><a name=\"modellog\"></a> " + QObject::tr("Projection Log") + "</h3>\n");
  myString+=QString("<pre>" + mProjectionLog + "</pre>");

  //
  // make a histogram of the distribution of probabilities
  // 
  QFileInfo myRasterFileInfo(workDir() + rawImageFileName());
  if (!myRasterFileInfo.exists())
  {
    qDebug("Projection projection does not exist, no raster related summary will be generated");
  }
  else if (100==1) //disable for now
  {
    myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\"><a name=\"probability_dist\"></a>" + QObject::tr("Distribution of probabilities:") + "</h3>\n");
    QgsRasterLayer * mypRasterLayer = new QgsRasterLayer(myRasterFileInfo.filePath(), 
        myRasterFileInfo.completeBaseName());
    // register this layer with the central layers registry
    // should not be needed as its not being displayed...
    //QgsMapLayerRegistry::instance()->addMapLayer( mypRasterLayer );

    const int myBinCount=10;
    bool myIgnoreOutOfRangeFlag=true;
    bool myThoroughBandScanFlag=true;
    const int myCurrentBand=1; //base 1!
    const int myUpperThreshold=254; //we will ignore eny value over this
    const int myLowerThreshold=0; //we will ignore eny value under this
    OmgDataSeries myRasterSeries;
    myRasterSeries.setLabel("Probability");

    mypRasterLayer->populateHistogram
      (myCurrentBand,myBinCount,myIgnoreOutOfRangeFlag,myThoroughBandScanFlag); 
    QgsRasterBandStats myRasterBandStats = 
      mypRasterLayer->bandStatistics(myCurrentBand);
    for (int myBin = 0; myBin < myBinCount; myBin++)
    {
      double myValue = static_cast<double> ( myRasterBandStats.histogramVector->at(myBin) );
      if (myValue <= myUpperThreshold && myValue >= myLowerThreshold)
      {
        //myValue = 0;
        continue;
      }
      myRasterSeries << myValue;
    }
    QImage myRasterImage = QImage( 400,400,QImage::Format_ARGB32 );
    myRasterImage.fill(Qt::white);
    QPainter myRasterPainter ( &myRasterImage );
    OmgGraph myRasterGraph(&myRasterPainter);
    myRasterSeries.setFillColor(Qt::red);
    myRasterSeries.setLineColor(Qt::darkGray);
    myRasterGraph.addSeries(myRasterSeries);
    myRasterGraph.setVerticesEnabled(false);
    myRasterGraph.setVertexLabelsEnabled(false);
    myRasterGraph.setAreaFillEnabled(true);
    myRasterGraph.setSpliningEnabled(false);
    myRasterGraph.setGridLinesEnabled(true);
    myRasterGraph.render();
    myRasterImage.save(mWorkDir + "/" + myTaxonName + "_probability_histogram.png");
    myString+=QString("  <p><img src=\"" + myTaxonName + "_probability_histogram.png\"></p>\n");
    //show other raster stats
    //myString+=QString("  <h3 align=\"center\" class=\"glossyBlue\"><a name=\"probability_dist\"></a>" + QObject::tr("Output Projection Raster Properties:") + "</h3>\n");
    //myString += "<p>" + mypRasterLayer->getMetadata() + "</p>";
    //continue on with the rest of the report now.
  }

  myString+=Omgui::getHtmlFooter();
  Omgui::createTextFile(myOutputFileName , myString);
  return myOutputFileName;
}

//mutators

void OmgProjection::setLayerSet(OmgLayerSet theLayerSet)
{
  mProjectionLayerSet=theLayerSet;
}
void OmgProjection::setMaskLayerName(QString theName)
{
  //qDebug("setProjectMaskLayerName " + theName.toLocal8Bit());
  OmgLayer myLayer;
  myLayer.setName(theName);
  myLayer.setCategorical(false);
  mProjectionLayerSet.setMask(myLayer); 
}
void OmgProjection::setOutputFormatLayerName(QString theName)
{
  //qDebug("setOutputFormatLayerName " + theName.toLocal8Bit());
  mOutputFormatLayer.setName(theName);
  mOutputFormatLayer.setCategorical(false);
}
void OmgProjection::setAlgorithm(OmgAlgorithm theAlgorithm)
{
  mAlgorithm = theAlgorithm;
}
void OmgProjection::setModelGuid(QString theGuid)
{
  mModelGuid = theGuid;
}
void OmgProjection::setModelDefinition(QString theDefinition)
{
  mModelDefinition = theDefinition;
}
void OmgProjection::setNormalizationDefinition(QString theDefinition)
{
  mNormalizationDefinition = theDefinition;
}
void OmgProjection::setTaxonName(QString theTaxonName)
{
  mTaxonName = theTaxonName;
}
void OmgProjection::setError(bool theFlag)
{
  mErrorFlag=theFlag;
}
void OmgProjection::setCompleted(bool theFlag)
{
  mCompletedFlag=theFlag;
}
void OmgProjection::setStartDateTimeStamp()
{
  mStartDateTimeStamp=QDateTime::currentDateTime();
}
void OmgProjection::setStartDateTimeStamp(QDateTime theDateTimeStamp)
{
  mStartDateTimeStamp=theDateTimeStamp; 
}
void OmgProjection::setEndDateTimeStamp()
{
  mEndDateTimeStamp = QDateTime::currentDateTime();
}
void OmgProjection::setEndDateTimeStamp(QDateTime theDateTimeStamp)
{
  mEndDateTimeStamp=theDateTimeStamp; 
}
void OmgProjection::setThumbnailFileName(QString theFileName)
{
  QFileInfo myFileInfo(theFileName);
  mThumbnailFileName=myFileInfo.fileName();
}
void OmgProjection::setPreviewFileName(QString theFileName)
{
  QFileInfo myFileInfo(theFileName);
  mPreviewFileName=myFileInfo.fileName();
}
void OmgProjection::setLegendFileName(QString theFileName)
{
  QFileInfo myFileInfo(theFileName);
  mLegendFileName=myFileInfo.fileName();
}
void OmgProjection::setRawImageFileName(QString theFileName)
{
  QFileInfo myFileInfo(theFileName);
  mRawImageFileName=myFileInfo.fileName();
}
void OmgProjection::setColouredImageFileName(QString theFileName)
{
  QFileInfo myFileInfo(theFileName);
  mColouredImageFileName=myFileInfo.fileName();
}

void OmgProjection::setWorkDir(QString theDir)
{
  //ensure we have a dir and an ending slash
  QFileInfo myFileInfo(theDir);
  mWorkDir=myFileInfo.absoluteFilePath() + QDir::separator();
}

void OmgProjection::setPercentCellsPresent(double thePercent )
{
  mPercentCellsPresent=thePercent;
}

void OmgProjection::setTotalCells(double theTotal)
{
  mTotalCells=theTotal;
}

void OmgProjection::appendToLog(QString theMessage)
{
  mProjectionLog += theMessage + "\n";
}

void OmgProjection::setLog(QString theLog)
{
  mProjectionLog = theLog;
}

void OmgProjection::setOmRasterFormat(QString theFormat)
{
  mOmRasterFormat = theFormat;
}

//accessors

OmgLayerSet OmgProjection::layerSet() const
{
  return mProjectionLayerSet;
}
QString OmgProjection::maskLayerName() const
{
  return mProjectionLayerSet.maskName();
}
OmgLayer OmgProjection::outputFormatLayer() const
{
  return mOutputFormatLayer;
}
QString OmgProjection::outputFormatLayerName() const
{
  return mOutputFormatLayer.name();
}
QStringList OmgProjection::projectionLayerNames() const
{
  return mProjectionLayerSet.nameList();
}
OmgAlgorithm OmgProjection::algorithm() const
{
  return mAlgorithm;
}
QString OmgProjection::modelGuid() const
{
  return mModelGuid;
}
QString OmgProjection::modelDefinition() const
{
  return mModelDefinition;
}
QString OmgProjection::taxonName() const
{
  return mTaxonName;
}
bool OmgProjection::hasError() const
{
  return mErrorFlag;
}
bool OmgProjection::isCompleted() const
{
  return mCompletedFlag;
}
QDateTime OmgProjection::startDateTimeStamp() const
{
  return mStartDateTimeStamp;
}
QDateTime OmgProjection::endDateTimeStamp() const
{
  return mEndDateTimeStamp;
}
QString OmgProjection::thumbnailFileName() const
{
  return mThumbnailFileName;
}
QString OmgProjection::previewFileName() const
{
  return mPreviewFileName;
}
QString OmgProjection::legendFileName() const
{
  return mLegendFileName;
}
QString OmgProjection::rawImageFileName() const
{
  // This if test seems refundant but I was
  // getting a crash before adding it
  if (!mRawImageFileName.isEmpty())
  {
    return mRawImageFileName;
  }
  else
  {
    return QString();
  }
}
QString OmgProjection::colouredImageFileName() const
{
  return mColouredImageFileName;
}

QString OmgProjection::omRasterFormat() const
{
  return mOmRasterFormat;
}
QString OmgProjection::workDir() const
{
  return QDir::toNativeSeparators(mWorkDir);
}
double OmgProjection::percentCellsPresent() const
{
 return mPercentCellsPresent; 
}
double OmgProjection::totalCells() const
{
 return mTotalCells; 
}

QString OmgProjection::projectionLog() const
{
  return mProjectionLog;
}

bool OmgProjection::postProcess()
{
  if (QFile::exists(workDir() + rawImageFileName()))
  {
    //save a nice looking png of the image to disk
    OmgGdal myGdal;
    QFileInfo myFileInfo(rawImageFileName());
    //get the raw wild name but without its extension
    QString myBaseName=myFileInfo.baseName();
    QString myImageFileName = myBaseName+".png";
    //convert tif generated by om to pseudocolor png
    //qDebug("Calling omggdal::writeImage(" +  mypModel->rawImageFileName().toLocal8Bit() + ","
    //+ myImageFileName.toLocal8Bit()+")");
    //
    // Deprecated for now 
    //
    //myGdal.writeImage(mypModel->workDir() + mypModel->rawImageFileName(),
    //    mypModel->workDir() + myImageFileName);
    //mypModel->setColouredImageFileName(myImageFileName);
    //save the legend for this pseudocolour image
    QString myLegendName = myBaseName+"_legend.png";
    myGdal.makeLegend(workDir() + myLegendName, 600);
    setLegendFileName(myLegendName);
    // 
    // and a smaller version for this report
    // 
    QString myPreviewImageName = myBaseName+"_preview.png";
    myGdal.writeImage(workDir() + rawImageFileName(),
        workDir() + myPreviewImageName,
        600, //width
        400); //height
    setPreviewFileName(myPreviewImageName);
    //
    // Create ascii grid file - disabled as can result in rediculously
    // large files
    //
    //QString myAsciiFileName = myBaseName + ".asc";
    //if (!myGdal.gdal2Ascii(workDir() + rawImageFileName(),
    //      workDir() + myAsciiFileName))
    //{
    //  emit logMessage(tr("Failed to convert model raster to AsciiGrid"));
    //}

    // Create the thumbnail - the createThumbnail method
    // will gracefully handle aborted / not completed
    // models
    // 
    createThumbnail();
    //
    // Generate the mode report in html format
    //
    toHtml(true);
    return true;
  }
  else
  {
    return false;
  }
}

void OmgProjection::createThumbnail()
{
  //
  // now get a thumbnail of the projection image and paste it into our
  // painter
  QFileInfo myFileInfo(rawImageFileName());
  QString myBaseName = myFileInfo.baseName();
  QString myImageFileName = mWorkDir + 
    QDir::separator() + 
    myBaseName + "_preview.png";
  myFileInfo = QFileInfo(myImageFileName);
  if (!myFileInfo.exists())
  {
    //we will use a model failed or model not run icons
    //since the model image does not exist
    if (!isCompleted())
    {
      if (hasError())
      {
        myImageFileName = ":/status_aborted.png";
      }
      else
      {
        //show queued icon
        myImageFileName = ":/status_queued.png";
      }
    }
    else
    {
      // Image not found for some other reason - it should exist
      myImageFileName = ":/non_categorical.png";
    }
  }
  QString myThumbImageName = mWorkDir + QDir::separator() + 
    myBaseName + "_thumb.png";
  Omgui::createThumbnail(
      myImageFileName,
      myThumbImageName,
      taxonName(),
      algorithm().name(),
      layerSet().name());

  setThumbnailFileName(myThumbImageName);
}

