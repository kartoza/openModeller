/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
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
#include "omgmodel.h"
#include "omgui.h"
#include "omggraph.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QSettings>
#include <QImage>

#if WITH_QWT
// QWT Charting widget
#include <qwt_array.h>
#include <qwt_legend.h>
#include <qwt_plot.h> 
#include <qwt_plot_curve.h> 
#include <qwt_plot_grid.h>
#endif //have qwt

OmgModel::OmgModel() : QThread(), OmgSerialisable(), OmgGuid(),
  mCoordinateSystem(""),
  mCoordinateSystemName(QObject::tr("None Selected")),
  mSpeciesFile(""),
  mTaxonName(""),
  mCompletedFlag(false),
  mErrorFlag(false),
  mShapefileName(""),
  mCsvFileName(""),
  mWorkDir(""),
  mModelDefinition(""),
  mNormalizationDefinition(""),
  mModelLog(""),
  mLocalitiesFilterType(NO_FILTER),
  mTestCompartmentCount(1)
{
  
}

OmgModel::~OmgModel()
{
  //delete all projections on the way out the door...
  while (!mProjectionList.isEmpty())
  {
    delete mProjectionList.takeFirst();
  }
}

//overloads setGuid of superclass
void OmgModel::setGuid(QString theGuid)
{
  //call the superclass
  OmgGuid::setGuid(theGuid);
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->setGuid(theGuid);
  }
}
void OmgModel::reset()
{
  mErrorFlag=false;
  mCompletedFlag=false;
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->reset();
  }
}

QString OmgModel::toString() const
{
  QString myString("Model Description:\n---------------------\n");
  myString += "Coordinate System Name:\n";
  myString += mCoordinateSystemName + "\n";
  myString += "Coordinate System:\n";
  myString += mCoordinateSystem + "\n";
  myString += "Species File:\n";
  myString += mSpeciesFile + "\n";
  myString += "Taxon Name:\n";
  myString += taxonName() + "\n";
  myString += "Model creationing Mask:\n";
  myString += mCreationLayerSet.maskName() + "\n";
  myString += "Layers used to create the model:\n";
  myString += mCreationLayerSet.toString() + "\n";
  myString += "Number of localities used to create the model:";
  myString += QString::number(mLocalities.count()) + "\n";
  myString += "Algorithm details:\n";
  //TODO implement toString in alg class
  myString += mAlgorithm.toString() + "\n";
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    myString += mypProjection->toString();
  }
  return myString;
}

bool OmgModel::fromXml(const QString theXml) 
{
  return fromXml_108(theXml); 
}

bool OmgModel::fromXml_108(const QString theXml) 
{
  QDomDocument myDocument("model");
  myDocument.setContent(theXml);
  QDomElement myRootElement = myDocument.firstChildElement("Model");
  if (myRootElement.isNull())
  {
    //TODO - just make this a warning
    //qDebug("top element could not be found!");
    return false;
  }
  setCompleted((myRootElement.attribute("Completed")=="true") ? true : false);
  setError((myRootElement.attribute("Error")=="true") ? true : false);
  //@TODO make guid a attribute of model rather
  QString myGuid= myRootElement.attribute("Guid");
  setGuid( myGuid );
  setTaxonName(myRootElement.firstChildElement("TaxonName").text());
  setSpeciesFile(myRootElement.firstChildElement("SpeciesFile").text());
  setWorkDir(myRootElement.firstChildElement("WorkingDirectory").text());
  //@TODO check if simply calling text gets back the mode def properly!

  QString aux = elementToXML( myRootElement.firstChildElement("Definition").firstChildElement("Model") );
  setModelDefinition(aux);

  aux = elementToXML( myRootElement.firstChildElement("NormalizationDefinition").firstChildElement("Normalization") );
  setNormalizationDefinition(aux);
  setLog(myRootElement.firstChildElement("Log").text());
  //
  // Parse out the model tests
  //
  QDomElement myModelTestsElement = myRootElement.firstChildElement("ModelTests");
  QDomElement myModelTestElement = myModelTestsElement.firstChildElement("ModelTest");
  mModelTests.clear();
  while(!myModelTestElement.isNull()) 
  {
    if (myModelTestElement.tagName()!="ModelTest")
    {
      myModelTestElement = myModelTestElement.nextSiblingElement();
      continue;
    }
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myTestCopy = myModelTestElement.cloneNode().toElement();
    QDomDocument myModelTestDoc("ModelTests");
    myModelTestDoc.appendChild(myTestCopy);
    QString myTestXml = myModelTestDoc.toString();

    OmgModelTest * myTest = new OmgModelTest(this);
    myTest->fromXml(myTestXml);
    addTest(myTest);
    myModelTestElement = myModelTestElement.nextSiblingElement();
  }
  // Get the coord system info
  QDomElement myCoordinateSystemElement=myRootElement.firstChildElement("CoordinateSystem");
  // Get the localities filter if any
  QDomElement myOptionsElement=myRootElement.firstChildElement("Options");
  QDomElement myFilterElement=myOptionsElement.firstChildElement("OccurrencesFilter");
  QDomElement myFilterTypeElement=myOptionsElement.firstChildElement("SpatiallyUnigue");
  if (!myFilterTypeElement.isNull())
  {
    mLocalitiesFilterType=SPATIALLY_UNIQUE;
  }
  else
  {
    myFilterTypeElement=myOptionsElement.firstChildElement("EnvironmentallyUnigue");
    if (!myFilterTypeElement.isNull())
    {
      mLocalitiesFilterType=ENVIRONMENTALLY_UNIQUE;
    }
    else
    {
      mLocalitiesFilterType=NO_FILTER;
    }
  }
  //get the coordinate system
  mCoordinateSystemName = myCoordinateSystemElement.firstChildElement("Name").text();
  mCoordinateSystem = myCoordinateSystemElement.firstChildElement("WKT").text();
  //get the timing data
  QDomElement myTimingElement = myRootElement.firstChildElement("Timing");
  mStartDateTimeStamp.setTime_t(myTimingElement.attribute("Start").toUInt());
  mEndDateTimeStamp.setTime_t(myTimingElement.attribute("End").toUInt());

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
  //
  // restore the localities
  mShapefileName =
    myRootElement.firstChildElement("Localities").firstChildElement("ShapefileName").text();
  mCsvFileName=
    myRootElement.firstChildElement("Localities").firstChildElement("CsvFileName").text();

  // @NOTE this code is a near duplication of code in omgexperiment::parsemodel()
  // and the two implementations should be refactored into one function

  mLocalities.clear();
  QDomElement myLocalityElement = myRootElement.firstChildElement("Localities").firstChildElement();
  while(!myLocalityElement.isNull()) 
  {
    if (myLocalityElement.tagName()!="Point")
    {
      myLocalityElement = myLocalityElement.nextSiblingElement();
      continue;
    }
    //qDebug("Parser found a point");
    OmgLocality myLocality;
    QString myId=myLocalityElement.attribute("Id");
    myLocality.setId(myId);
    QString myX=myLocalityElement.attribute("X");
    myLocality.setLongitude(myX.toDouble());
    QString myY=myLocalityElement.attribute("Y");
    myLocality.setLatitude(myY.toDouble());
    // this should be ignored by the omlib
    // but is used for de/serialising 
    // localities in the desktop so dont remove it
    QString myAbundance=myLocalityElement.attribute("Abundance");
    myLocality.setAbundance(myAbundance.toDouble());
    QString mySample=myLocalityElement.attribute("Sample");
    //qDebug("Sample for point: " + mySample.toLocal8Bit());
    QStringList myList=mySample.split(" ");
    QStringListIterator myIterator(myList);
    OmgSampleVector mySamples;
    while (myIterator.hasNext())
    {
      double myDouble=myIterator.next().toDouble();
      mySamples.push_back(myDouble);
    }
    myLocality.setSamples(mySamples);
    mLocalities.push_back(myLocality);
    myLocalityElement = myLocalityElement.nextSiblingElement();
  }
  //
  // Restore the creation layerset
  //
  QDomElement myLayerSetElement =
    myRootElement.firstChildElement("CreationLayerSet").firstChildElement();
  QDomDocument myLayerSetDoc("layerset");
  //note we need to do a deep copy here because the
  //element is shared otherwise and when we
  //reparent it the loop stops after the first node
  //as no more siblings are found!
  QDomElement myCopy1 = myLayerSetElement.cloneNode().toElement();
  myLayerSetDoc.appendChild(myCopy1);
  QString myXml1 = myLayerSetDoc.toString();
  OmgLayerSet myLayerSet;
  //now hand over the xml snippet to the layerset class to be deserialised
  if (myLayerSet.fromXml(myXml1))
  {
    mCreationLayerSet = myLayerSet;
  }
  else
  {
    //return false;
  }
  //
  // Restore projections...
  //
  projectionsFromXml(myRootElement);
  return true;
}

QString OmgModel::attributesToXML(const QDomNamedNodeMap attributes)
{
  QString aux;
  int myCount = attributes.size();
  
  for(int i=0; i<myCount; ++i)
  {
    aux.append(attributes.item(i).nodeName());
    aux.append("=\"");
    aux.append(attributes.item(i).nodeValue());
    aux.append("\" ");
  }
  return aux;
}

QString OmgModel::elementToXML(const QDomElement element)
{
  QString aux("");
  if(element.isNull())
  {
    return aux;
  }
  aux.append("<" + element.tagName() + " ");
  aux.append( attributesToXML( element.attributes() ));
  aux.append(">");
  aux.append( element.text() );
  if( element.hasChildNodes() )
  {
    QDomNodeList myChildNodes = element.childNodes();

    for( int i=0; i < myChildNodes.length(); ++i ) {

      QDomNode myNode = myChildNodes.at(i);

      if( myNode.isElement() ) {

        aux.append( elementToXML( myNode.toElement() ));
      }
    }
  }
  aux.append( "</" + element.tagName() + ">");
  return aux;
}

bool OmgModel::projectionsFromXml(QDomElement &theRootElement)
{
  QDomElement myElement = 
    theRootElement.firstChildElement("Projections").firstChildElement();
  while(!myElement.isNull()) 
  {
    if (myElement.tagName()!="Projection")
    {
      myElement = myElement.nextSiblingElement();
      continue;
    }
    OmgProjection * mypProjection = new OmgProjection();
    QDomDocument myDocument("projection");
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myCopy = myElement.cloneNode().toElement();
    myDocument.appendChild(myCopy);
    QString myXml = myDocument.toString();
    //now hand over the xml snippet to the projection class to be deserialised
    if (mypProjection->fromXml(myXml))
    {
      mypProjection->setModelGuid(guid());
      mProjectionList << mypProjection;
    }
    else
    {
      delete mypProjection;
    }
    myElement = myElement.nextSiblingElement();
  }
  return true;
}//projectionsFromXml

// Less Than or Equal to v 1.0.7
bool OmgModel::fromXml_LTE107(const QString theXml) 
{
  QDomDocument myDocument("model");
  myDocument.setContent(theXml);
  QDomElement myRootElement = myDocument.firstChildElement("Model");
  if (myRootElement.isNull())
  {
    //TODO - just make this a warning
    //qDebug("top element could not be found!");
    return false;
  }
  setCompleted((myRootElement.attribute("Completed")=="true") ? true : false);
  setError((myRootElement.attribute("Error")=="true") ? true : false);
  //@TODO make guid a attribute of model rather
  QString myGuid= myRootElement.attribute("Guid");
  setGuid( myGuid );
  setTaxonName(myRootElement.firstChildElement("TaxonName").text());
  setSpeciesFile(myRootElement.firstChildElement("SpeciesFile").text());
  setWorkDir(myRootElement.firstChildElement("WorkingDirectory").text());
  //@TODO check if simply calling text gets back the mode def properly!
  setModelDefinition(myRootElement.firstChildElement("Definition").text());
  setNormalizationDefinition(myRootElement.firstChildElement("Normalization").text());
  setLog(myRootElement.firstChildElement("Log").text());
  QDomElement myStatsElement = myRootElement.firstChildElement("Stats");
  mModelTests.clear();
  OmgModelTest* myModelTest = new OmgModelTest(this);
  myModelTest->setRocScore(myStatsElement.attribute("RocScore").toDouble());
  myModelTest->setAccuracy(myStatsElement.attribute("Accuracy").toDouble());
  myModelTest->setOmission(myStatsElement.attribute("Omission").toDouble());
  myModelTest->setCommission(myStatsElement.attribute("Commission").toDouble());
  myModelTest->setTruePositiveCount(myStatsElement.attribute("TruePositiveCount").toInt());
  myModelTest->setFalsePositiveCount(myStatsElement.attribute("FalsePositiveCount").toInt());
  myModelTest->setFalseNegativeCount(myStatsElement.attribute("FalseNegativeCount").toInt());
  myModelTest->setTrueNegativeCount(myStatsElement.attribute("TrueNegativeCount").toInt());
  //loop through the roc points
  myModelTest->clearRocPoints();
  QDomElement myRocStatsElement = myStatsElement.firstChildElement("RocStats");
  QDomElement myRocPointElement = myRocStatsElement.firstChildElement("RocPoint");
  while(!myRocPointElement.isNull()) 
  {
    if (myRocPointElement.tagName()!="RocPoint")
    {
      myRocPointElement = myRocPointElement.nextSiblingElement();
      continue;
    }
    QPair<double,double> myPair;
    myPair.first  = myRocPointElement.attribute("x").toDouble();
    myPair.second = myRocPointElement.attribute("y").toDouble();
    myModelTest->addRocPoint(myPair);
    myRocPointElement = myRocPointElement.nextSiblingElement();
  }
  addTest(myModelTest);
  //get teh coord system info
  QDomElement myCoordinateSystemElement=myRootElement.firstChildElement("CoordinateSystem");
  //get teh localities filter if any
  QDomElement myOptionsElement=myRootElement.firstChildElement("Options");
  QDomElement myFilterElement=myOptionsElement.firstChildElement("OccurrencesFilter");
  QDomElement myFilterTypeElement=myOptionsElement.firstChildElement("SpatiallyUnigue");
  if (!myFilterTypeElement.isNull())
  {
    mLocalitiesFilterType=SPATIALLY_UNIQUE;
  }
  else
  {
    myFilterTypeElement=myOptionsElement.firstChildElement("EnvironmentallyUnigue");
    if (!myFilterTypeElement.isNull())
    {
      mLocalitiesFilterType=ENVIRONMENTALLY_UNIQUE;
    }
    else
    {
      mLocalitiesFilterType=NO_FILTER;
    }
  }
  //get teh coordinate system
  mCoordinateSystemName = myCoordinateSystemElement.firstChildElement("Name").text();
  mCoordinateSystem = myCoordinateSystemElement.firstChildElement("WKT").text();
  //get the timing data
  QDomElement myTimingElement = myRootElement.firstChildElement("Timing");
  mStartDateTimeStamp.setTime_t(myTimingElement.attribute("Start").toUInt());
  mEndDateTimeStamp.setTime_t(myTimingElement.attribute("End").toUInt());
  //
  // Get the projection details - backwards support for older om versions
  //
  OmgProjection * mypProjection = new OmgProjection();
  mypProjection->setModelGuid(guid());
  mypProjection->setPercentCellsPresent(myStatsElement.attribute("PercentCellsPresent").toDouble());
  mypProjection->setTotalCells(myStatsElement.attribute("TotalCells").toDouble());
  QDomElement myOutputImagesElement=myRootElement.firstChildElement("OutputImages");
  QDomElement myThumbnailElement=myOutputImagesElement.firstChildElement("ThumbnailImage");
  QDomElement myPreviewElement=myOutputImagesElement.firstChildElement("PreviewImage");
  QDomElement myLegendElement=myOutputImagesElement.firstChildElement("LegendImage");
  QDomElement myRawImageElement=myOutputImagesElement.firstChildElement("RawImage");
  QDomElement myColouredImageElement=myOutputImagesElement.firstChildElement("ColouredImage");
  mypProjection->setThumbnailFileName( myThumbnailElement.text() );
  mypProjection->setPreviewFileName( myPreviewElement.text() );
  mypProjection->setLegendFileName( myLegendElement.text() );
  mypProjection->setRawImageFileName( myRawImageElement.text() );
  //assume if raw image file exists that projection was successful
  if (QFile::exists(workDir() + QDir::separator() + myRawImageElement.text()))
  {
    mypProjection->setCompleted(true);
    mypProjection->setError(hasError());
  }
  else
  {
    QString myError = workDir() + QDir::separator() + myRawImageElement.text() + "\n does not exist";
    qDebug(myError.toLocal8Bit());
    mypProjection->setCompleted(true);
    mypProjection->setError(true);
  }

  mypProjection->setColouredImageFileName( myColouredImageElement.text() );
  mypProjection->setModelGuid(myGuid);
  QDomElement myModelRasterFormatElement=myOutputImagesElement.firstChildElement("ModelRasterFormat");
  if (!myModelRasterFormatElement.isNull())
  {
    mypProjection->setOmRasterFormat(myModelRasterFormatElement.text());
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
      mypProjection->setAlgorithm(myAlgorithm);
    }
    else
    {
      //return false;
    }
  }
  //
  // restore the localities
  mShapefileName = myRootElement.firstChildElement("Localities").firstChildElement("ShapefileName").text();
  mCsvFileName= myRootElement.firstChildElement("Localities").firstChildElement("CsvFileName").text();

  // @NOTE this code is a near duplication of code in omgexperiment::parsemodel()
  // and omgmodeltest::fromXml(). these implementations should be refactored into one function

  mLocalities.clear();
  QDomElement myLocalityElement = myRootElement.firstChildElement("Localities").firstChildElement();
  while(!myLocalityElement.isNull()) 
  {
    if (myLocalityElement.tagName()!="Point")
    {
      myLocalityElement = myLocalityElement.nextSiblingElement();
      continue;
    }
    //qDebug("Parser found a point");
    OmgLocality myLocality;
    QString myId=myLocalityElement.attribute("Id");
    myLocality.setId(myId);
    QString myX=myLocalityElement.attribute("X");
    myLocality.setLongitude(myX.toDouble());
    QString myY=myLocalityElement.attribute("Y");
    myLocality.setLatitude(myY.toDouble());
    // this should be ignored by the omlib
    // but is used for de/serialising 
    // localities in the desktop so dont remove it
    QString myAbundance=myLocalityElement.attribute("Abundance");
    myLocality.setAbundance(myAbundance.toDouble());
    QString mySample=myLocalityElement.attribute("Sample");
    //qDebug("Sample for point: " + mySample.toLocal8Bit());
    QStringList myList=mySample.split(" ");
    QStringListIterator myIterator(myList);
    OmgSampleVector mySamples;
    while (myIterator.hasNext())
    {
      double myDouble=myIterator.next().toDouble();
      mySamples.push_back(myDouble);
    }
    myLocality.setSamples(mySamples);
    mLocalities.push_back(myLocality);
    myLocalityElement = myLocalityElement.nextSiblingElement();
  }
  //
  // Restore the output layer format
  //
  QDomElement myOutputFormatElement = myRootElement.firstChildElement("OutputFormatLayer").firstChildElement("Map");
  OmgLayer myLayer;
  myLayer.setName(myOutputFormatElement.attribute("Id"));
  QString myCategoricalString = myOutputFormatElement.attribute("IsCategorical" );
  myLayer.setCategorical((myCategoricalString=="0")? false : true);
  mypProjection->setOutputFormatLayerName(myLayer.name());
  //
  // Restore the creation layerset
  //
  QDomElement myLayerSetElement = myRootElement.firstChildElement("CreationLayerSet").firstChildElement();
  QDomDocument myLayerSetDoc("layerset");
  //note we need to do a deep copy here because the
  //element is shared otherwise and when we
  //reparent it the loop stops after the first node
  //as no more siblings are found!
  QDomElement myCopy1 = myLayerSetElement.cloneNode().toElement();
  myLayerSetDoc.appendChild(myCopy1);
  QString myXml1 = myLayerSetDoc.toString();
  OmgLayerSet myLayerSet;
  //now hand over the xml snippet to the layerset class to be deserialised
  if (myLayerSet.fromXml(myXml1))
  {
    mCreationLayerSet = myLayerSet;
  }
  else
  {
    //return false;
  }
  //
  // Restore the projections layerset
  //
  QDomElement myLayerSetElement2 = 
    myRootElement.firstChildElement("ProjectionLayerSet").firstChildElement();
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
    mypProjection->setLayerSet(myLayerSet2);
  }
  else
  {
    //return false;
  }
  mProjectionList << mypProjection;
  return true;
}

QString OmgModel::toXml() const 
{
  //
  // This does not follow the same schema as libopenmodeller
  // since for the gui we have to serialise the extra properties
  // of the OmgModel class. See toModelCreationXml() and 
  // toModelProjectionXml() fo libom compliant representations
  //
  // Also note that the xml produced below is pretty ugly and 
  // will be completely redesigned in a future release
  //
  QString mySampleCount=QString::number(mLocalities.count());
  QString myString;
  myString += "<Model Guid=\"" + guid() + "\" Completed=\"";
  mCompletedFlag ? myString +="true" : myString += "false";
  myString += "\" Error=\"";
  mErrorFlag ? myString += "true" : myString += "false";
  myString += "\">";
  myString += "<CoordinateSystem>";
  myString +=   "<Name>";
  myString +=     mCoordinateSystemName ;
  myString +=   "</Name>\n";
  myString +=   "<WKT>";
  myString +=     mCoordinateSystem ;
  myString +=   "</WKT>\n";
  myString += "</CoordinateSystem>\n";
  myString += "<Options>\n";
  myString += "  <OccurrencesFilter>\n";
  if (mLocalitiesFilterType==NO_FILTER)
  {
    //add nothing
  }
  else if(mLocalitiesFilterType==SPATIALLY_UNIQUE)
  {
    myString += "    <SpatiallyUnique/>\n";
  }
  else //environmentally unique
  {
    myString += "    <EnvironmentallyUnique/>\n";
  }
  myString += "  </OccurrencesFilter>\n";
  myString += "</Options>\n";
  myString += "<SpeciesFile>";
  myString +=   mSpeciesFile;
  myString += "</SpeciesFile>\n";
  myString += "<TaxonName>";
  myString +=   taxonName();
  myString += "</TaxonName>\n";
  myString += "<CreationLayerSet>";
  myString +=   mCreationLayerSet.toXml();
  myString += "</CreationLayerSet>\n";
  myString +=   mAlgorithm.toXml();
  //loop through all the localities
  myString += "<Localities>";
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    myString+=myLocality.toXml();
  }
  myString +=   "<ShapefileName>" + mShapefileName + "</ShapefileName>";
  myString +=   "<CsvFileName>" + mCsvFileName + "</CsvFileName>";
  myString += "</Localities>\n";
  myString += "<Timing Start=\"" + QString::number(startDateTimeStamp().toTime_t()) + "\" "
           +  "End=\"" + QString::number(endDateTimeStamp().toTime_t()) + "\"/>\n";
  myString += "<WorkingDirectory>";
  myString +=   mWorkDir;
  myString += "</WorkingDirectory>\n";
  myString += "<Definition>";
  myString +=   mModelDefinition;
  myString += "</Definition>\n";
  myString += "<NormalizationDefinition>";
  myString +=   mNormalizationDefinition;
  myString += "</NormalizationDefinition>\n";
  myString += "<ModelTests>\n";
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    myString += mypModelTest->toXml()+"\n";
  }
  myString += "</ModelTests>\n";
  myString += "<Log>\n";
  myString +=   mModelLog;
  myString += "</Log>\n";
  myString += "<Projections>\n";
  QListIterator<OmgProjection *> myProjectionIterator(mProjectionList);
  while (myProjectionIterator.hasNext())
  {
    OmgProjection * mypProjection = myProjectionIterator.next();
    myString += mypProjection->toXml();
  }
  myString += "</Projections>\n";
  myString += "</Model>\n";
  return myString;
}

QString OmgModel::toModelCreationXml() const 
{
  //QString myString("<ModelParameters xmlns=\"http://openmodeller.cria.org.br/xml/1.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://openmodeller.cria.org.br/xml/1.0 http://openmodeller.cria.org.br/xml/1.0/openModeller.xsd\">\n");
  //
  // Options
  //
  QString myString = "<Options>\n";
  myString += "  <OccurrencesFilter>\n";
  if (mLocalitiesFilterType==NO_FILTER)
  {
    //add nothing
  }
  else if(mLocalitiesFilterType==SPATIALLY_UNIQUE)
  {
    myString += "      <SpatiallyUnique/>\n";
  }
  else //environmentally unique
  {
    myString += "      <EnvironmentallyUnique/>\n";
  }
  myString += "    </OccurrencesFilter>\n";
  myString += "  </Options>\n";

  //
  // Sampler
  // 

  myString += "  <Sampler>\n";
  bool myIncludeExtraNodes = false;
  myString+=mCreationLayerSet.toXml(myIncludeExtraNodes);
  
  //
  // Presences
  //
  
  myString += QString("    <Presence Label=\""+taxonName()+"\">\n");
  myString += QString("      <CoordinateSystem>\n");
  myString += QString("         "+mCoordinateSystem+"\n");
  myString += QString("      </CoordinateSystem>\n");
  //loop through all the localities
  bool myHasAbsencesFlag=false;
  for (int i=0;i<mLocalities.size();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if ( myLocality.abundance() > 0 )
    {
      myString+=myLocality.toXml();
    }
    else
    {
      myHasAbsencesFlag=true;
    }
  }
  myString += QString("      </Presence>\n");
  
  //
  // Absences
  //
  
  if (myHasAbsencesFlag)
  {
    myString += QString("    <Absence Label=\""+taxonName()+"\">\n");
    myString += QString("      <CoordinateSystem>\n");
    myString += QString("         "+mCoordinateSystem+"\n");
    myString += QString("      </CoordinateSystem>\n");
    //loop through all the localities
    for (int i=0;i<mLocalities.count();i++)
    {
      OmgLocality myLocality = mLocalities.at(i);
      if (myLocality.abundance() <= 0)
      {
        myString+=myLocality.toXml();
      }
    }
    myString += QString("      </Absence>\n");
  }
  //
  // finish up
  //
  
  myString += QString("  </Sampler>\n");
  myString+=mAlgorithm.toModelCreationXml();
  //myString += QString("</ModelParameters>\n");
  return myString;
}

QString OmgModel::toSerializedModelXml() const 
{
  QString myString("<SerializedModel>\n");

  //
  // Sampler
  // 

  myString += "  <Sampler>\n";
  bool myIncludeExtraNodes = false;
  myString+=mCreationLayerSet.toXml(myIncludeExtraNodes);
  
  //
  // Presences
  //
  
  myString += QString("    <Presence Label=\""+taxonName()+"\">\n");
  myString += QString("      <CoordinateSystem>\n");
  myString += QString("         "+mCoordinateSystem+"\n");
  myString += QString("      </CoordinateSystem>\n");
  //loop through all the localities
  bool myHasAbsencesFlag=false;
  for (int i=0;i<mLocalities.size();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if ( myLocality.abundance() > 0 )
    {
      myString+=myLocality.toXml();
    }
    else
    {
      myHasAbsencesFlag=true;
    }
  }
  myString += QString("      </Presence>\n");
  
  //
  // Absences
  //
  
  if (myHasAbsencesFlag)
  {
    myString += QString("    <Absence Label=\""+taxonName()+"\">\n");
    myString += QString("      <CoordinateSystem>\n");
    myString += QString("         "+mCoordinateSystem+"\n");
    myString += QString("      </CoordinateSystem>\n");
    //loop through all the localities
    for (int i=0;i<mLocalities.count();i++)
    {
      OmgLocality myLocality = mLocalities.at(i);
      if (myLocality.abundance() <= 0)
      {
        myString += myLocality.toXml();
      }
    }
    myString += QString("      </Absence>\n");
  }
  //
  // finish up
  //
  
  myString += QString("  </Sampler>\n");
  myString += mAlgorithm.toSerializedModelXml(normalizationDefinition().trimmed(), modelDefinition().trimmed());
  myString += QString("</SerializedModel>\n");
  return myString;
}

QString OmgModel::toHtml(bool theForceFlag, bool theForceGraphRenderFlag) const
{
  //make sure the working dir for the experiment exists
  QDir myWorkDir(mWorkDir);
  //QString.replace() is destructive so make a copy first
  QString myTaxonName = taxonName();
  myTaxonName.replace(" ","_");
  QString myOutputFileName(mWorkDir + "/" + myTaxonName + "_report.html");
  if (!myWorkDir.exists())
  {
    qDebug(QObject::tr("Working directory does not exist...html not saved"));
    return "";
  }
  else //save it out to file
  {
    qDebug(QObject::tr("Saving experiment report as html"));
    if (QFile::exists(myOutputFileName) && !theForceFlag)
    return myOutputFileName;
  }

  QString myAlgorithmParameters = mAlgorithm.toHtml();


  //
  // Main report starts here
  //
  QString myString;
  myString+=Omgui::getHtmlHeader();
  myString += QString("  <br/>\n");
  myString += QString("  <h1>" + QObject::tr("Taxon: ") + taxonName() + "</h1>\n");
  myString += QString("  <center><table border=\"0\">\n");
  myString += QString("  <tr>\n");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Algorithm:") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+mAlgorithm.name()+"</td>\n");
  myString += QString("  </tr>\n\n");  
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Started: ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+startDateTimeStamp().toString()+"</td>\n");
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Completed: ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+endDateTimeStamp().toString()+"</td>\n");
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Processing time: ") + "</td>\n");
  
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

  //
  // Occurrence filtering options
  //
  myString +="  <tr>\n\n";
  myString +="    <td class=\"headerCell\"> " + 
      QObject::tr("Occurrence filtering: ") + "</td>\n";
  myString +="    <td class=\"largeCell\">";
  if (mLocalitiesFilterType==NO_FILTER)
  {
    myString += QObject::tr("Unfiltered");
  }
  else if(mLocalitiesFilterType==SPATIALLY_UNIQUE)
  {
    myString += QObject::tr("Spatially Unique");
  }
  else //environmentally unique
  {
    myString += QObject::tr("Environmentally Unique");
  }
  myString += "   </td>\n";
  myString +="  </tr>\n\n";
  myString += QString("  <tr>\n");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Model GUID:") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+guid()+"</td>\n");
  myString += QString("  </tr>\n\n");  
  myString += QString("  </table></center>\n");

  //
  // Ok thats the summary header taken care of, now come the various details...
  // ..... first the roc table......
  // 
  
  // First the summary roc table
#if WITH_QWT
  if ( theForceGraphRenderFlag )
  {
    createSummaryQwtRocGraph(); 
    QString myFileName = myTaxonName + "summary_roc_curve.png"; //relative path
    myString += "<h3 align=\"center\" class=\"glossyBlue\"><a name=\"auc_summary\"></a>" + 
      QObject::tr("Summary AUC Chart:") + "</h3>\n";
    myString += "<center><img src=\"" + myFileName + "\"></center>\n";
  }
#endif
  // Now make a summary table for the tests
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  myString += QString("  <center><table border=\"0\">\n");
  myString += QString("  <tr>\n");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Test") + "</td>\n");
  myString += QString("    <td class=\"headerCell\">"+ QObject::tr("Accuracy") + "</td>\n");
  myString += QString("    <td class=\"headerCell\">"+ QObject::tr("Roc Score") + "</td>\n");
  myString += QString("    <td class=\"headerCell\">"+ QObject::tr("Kappa") + "</td>\n");
  myString += QString("  </tr>\n\n");  
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();

    float myKappa = mypModelTest->calculateKappa();
    QString myKappaStr = (myKappa < 0.0) ? "-": QString::number(myKappa);

    myString += QString("  <tr>\n");
    myString += QString("    <td> %1 </td>\n").arg(mypModelTest->label());
    myString += QString("    <td> %1 </td>\n").arg(mypModelTest->accuracy());
    myString += QString("    <td> %1 </td>\n").arg(mypModelTest->rocScore());
    myString += QString("    <td> %1 </td>\n").arg(myKappaStr);
    myString += QString("  </tr>\n\n");  
  }
  myString += QString("  </table></center>\n");
  // Then iterate the individual tests for complete report on each
  myTestIterator.toFront();
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    mypModelTest->createRocGraph(workDir(), taxonName());
    myString += mypModelTest->toHtml();
  }

  //
  // Projection summaries go here...
  //
  myString += QString("  <h3 align=\"center\" class=\"glossyBlue\">" + QObject::tr("Projection(s) Summary:") + "</h3>\n");
  if (mProjectionList.size())
  {
    QListIterator<OmgProjection *> myIterator(mProjectionList);
    while (myIterator.hasNext())
    {
      OmgProjection * mypProjection = myIterator.next();
      myString += mypProjection->toSummaryHtml(); 
      myString += "\n<br>\n";
    }
  }
  else
  {
    myString += QObject::tr("<center>No projections requested</center>") + "\n";
  }
  //
  // Algorithm paramters and layer details etc next.....
  //
  myString += QString("  <a name=\"algparameters\"></a>\n");  
  myString += QString("  <p>"+myAlgorithmParameters+"</p>\n");
  myString += QString("  <h3 align=\"center\" class=\"glossyBlue\">" + QObject::tr("Model Creation Layers:") + "</h3>\n");  
  myString+=mCreationLayerSet.toHtml(); 
  myString += QString("  <h3 align=\"center\" class=\"glossyBlue\">" + QObject::tr("Model Log") + "</h3>\n");
  myString += QString("<pre>" + mModelLog + "</pre>");
  myString+=Omgui::getHtmlFooter();
  Omgui::createTextFile(myOutputFileName , myString);
  return myOutputFileName;
}

QString OmgModel::toPrintHtml(bool theForceFlag) const
{
  //QString.replace() is destructive so make a copy first
  QString myTaxonName = taxonName();
  myTaxonName.replace(" ","_");
  QString myAlgorithmParameters = mAlgorithm.toHtml();
  //
  // Main report starts here
  //
  QString myString;
  myString += QString("  <br/>\n");
  myString += QString("  <h3 align=\"center\" class=\"glossyBlue\">" + taxonName() + " - " + mAlgorithm.name() + "</h3>\n");
  myString += QString("  <table width=\"100%\">\n");
  myString += QString("  <tr>\n");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Taxon :") + "</td>\n");
  myString += QString("    <td class=\"largeCell\"><i>"+taxonName()+"</i></td>\n");
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\n");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Algorithm :") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+mAlgorithm.name()+"</td>\n");
  myString += QString("  </tr>\n\n");  
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Started: ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+startDateTimeStamp().toString()+"</td>\n");
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Completed : ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+endDateTimeStamp().toString()+"</td>\n");
  myString += QString("  </tr>\n\n");
  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Processing time : ") + "</td>\n");

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

  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Average Area Under Curve (AUC) : ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">"+ QString::number( averageRocScore(),'f',2 )
      + "</td>\n");
  myString += QString("  </tr>\n\n");

  myString += QString("  <tr>\b");
  myString += QString("    <td class=\"headerCell\"> " + QObject::tr("Average Accuracy : ") + "</td>\n");
  myString += QString("    <td class=\"largeCell\">" + QString::number( averageAccuracy() ) + "%"
      + QObject::tr(" (using 50% threshold)")
      + "</td>\n");
  myString += QString("  </tr>\n\n");
  myString +=QString("  <tr>\b");
  myString +=QString("    <td class=\"headerCell\"> " + QObject::tr("Model Creation Layerset : ") + "</td>\n");
  myString +=QString("    <td class=\"largeCell\">" + mCreationLayerSet.name() + "</td>\n");
  myString +=QString("  </tr>\n\n");
  myString +=QString("  <tr>\b");
  myString +=QString("  </table>\n");
  myString +=QString("  <h3 align=\"center\" class=\"glossyBlue\">" + QObject::tr("Output statistics:") + "</h3>\n");
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    myString += mypModelTest->toHtml();
  }
  return myString;
}

QString OmgModel::localitiesToCsv(QString theDelimiter, bool theSuppressHeaderFlag) const
{
  QString myString = "#Num" + theDelimiter + "Lon" + theDelimiter + "Lat" + theDelimiter + "Taxon";
  QStringListIterator myIterator(creationLayerNames());
  while (myIterator.hasNext())
  {       
    QFileInfo myFileInfo( myIterator.next() );
    myString += QString( theDelimiter + myFileInfo.baseName() );
  }
  for (int myRow = 0; myRow < localities().size(); ++myRow) 
  {
    myString+="\n";
    OmgLocality myLocality = localities().at(myRow);
    myString += myLocality.id() + theDelimiter;
    myString += QString::number(myLocality.longitude()) + theDelimiter;
    myString += QString::number(myLocality.latitude()) + theDelimiter;
    myString += taxonName().replace(","," ");
    for (int myCol = 0; myCol < myLocality.samples().size(); ++myCol) 
    {
      double mySample = myLocality.samples().at(myCol);
      myString += theDelimiter + QString::number(mySample);
    }
  }
  //myString+="\r\n";
  return myString;
}

bool OmgModel::toShapefile(QString theShapefileName) const
{
  return Omgui::localitiesToShapefile(theShapefileName,mLocalities);
}

//
// mutators
// 

void OmgModel::setModellerPlugin(OmgModellerPluginInterface * thePlugin)
{
  mpModellerPlugin = thePlugin;
}

void OmgModel::setTestCompartmentCount(int theCount)
{
  mTestCompartmentCount = theCount;
}

void OmgModel::setLocalities(OmgLocalities theLocalities)
{
   mLocalities = theLocalities; 
}

void OmgModel::setCoordinateSystem(const QString theCoordinateSystem)
{
  mCoordinateSystem = theCoordinateSystem;
}
void OmgModel::setCoordinateSystemName (const QString theName)
{
  mCoordinateSystemName = theName;
}

void OmgModel::setSpeciesFile(const QString theSpeciesFile)
{
  mSpeciesFile = theSpeciesFile;
}
void OmgModel::setShapefileName(const QString theShapefileName)
{
  QFileInfo myFileInfo(theShapefileName);
  mShapefileName = myFileInfo.fileName();
}
void OmgModel::setCsvFileName(const QString theCsvFileName)
{
  QFileInfo myFileInfo(theCsvFileName);
  mCsvFileName = myFileInfo.fileName();
}
void OmgModel::setLocalitiesFilterType(LocalitiesFilterType theFilterType)
{
  mLocalitiesFilterType = theFilterType;
}
OmgModel::LocalitiesFilterType OmgModel::localitiesFilterType() const
{
  return mLocalitiesFilterType;
}

void OmgModel::setTaxonName(QString theName)
{
  mTaxonName = theName;
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->setTaxonName(theName);
  }
}

void OmgModel::setCreationLayerSet(OmgLayerSet theLayerSet)
{
  mCreationLayerSet=theLayerSet;
}
void OmgModel::setAlgorithm(OmgAlgorithm theAlgorithm)
{
  //qDebug("setAlgorithm" + theAlgorithm.name().toLocal8Bit());
  mAlgorithm = theAlgorithm;
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->setAlgorithm(theAlgorithm);
  }
}
void OmgModel::setError(bool theFlag)
{
  mErrorFlag=theFlag;
}
void OmgModel::setCompleted(bool theFlag)
{
  mCompletedFlag=theFlag;
}
void OmgModel::setStartDateTimeStamp()
{
  mStartDateTimeStamp = QDateTime::currentDateTime();
}
void OmgModel::setStartDateTimeStamp(QDateTime theDateTimeStamp)
{
  mStartDateTimeStamp = theDateTimeStamp; 
}
void OmgModel::setEndDateTimeStamp()
{
  mEndDateTimeStamp = QDateTime::currentDateTime();
}
void OmgModel::setEndDateTimeStamp(QDateTime theDateTimeStamp)
{
  mEndDateTimeStamp = theDateTimeStamp; 
}

void OmgModel::setWorkDir(QString theDir)
{
  //ensure we have a dir and an ending slash
  QFileInfo myFileInfo(theDir);
  mWorkDir=myFileInfo.absoluteFilePath() + QDir::separator();
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->setWorkDir(theDir);
  }
}

void OmgModel::setModelDefinition(QString theDefinition)
{
  mModelDefinition = theDefinition;
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mypProjection->setModelDefinition(theDefinition);
  }
}

void OmgModel::setNormalizationDefinition(QString theDefinition)
{
  mNormalizationDefinition = theDefinition;
}
void OmgModel::appendToLog(QString theMessage)
{
  mModelLog += theMessage + "\n";
}

void OmgModel::setLog(QString theLog)
{
  mModelLog = theLog;
}
//accessors

const QList<OmgProjection *> OmgModel::projectionList() const
{
  return mProjectionList;
}

const QList<OmgModelTest *> OmgModel::getModelTests() const
{
  return mModelTests;
}

int OmgModel::testCompartmentCount()
{
  return mTestCompartmentCount;
}

OmgLocalities OmgModel::localities() const
{
  return mLocalities;
}

QString OmgModel::coordinateSystem() const
{
  return mCoordinateSystem;
}

QString OmgModel::coordinateSystemName() const
{
  return mCoordinateSystemName;
}

QString OmgModel::speciesFile() const
{
  return mSpeciesFile;
}

QString OmgModel::shapefileName() const
{
  QFileInfo myFileInfo(mShapefileName);
  QString myFileName=myFileInfo.fileName();
  return myFileName;
}

QString OmgModel::csvFileName() const
{
  return mCsvFileName;
}

QString OmgModel::taxonName() const
{
  return mTaxonName;
}

OmgLayerSet OmgModel::creationLayerSet() const
{
  return mCreationLayerSet; 
}
QString OmgModel::creationMaskLayerName() const
{
  return mCreationLayerSet.maskName();
}
QStringList OmgModel::creationLayerNames() const
{
  return mCreationLayerSet.nameList();
}
OmgAlgorithm OmgModel::algorithm() const
{
  return mAlgorithm;
}
bool OmgModel::hasError() const
{
  return mErrorFlag;
}
bool OmgModel::isCompleted() const
{
  return mCompletedFlag;
}
QDateTime OmgModel::startDateTimeStamp() const
{
  return mStartDateTimeStamp;
}
QDateTime OmgModel::endDateTimeStamp() const
{
  return mEndDateTimeStamp;
}
QString OmgModel::workDir() const
{
  return QDir::toNativeSeparators(mWorkDir);
}
double OmgModel::averageRocScore() const
{
  //loop through the tests and return the average
  int myTestCount = mModelTests.count();
  double mySum = 0;
  if (0 >= myTestCount) return -1;
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    mySum += mypModelTest->rocScore();
  }
  if (0==mySum)
  {
    return mySum;
  }
  else
  {
    return mySum / myTestCount;
  }
}
double OmgModel::averageAccuracy() const
{
  //loop through the tests and return the average
  int myTestCount = mModelTests.count();
  double mySum = 0;
  if (0 >= myTestCount) return -1;
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    mySum += mypModelTest->rocScore();
  }
  if (0==mySum)
  {
    return mySum;
  }
  else
  {
    return mySum / myTestCount;
  }
}

QString OmgModel::modelDefinition() const
{
  return mModelDefinition;
}

QString OmgModel::normalizationDefinition() const
{
  return mNormalizationDefinition;
}

QString OmgModel::modelLog() const
{
  return mModelLog;
}


bool OmgModel::parseModelResult(QString theModelXml)
{
  //qDebug("******** Parsing model result ***********");
  //Omgui::createTextFile("/tmp/model.xml",theModelXml);
  QDomDocument myDocument("mydocument");
  myDocument.setContent(theModelXml);
  QDomElement myTopElement = myDocument.firstChildElement("SerializedModel");
  if (myTopElement.isNull())
  {
    //qDebug("Top (SerializedModel) element could not be found!");
    return false;
  }
  QDomElement mySamplerElement = myTopElement.firstChildElement("Sampler");
  if (mySamplerElement.isNull())
  {
    //qDebug("Sampler element could not be found!");
    return false;
  }
  
  //
  // Parse for presence records
  //
  OmgLocalities myLocalities; //used by absence parser too...
  QDomElement myPresenceElement = mySamplerElement.firstChildElement("Presence");
  if (!myPresenceElement.isNull())
  {
    QDomElement myElement = myPresenceElement.firstChildElement();
    while(!myElement.isNull()) 
    {
      if (myElement.tagName()!="Point")
      {
        myElement = myElement.nextSiblingElement();
        continue;
      }
      //qDebug("Parser found a point");
      OmgLocality myLocality;
      QString myId=myElement.attribute("Id");
      myLocality.setId(myId);
      QString myX=myElement.attribute("X");
      myLocality.setLongitude(myX.toDouble());
      QString myY=myElement.attribute("Y");
      myLocality.setLatitude(myY.toDouble());
      QString mySample=myElement.attribute("Sample");
      //qDebug("Sample for point: " + mySample.toLocal8Bit());
      QStringList myList=mySample.split(" ");
      QStringListIterator myIterator(myList);
      OmgSampleVector mySamples;
      while (myIterator.hasNext())
      {
        double myDouble=myIterator.next().toDouble();
        mySamples.push_back(myDouble);
      }
      myLocality.setSamples(mySamples);
      myLocalities.push_back(myLocality);
      myElement = myElement.nextSiblingElement();
    }
  }
  //
  // Parse for absences
  //
  QDomElement myAbsenceElement = mySamplerElement.firstChildElement("Absence");
  if (!myAbsenceElement.isNull())
  {
    QDomElement myElement = myAbsenceElement.firstChildElement();
    while(!myElement.isNull()) 
    {
      if (myElement.tagName()!="Point")
      {
        myElement = myElement.nextSiblingElement();
        continue;
      }
      //qDebug("Parser found a point");
      OmgLocality myLocality;
      myLocality.setAbundance(0);
      QString myId=myElement.attribute("Id");
      myLocality.setId(myId);
      QString myX=myElement.attribute("X");
      myLocality.setLongitude(myX.toDouble());
      QString myY=myElement.attribute("Y");
      myLocality.setLatitude(myY.toDouble());
      QString mySample=myElement.attribute("Sample");
      //qDebug("Sample for point: " + mySample.toLocal8Bit());
      QStringList myList=mySample.split(" ");
      QStringListIterator myIterator(myList);
      OmgSampleVector mySamples;
      while (myIterator.hasNext())
      {
        double myDouble=myIterator.next().toDouble();
        mySamples.push_back(myDouble);
      }
      myLocality.setSamples(mySamples);
      myLocalities.push_back(myLocality);
      myElement = myElement.nextSiblingElement();
    }
  }
  setLocalities(myLocalities);
  //
  // Extract the serialised model tag if present
  //
  QDomElement myAlgorithmElement = myTopElement.firstChildElement("Algorithm");
  QDomElement mySerialisedModelElement = myAlgorithmElement.firstChildElement("Model");
  if (!mySerialisedModelElement.isNull())
  {
    QDomDocument myModelDoc("MyModel");
    myModelDoc.appendChild(mySerialisedModelElement);
    //set the (optional) normalisation def after stripping off the doctype tag
    setModelDefinition(myModelDoc.toString().remove("<!DOCTYPE MyModel>"));
  }

  //
  // Extract the serialised normalisation parameters if present
  //
  QDomElement mySerialisedNormalizationElement = myAlgorithmElement.firstChildElement("Normalization");
  if (mySerialisedNormalizationElement.isNull())
  {
    qDebug("Normalisation element could not be found!");
    //do nothing (its optional
  }
  else
  {
    QDomDocument myModelDoc("MyNormalization");
    myModelDoc.appendChild(mySerialisedNormalizationElement);
    //set the mode def after stripping off the doctype tag
    setNormalizationDefinition(myModelDoc.toString().remove("<!DOCTYPE MyNormalization>"));
  }
  return true;
}

bool OmgModel::postProcess( bool theForceGraphRenderFlag )
{
  appendToLog( tr("Processing model results for ") + taxonName() );
  //
  // Create a csv file of the samples
  //
  QString myFileName = taxonName();
  myFileName = myFileName.replace(" ","_") + ".csv";
  Omgui::createTextFile(workDir() + QDir::separator() + myFileName , localitiesToCsv());
  setCsvFileName(myFileName);
  appendToLog(tr("Model post processing successful"));           
  toHtml(true, theForceGraphRenderFlag );
  return true;
}

//
// Run the model!
//

void OmgModel::run()
{
  qDebug("Running model: " + guid().toLocal8Bit());
  //if the experiment is being resumed dont bother to
  //rerun completed models.
  //@TODO allow user to force rerun...
  if (isCompleted())
  {
    mModelLog += tr("Model for ") + taxonName() + tr(" already exists, skipping to next job.") + "\n";
    return;
  }
  OmgAlgorithm myAlgorithm = algorithm();
  if (!QFile::exists(mWorkDir))
  {
    //emit logMessage(tr("Experiment working directory does not exist...creating\n"));
    QDir myDir(mWorkDir);
    if (!myDir.mkdir(mWorkDir))
    {
      //emit logMessage(tr("Model failed to create working directory\n") + mWorkDir );
      return;
    }
    else
    {
      //emit logMessage(tr("Experiment working directory created\n") + mWorkDir );
    }
  }
  //under the working dir we store each model output in a subdir based on the alg being used
  QString mySubDirName = myAlgorithm.name();
  mySubDirName.replace(" ","_");
  QDir myDir(mWorkDir + mySubDirName);
  if (!myDir.exists())
  {
    mModelLog += tr("Algorithm working directory does not exist...creating") + "\n";
    mModelLog += mWorkDir + mySubDirName + "\n";
    QDir mySubDir(mWorkDir);
    if (!mySubDir.mkdir(mySubDirName))
    {
      mModelLog += tr("Experiment failed to create working directory\n") + mWorkDir + mySubDirName + "\n";
      return;
    }
  }
  setWorkDir(mWorkDir + mySubDirName + QDir::separator());
  //set the start time
  setStartDateTimeStamp();
  mModelLog += tr("Creating model definition for ") + taxonName() + "\n";

  QString myModelXml = mpModellerPlugin->createModel(this);

  setEndDateTimeStamp();
  mModelLog += tr("Model has completed:") + "\n";
  setCompleted(true);
  if (hasError())
  {
    mModelLog += tr("But has an error!") + "\n";
    mModelLog += tr("Model failed to complete the model creation stage") + "\n";
    // create a shapefile of the localities
    // we do this even in failure case so the 
    // user can still view
    // a map of the localities and perhaps try to 
    // discern why a model may have failed etc
    QString myShapefileName = taxonName().replace(" ","_");
    if (toShapefile(workDir() + myShapefileName + "failedModel.shp"))
    {
      mModelLog += tr("Creating shapefile as: ") + 
        QString(myShapefileName + ".shp").toLocal8Bit() + "\n";
      setShapefileName(myShapefileName + ".shp");
    }
    // mark projections as having error too
    QListIterator<OmgProjection *> myIterator(mProjectionList);
    while (myIterator.hasNext())
    {
      OmgProjection * mypProjection = myIterator.next();
      mypProjection->appendToLog(tr("Projection not run because model not built"));
      mypProjection->setError(true);
    }
    return;
  }

  mModelLog += tr("Without errors") + "\n";
  mModelLog += tr("Parsing model") + "\n";
  //store the whole model definition in the model object
  // parse the model xml that came back and add the samples and the
  // serialised model definition
  parseModelResult(myModelXml);
  mModelLog += tr("Model parsed") + "\n";
  //
  // Write the shapefile based on the parsed results
  //
  QString myShapefileName = taxonName().replace(" ","_");
  if (toShapefile(workDir() + myShapefileName + ".shp"))
  {
    mModelLog += tr("Creating shapefile as: ") + 
    QString(myShapefileName + ".shp").toLocal8Bit() + "\n";
    setShapefileName(myShapefileName + ".shp");
  }

  //
  // Test the model
  //
  double myThreshold = -1.0;

  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypTest = myTestIterator.next();

    if (myThreshold > 0.0 && !mypTest->isInternal())
    {
      // this means that there was a previous internal test and the user
      // requested to use the lowest training threshold
      mypTest->setThreshold(myThreshold);
    }

    runTest(mypTest);

    if (mypTest->isInternal() && mypTest->useLowestThreshold() && 
        mypTest->isCompleted() && ! mypTest->hasError())
    {
      // Store lowest training threshold for subsequent external tests
      myThreshold = mypTest->threshold();
    }
  }

  //
  // Project the model
  //
  QListIterator<OmgProjection *> myIterator(mProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();
    mModelLog += tr("Projecting model for ") + taxonName() + " into scenario " 
      + mypProjection->layerSet().name()  + "\n";
    mypProjection->setModelDefinition(modelDefinition());
    mypProjection->setNormalizationDefinition(normalizationDefinition());
    mypProjection->setWorkDir(workDir());
    mypProjection->setTaxonName(taxonName());
    mypProjection->setAlgorithm(algorithm());
    mypProjection->setStartDateTimeStamp();

    mpModellerPlugin->projectModel(mypProjection);

    mypProjection->setEndDateTimeStamp();

    //
    // Handle projection success / failure appropriately
    //
    mModelLog += tr("Projection completed for ") + taxonName() + " into scenario " 
      + mypProjection->layerSet().name()  + "\n";
    mypProjection->setCompleted(true);
    if (mypProjection->hasError())
    {
      mModelLog += tr("Projection had error!") + "\n";
      mModelLog += tr("Failed to complete the model projection stage") + "\n";
    }
    else
    {
      mModelLog += tr("Model projection completed! Post processing...") + "\n";
      mypProjection->postProcess();
    }
    postProcess(); //the model
  }
}

void OmgModel::addProjection(OmgProjection * thepProjection)
{
  thepProjection->setModelGuid(guid());
  mProjectionList << thepProjection;
}

int OmgModel::presenceCount() const
{
  int myPresenceCount=0;
  //return count of presence samples
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if (0 < myLocality.abundance()) ++myPresenceCount;
  }
  return myPresenceCount;
}
int OmgModel::usablePresenceCount() const
{
  int myPresenceCount=0;
  //return count of presence samples
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if (0 < myLocality.abundance() && 0 < myLocality.samples().size()) ++myPresenceCount;
  }
  return myPresenceCount;
}
int OmgModel::absenceCount() const
{
  int myAbsenceCount=0;
  //return count of presence samples
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if (0==myLocality.abundance()) ++myAbsenceCount;
  }
  return myAbsenceCount;
}
int OmgModel::usableAbsenceCount() const
{
  int myAbsenceCount=0;
  //return count of presence samples
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if (0==myLocality.abundance() && 0 < myLocality.samples().size()) ++myAbsenceCount;
  }
  return myAbsenceCount;
}

void OmgModel::clearTests()
{
  mModelTests.clear();
}
void OmgModel::addTest(OmgModelTest * thepTest)
{
  mModelTests << thepTest;
}

/** Set and Get TrainingProportion. */
void
OmgModel::setTrainingProportion( float proportion )
{
  mTrainingProportion = proportion;
}
float 
OmgModel::getTrainingProportion()
{
  return mTrainingProportion;
}

#if WITH_QWT
void OmgModel::createSummaryQwtRocGraph() const 
{
  qDebug("createQwtRocGraph start");
  QwtPlot * mypPlot = new QwtPlot();
  mypPlot->setTitle(QObject::tr("Sensitivity vs. 1 - Specificity for %1")
      .arg(taxonName()));
  mypPlot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
  // Set axis titles
  mypPlot->setAxisTitle(QwtPlot::xBottom, 
      QObject::tr("1 - Specificity (Fractional Predicted Area)"));
  mypPlot->setAxisTitle(QwtPlot::yLeft, 
      QObject::tr("Sensitivity (1 - Omission Rate)"));
  //
  // add a grid
  //
  QwtPlotGrid * myGrid = new QwtPlotGrid();
  //QwtScaleDiv myScaleDiv;
  //myScaleDiv.setInterval(0.1);
  //myGrid->setXDiv(myScaleDiv);
  myGrid->attach(mypPlot);
  QListIterator<OmgModelTest *> myTestIterator(mModelTests);
  while (myTestIterator.hasNext())
  {
    OmgModelTest * mypModelTest = myTestIterator.next();
    mypModelTest->getRocCurve()->attach(mypPlot);
  }
  //
  // now make a AUC=0.5 line too
  //
  /*
  QwtPlotCurve * mypRocCurve2 = new QwtPlotCurve(QObject::tr("Random prediction (AUC=0.5)"));
  mypRocCurve2->setRenderHint(QwtPlotItem::RenderAntialiased);
  mypRocCurve2->setPen(QPen(Qt::blue));
  QwtArray<double> myX2Data;//qwtarray is just a wrapped qvector
  QwtArray<double> myY2Data;//qwtarray is just a wrapped qvector
  for (int myCounter = 0; myCounter < mRocPoints.size(); ++ myCounter)
  {
    double myValue = 0;
    if (myCounter > 0)
    {
      myValue = 1/myCounter;
    }
    myX2Data.append(myValue);
    myY2Data.append(myValue);
  }
  mypRocCurve2->setData(myX2Data,myY2Data);
  mypRocCurve2->attach(mypPlot);
  */
  //
  // Finish up by screen grabbing the widget
  //
  mypPlot->resize(QSize(600,600));
  mypPlot->setCanvasBackground(Qt::white);
  mypPlot->setCanvasBackground(Qt::white);
  mypPlot->replot();
  QPixmap myPixmap = QPixmap::grabWidget(mypPlot);
  QString myTaxonName = taxonName();
  myTaxonName = myTaxonName.replace(" ","_");
  QString myFileName = workDir() + QDir::separator() + myTaxonName + "summary_roc_curve.png";
  myPixmap.save(myFileName); 
  //mypPlot->show(); //for debugging only
  //ensure all children get removed
  mypPlot->setAutoDelete(true);
  delete mypPlot;
  qDebug("createQwtRocGraph done");
}

#endif //QWT


void OmgModel::runTest( OmgModelTest * thepTest )
{
  //
  // Test the model
  //
  OmgModelTest * mypTest = thepTest;

  QString testShapefileName;
  QString myShapefileName = mTaxonName;
  myShapefileName.replace(" ","_");

  if (mypTest->isInternal())
  {
    mModelLog += tr("Running internal test for ");
  }
  else
  {
    mModelLog += tr("Running external test for ");
	
    //testShapefileName = workDir() + myShapefileName + "_external_test.shp";
    testShapefileName = workDir() + myShapefileName + "_external_test_" + QString::number(mModelTests.size()) + ".shp";
    //
    // Write the shapefile
    //
    if ( Omgui::localitiesToShapefile( testShapefileName, mypTest->getLocalities() ) )
    {
      mModelLog += tr("Creating shapefile as: ") + 
      testShapefileName.toLocal8Bit() + "\n";
      mypTest->setShapefileName(testShapefileName);
    }
  }

  mModelLog += taxonName() + "\n";

  mypTest->setStartDateTimeStamp();

  mpModellerPlugin->testModel(this, mypTest);

  mypTest->setEndDateTimeStamp();

  //
  // Handle test success / failure appropriately
  //
  mModelLog += tr("Test completed for ") + taxonName() + "\n";
  mypTest->setCompleted(true);

  if (mypTest->hasError())
  {
    mModelLog += tr("Model test had error!") + "\n";
    mModelLog += tr("Failed to complete the model test stage") + "\n";
  }
}

