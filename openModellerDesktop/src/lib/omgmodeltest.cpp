/***************************************************************************
 *   Copyright (C) 2008 by Tim Sutton                                      *
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
#include "omgmodeltest.h"
#include "omgui.h"
#include "omggraph.h"
#include "omgmodel.h"

#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QDomElement>
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


OmgModelTest::OmgModelTest(const OmgModel * const thepModel) : OmgSerialisable(), OmgGuid(),
  mpModel(thepModel),
  mCompletedFlag(false),
  mInternalFlag(false),
  mErrorFlag(false),
  mLabel(QObject::tr("Statistics")),
  mLog(""),
  mShapefileName(""),
  mUseLowestThreshold(false),
  mThreshold(0.5),
  mOmission(0.0),
  mCommission(0.0),
  mTruePositiveCount(0),
  mFalsePositiveCount(0),
  mFalseNegativeCount(0),
  mTrueNegativeCount(0),
  mRocScore(0.0),
  mMaxOmission(-1.0),
  mAreaRatio(-1.0),
  mNumBackgroundPoints(10000),
  mAccuracy(0.0)
{
  
}

OmgModelTest::~OmgModelTest()
{
}

QString OmgModelTest::toString() const
{
  QString myString("Model Test:\n---------------------\n");
  //implement me
  return myString;
}

bool OmgModelTest::fromXml(const QString theXml) 
{
  QDomDocument myDocument("modeltest");
  myDocument.setContent(theXml);
  QDomElement myStatsElement = myDocument.firstChildElement("ModelTest");
  if (myStatsElement.isNull())
  {
    //TODO - just make this a warning
    //qDebug("top element could not be found!");
    return false;
  }
  setGuid(myStatsElement.attribute("Guid"));
  if (myStatsElement.hasAttribute("Label"))
  {
    setLabel(myStatsElement.attribute("Label"));
  }

  if (myStatsElement.hasAttribute("Internal"))
  {
    setInternal((myStatsElement.attribute("Internal")=="true") ? true : false);
  }

  if (myStatsElement.hasAttribute("UseLowestThreshold"))
  {
    setUseLowestThreshold((myStatsElement.attribute("UseLowestThreshold")=="true") ? true : false);
  }
  else
  {
    setUseLowestThreshold(false);
  }    

  if (myStatsElement.hasAttribute("Threshold"))
  {
    setThreshold(myStatsElement.attribute("Threshold").toDouble());
  }
  else
  {
    setThreshold(0.5);
  }    

  setRocScore(myStatsElement.attribute("RocScore").toDouble());
  setAccuracy(myStatsElement.attribute("Accuracy").toDouble());
  setOmission(myStatsElement.attribute("Omission").toDouble());//as %
  setCommission(myStatsElement.attribute("Commission").toDouble());//as %
  setTruePositiveCount(myStatsElement.attribute("TruePositiveCount").toInt()); //#of samples
  setFalsePositiveCount(myStatsElement.attribute("FalsePositiveCount").toInt()); //#of samples
  setFalseNegativeCount(myStatsElement.attribute("FalseNegativeCount").toInt()); //#of samples
  setTrueNegativeCount(myStatsElement.attribute("TrueNegativeCount").toInt()); //#of samples
  //loop through the roc points
  mRocPoints.clear();
  QDomElement myRocStatsElement = myStatsElement.firstChildElement("RocStats");
  if (myRocStatsElement.hasAttribute("NumBackgroundPoints"))
  {
    setNumBackgroundPoints(myRocStatsElement.attribute("NumBackgroundPoints").toInt());
  }
  if (myRocStatsElement.hasAttribute("MaxOmission"))
  {
    setMaxOmission(myRocStatsElement.attribute("MaxOmission").toDouble());
  }
  if (myRocStatsElement.hasAttribute("AreaRatio"))
  {
    setAreaRatio(myRocStatsElement.attribute("AreaRatio").toDouble());
  }
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
    addRocPoint(myPair);
    myRocPointElement = myRocPointElement.nextSiblingElement();
  }

  // @NOTE this code is a near duplication of code in omgexperiment::parsemodel()
  // and omgmodel::fromXml(). these implementations should be refactored into one function

  mLocalities.clear();
  QDomElement myLocalitiesElement = myRocStatsElement.nextSiblingElement();
  if((!myLocalitiesElement.isNull()) && myLocalitiesElement.tagName()=="Localities")
  {
    QDomElement myLocalityElement = myLocalitiesElement.firstChildElement();
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
  }

  mShapefileName =
    myStatsElement.firstChildElement("Localities").firstChildElement("ShapefileName").text();
  qDebug("mShapefileName: " + mShapefileName.toLocal8Bit() );

  return true;
}

QString OmgModelTest::toXml() const 
{
  QString myIsInternalStr = (mInternalFlag) ? "true" : "false";
  QString myUseLowestThresholdStr = (mUseLowestThreshold) ? "true" : "false";

  QString myString = 
  "<ModelTest "
    "Guid=\"" + guid() + "\" "
    "Label=\"" + mLabel + "\" "
    "Internal=\"" + myIsInternalStr + "\" " 
    "UseLowestThreshold=\"" + myUseLowestThresholdStr + "\" " 
    "Threshold=\"" + QString::number(mThreshold) + "\" " 
    "Omission=\"" + QString::number(mOmission) + "\" " 
    "Commission=\"" + QString::number(mCommission) + "\" "
    "RocScore=\"" + QString::number(mRocScore) + "\" "
    "Accuracy=\"" + QString::number(mAccuracy) + "\" "
    "TruePositiveCount=\"" + QString::number(truePositiveCount()) + "\" "
    "FalsePositiveCount=\"" + QString::number(falsePositiveCount()) + "\" "
    "FalseNegativeCount=\"" + QString::number(falseNegativeCount()) + "\" "
    "TrueNegativeCount=\"" + QString::number(trueNegativeCount()) + "\">\n"
  "  <RocStats NumBackgroundPoints=\""+ QString::number(mNumBackgroundPoints) +"\" "
              "MaxOmission=\""+ QString::number(mMaxOmission) +"\" "
              "AreaRatio=\""+ QString::number(mAreaRatio) +"\">\n";
  QListIterator< QPair <double,double> > myIterator (mRocPoints);
  while (myIterator.hasNext())
  {
    QPair<double,double> myPair = myIterator.next();
    myString += "  <RocPoint x=\"" + QString::number(myPair.first) + "\" " + 
                            "y=\"" + QString::number(myPair.second) + "\"/>\n";
  }
  myString += "  </RocStats>\n";
  myString += "  <Localities>\n";
  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    myString+=myLocality.toXml();
  }
  myString +=   "<ShapefileName>" + mShapefileName + "</ShapefileName>";
  myString += "  </Localities>\n";
  myString += "</ModelTest>\n";

  return myString;
}


QString OmgModelTest::toHtml(bool theForceFlag) const
{
  //Partial Roc 
  QString myPartialRocString = "";
  if (mMaxOmission >= 0.0 && mAreaRatio >= 0.0)
  {
    myPartialRocString += 
    "  <tr>\n\n"
    "    <td class=\"headerCell\"> " + 
    QObject::tr("Area Ratio for maximum omission of ") + 
    QString::number( mMaxOmission*100 ) + "%: </td>\n"
    "    <td class=\"largeCell\">" +
    QString::number( mAreaRatio,'f',2 ) + "</td>\n"
    "  </tr>\n\n";
  }
  // Background points for ROC
  QString myBackgroundPointsString = "";
  if (mNumBackgroundPoints > 0)
  {
    myBackgroundPointsString += 
    "  <tr>\n\n"
    "    <td class=\"headerCell\"> " + 
    QObject::tr("Number of Background Points") + ": </td>\n"
    "    <td class=\"largeCell\">" +
    QString::number( mNumBackgroundPoints ) + "</td>\n"
    "  </tr>\n\n";
  }
  // Roc table 
  QString myString =
    "<h3 align=\"center\" class=\"glossyBlue\">" + mLabel + "</h3>\n"
    //make a table of the roc curve
    "<center><table width=\"100%\">\n"
    "<tr><th align=\"center\" class=\"glossy\">" 
    + QObject::tr("ROC Curve") + "</th><tr>\n"
    "<tr><td><center><img src=\"" + guid() + "_roc_curve.png\"></center></td></tr>\n"
    "</table>\n"
    "<center><table>\n"
    "  <tr>\n\n"
    "    <td class=\"headerCell\"> " + 
    QObject::tr("Total Area Under Curve (AUC)") + ": </td>\n"
    "    <td class=\"largeCell\">" +
    QString::number( mRocScore,'f',2 ) + "</td>\n"
    "  </tr>\n\n" + myPartialRocString + "</table></center><br/>\n"
    "<table class=\"rocTable\">"; //qt 4.3 seems to ignore table css
  QString myCounterString = 
    "<tr>\n"
    "<th>" + QObject::tr("points") + "\n"
    "</th>\n"; //row will be completed in loop below
  QString myXString =
    "<tr>\n"
    "<th>X\n"
    "</th>\n"; //row will be completed in loop below
  QString myYString =
    "<tr>\n"
    "<th>Y\n"
    "</th>\n"; //row will be completed in loop below
  unsigned int myCounter = 1;
  bool myAlternateCell=false; //for highlighting every other row
  QListIterator< QPair <double,double> > myRocIterator (mRocPoints);
  while (myRocIterator.hasNext())
  {
    QPair<double,double> myPair = myRocIterator.next();
    myCounterString += "<td class=\"headerCell\">" + QString::number(myCounter) + "</td>\n";
    QString myTd;
    if (myAlternateCell)
    {
      myTd = "<td>"; 
      myAlternateCell=false;
    }
    else
    {
      myTd = "<td class=\"alternateCell\">"; 
      myAlternateCell=true;
    }
    myXString += myTd + QString::number(myPair.first,'f',3) + " </td>\n";
    myYString += myTd + QString::number(myPair.second,'f',3) + " </td>\n";

    ++myCounter;
  }
  double mySpecificity = calculateSpecificity();
  QString mySpecificityStr = (mySpecificity < 0.0) ? "-": QString::number(mySpecificity);
  double myCommission = calculateCommisionRate();
  QString myCommissionStr = (usableAbsenceCount() < 1.0) ? "-": QString::number(myCommission);
  double myPrevalence = calculatePrevalence();
  QString myPrevalenceStr = (usableAbsenceCount() < 1.0) ? "-": QString::number(myPrevalence);
  double myGlobalDiagnosticPower = calculateGlobalDiagnosticPower();
  QString myGlobalDiagnosticPowerStr = (usableAbsenceCount() < 1.0) ? "-": QString::number(myGlobalDiagnosticPower);
  myCounterString += "</tr>\n";
  myXString += "</tr>\n";
  myYString += "</tr>\n";
  myString += myCounterString +
    myXString +
    myYString +
    "</table></center>\n"
    "<br/>\n"
    // Now confusion matrix...
    "<center><table width=\"100%\">\n"
    "<tr>\n"
    "<th colspan=\"3\" class=\"glossy\"><center>" + 
    QObject::tr("Confusion Matrix") + "</center></th>\n"
    "</tr>\n"
    "<tr>\n"
    "<td></td>"
    "<th><center>" + QObject::tr("True Presence") + "</center></th>\n"
    "<th><center>" + QObject::tr("True Absence") + "</center></th>\n"
    "</tr>\n"
    "<tr>\n"
    "<th>" + QObject::tr("Predicted Presence") + "</th>\n"
    "<td border=\"1\"><center>" + QString::number(truePositiveCount()) + " (a)</center></td>\n"
    "<td border=\"1\"><center>" + QString::number(falsePositiveCount()) + " (b)</center></td>\n"
    "</tr>\n"
    "<tr>\n"
    "<th>" + QObject::tr("Predicted Absence") + "</th>\n"
    "<td border=\"1\"><center>" + QString::number(falseNegativeCount()) + " (c)</center></td>\n"
    "<td border=\"1\"><center>" + QString::number(trueNegativeCount()) + " (d)</center></td>\n"
    "</tr>\n"
    "<tr>\n"
    "<td colspan=\"3\"><br/><center>\n"
    "Threshold " + QString::number(threshold()*100) + "%\n"
    "</center></td>\n"
    "</tr>\n"
    "</table></center>\n"
    "<br/>\n"
    // derived stats from confusion matrix
    "<center><table width=\"100%\">\n"
    "<tr>\n"
    "<td colspan=\"3\" class=\"glossy\"><center>" + 
    QObject::tr("Statistics") + "</center></th>\n"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\" width=\"60%\">" + QObject::tr("Number of usable presences") + "</td>\n"
    "<td width=\"20%\">" + QString::number(usablePresenceCount()) + "</td>"
    "<td width=\"20%\"></td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Number of usable absences") + "</td>\n"
    "<td>" + QString::number(usableAbsenceCount()) + "</td>"
    "<td></td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Prevalence") + "</td>\n"
    "<td>" + myPrevalenceStr + "</td>"
    "<td>(a + c) / N</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Global diagnostic power") + "</td>\n"
    "<td>" + myGlobalDiagnosticPowerStr + "</td>"
    "<td>(b + d) / N</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Correct classification rate") + "</td>\n"
    "<td>" + QString::number(calculateRateOfCorrectClassification()) + "</td>"
    "<td>(a + d) / N</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Sensitivity") + "</td>\n"
    "<td>" + QString::number(calculateSensitivity()) + "</td>"
    "<td>a /(a + c)</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Specificity") + "</td>\n"
    "<td>" + mySpecificityStr + "</td>"
    "<td>d /(b + d)</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Omission error") + "</th>\n"
    "<td>" + QString::number(calculateOmmissionRate()) + "</td>"
    "<td>c /(a + c)</td>"
    "</tr>\n"
    "<tr>\n"
    "<td class=\"headerCell\">" + QObject::tr("Commision error") + "</td>\n"
    "<td>" + myCommissionStr + "</td>"
    "<td>b /(b + d)</td>"
    "</tr>\n"
    "<tr>\n"
    "<td colspan=\"3\"><center>\n"
    + QObject::tr("Key: ") + "(a) " + QObject::tr("True Positive") + "\n"
    " (b) " + QObject::tr("False Positive") + "\n"
    " (c) " + QObject::tr("False Negative") + "\n"
    " (d) " + QObject::tr("True Negative") + "\n"
    " (N) " + QObject::tr("Number of samples") + "\n"
    "</center></td>\n"
    "</tr>\n"
    "<tr>\n"
    "<td colspan=\"3\"><center>\n"
    + QObject::tr("Methodology: ") + "Fielding, A. H., and J. F. Bell. 1997. "
    "A review of methods for the assessment of prediction errors in conservation "
    "presence/absence models. Environmental Conservation 24:38-49."
    "</center></td>\n"
    "</tr>\n"
    "</table></center>\n";

  return myString;
}

void OmgModelTest::createRocGraph(QString theWorkDir, QString theTaxonName) const 
{
#if WITH_QWT
  //delegate to the qwt roc fn
  createQwtRocGraph(theWorkDir,theTaxonName);
#else
  Q_UNUSED(theTaxonName);
  QImage myImage = QImage( 200,200,QImage::Format_ARGB32 );
  myImage.fill(Qt::white);
  QPainter myPainter ( &myImage );
  OmgGraph myGraph(&myPainter);
  OmgDataSeries mySeries = getRocPoints();
  mySeries.setLineColor(Qt::darkGray);
  mySeries.setFillColor(QColor("#3087d3"));
  myGraph.addSeries(mySeries);
  myGraph.setVerticesEnabled(false);
  myGraph.setVertexLabelsEnabled(false);
  myGraph.setAreaFillEnabled(true);
  myGraph.setSpliningEnabled(false);
  myGraph.setGridLinesEnabled(true);
  myGraph.setDiagonalEnabled();
  myGraph.render();
  QString myFileName = theWorkDir + QDir::separator() + guid() + "_roc_curve.png";
  qDebug ("Saving roc graph to " + myFileName.toLocal8Bit());
  myImage.save(myFileName,"PNG");
#endif
}

#if WITH_QWT
void OmgModelTest::createQwtRocGraph(QString theWorkDir, QString theTaxonName) const 
{
  qDebug("createQwtRocGraph start");
  QwtPlot * mypPlot = new QwtPlot();
  mypPlot->setTitle(QObject::tr("Sensitivity vs. 1 - Specificity for %1")
      .arg(theTaxonName));
  
  //mypPlot->insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
  
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
  QwtPlotCurve * mypRocCurve = getRocCurve(); 
  mypRocCurve->attach(mypPlot);
  //
  // now make a AUC=0.5 line too
  //
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
  //
  // Finish up by screen grabbing the widget
  //
  mypPlot->resize(QSize(600,600));
  mypPlot->setCanvasBackground(Qt::white);
  mypPlot->setCanvasBackground(Qt::white);
  mypPlot->replot();
  QPixmap myPixmap = QPixmap::grabWidget(mypPlot);
  QString myFileName = theWorkDir + QDir::separator() + guid() + "_roc_curve.png";
  myPixmap.save(myFileName); 
  //mypPlot->show(); //for debugging only
  //ensure all children get removed
  mypPlot->setAutoDelete(true);
  delete mypPlot;
  qDebug("createQwtRocGraph done");
}

QwtPlotCurve * OmgModelTest::getRocCurve() const
{
  //
  // Actual auc values
  //
  QwtPlotCurve * mypRocCurve = 
    new QwtPlotCurve(QObject::tr("Training Data (AUC=%1").arg(rocScore()));
  mypRocCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
  mypRocCurve->setPen(QPen(Qt::red));
  QwtArray<double> myXData;//qwtarray is just a wrapped qvector
  QwtArray<double> myYData;//qwtarray is just a wrapped qvector
  QListIterator< QPair <double,double> > myIterator (mRocPoints);
  while (myIterator.hasNext())
  {
    QPair<double,double> myPair = myIterator.next();
    myXData.append(myPair.first);
    myYData.append(myPair.second);
  }
  mypRocCurve->setData(myXData,myYData);
  return mypRocCurve;
}
#endif //QWT


void OmgModelTest::addRocPoint( QPair <double,double> thePair )
{
  mRocPoints << thePair;
}

void OmgModelTest::clearRocPoints()
{
  mRocPoints.clear();
}

OmgDataSeries OmgModelTest::getRocPoints() const
{
  OmgDataSeries mySeries;
  mySeries.setLabel( QObject::tr("Roc Curve") );
  QListIterator< QPair <double,double> > myIterator (mRocPoints);
  while (myIterator.hasNext())
  {
    QPair<double,double> myPair = myIterator.next();
    mySeries << myPair;
  }
  return mySeries;
}

//
// mutators
// 

void OmgModelTest::setLabel(QString theLabel)
{
  mLabel=theLabel;
}

void OmgModelTest::setError(bool theFlag)
{
  mErrorFlag=theFlag;
}

void OmgModelTest::setCompleted(bool theFlag)
{
  mCompletedFlag=theFlag;
}

void OmgModelTest::setInternal(bool theFlag)
{
  mInternalFlag=theFlag;
}

void OmgModelTest::setStartDateTimeStamp()
{
  mStartDateTimeStamp=QDateTime::currentDateTime();
}

void OmgModelTest::setStartDateTimeStamp(QDateTime theDateTimeStamp)
{
  mStartDateTimeStamp=theDateTimeStamp; 
}

void OmgModelTest::setEndDateTimeStamp()
{
  mEndDateTimeStamp = QDateTime::currentDateTime();
}

void OmgModelTest::setEndDateTimeStamp(QDateTime theDateTimeStamp)
{
  mEndDateTimeStamp=theDateTimeStamp; 
}

void OmgModelTest::setUseLowestThreshold(bool theChoice)
{
  mUseLowestThreshold = theChoice;
}

void OmgModelTest::setThreshold(double theThreshold)
{
  mThreshold = theThreshold;
}

void OmgModelTest::setOmission(double theOmission)
{
  mOmission=theOmission;
}

void OmgModelTest::setCommission(double theCommission)
{
  mCommission=theCommission;
}
void OmgModelTest::setTruePositiveCount(int theCount)
{
  mTruePositiveCount = theCount;
}
void OmgModelTest::setFalsePositiveCount(int theCount)
{
  mFalsePositiveCount = theCount;
}
void OmgModelTest::setFalseNegativeCount(int theCount)
{
  mFalseNegativeCount = theCount;
}
void OmgModelTest::setTrueNegativeCount(int theCount)
{
  mTrueNegativeCount = theCount;
}
void OmgModelTest::setRocScore(double theRocScore)
{
  mRocScore=theRocScore;
}
void OmgModelTest::setAccuracy(double theAccuracy)
{
  mAccuracy=theAccuracy;
}
void OmgModelTest::setMaxOmission(double theMaxOmission)
{
  mMaxOmission=theMaxOmission;
}
void OmgModelTest::setAreaRatio(double theAreaRatio)
{
  mAreaRatio=theAreaRatio;
}
void OmgModelTest::setNumBackgroundPoints(int theNumber)
{
  mNumBackgroundPoints = theNumber;
}
void OmgModelTest::setLocalities(OmgLocalities theLocalities)
{
  mLocalities = theLocalities;
}

void OmgModelTest::appendToLog(QString theMessage)
{
  mLog += theMessage + "\n";
}
void OmgModelTest::setLog(QString theLog)
{
  mLog = theLog;
}

//
// Accessors
//
QString OmgModelTest::label() const
{
  return mLabel;
}
bool OmgModelTest::hasError() const
{
  return mErrorFlag;
}
bool OmgModelTest::isCompleted() const
{
  return mCompletedFlag;
}
bool OmgModelTest::isInternal() const
{
  return mInternalFlag;
}
QDateTime OmgModelTest::startDateTimeStamp() const
{
  return mStartDateTimeStamp;
}
QDateTime OmgModelTest::endDateTimeStamp() const
{
  return mEndDateTimeStamp;
}
bool OmgModelTest::useLowestThreshold() const
{
  return mUseLowestThreshold;
}
double OmgModelTest::threshold() const
{
  return mThreshold;
}
double OmgModelTest::omission() const
{
  return mOmission;
}
double OmgModelTest::commission() const
{
  return mCommission;
}
int OmgModelTest::truePositiveCount() const
{
  return mTruePositiveCount;
}
int OmgModelTest::falsePositiveCount() const
{
  return mFalsePositiveCount;
}
int OmgModelTest::falseNegativeCount() const
{
  return mFalseNegativeCount;
}
int OmgModelTest::trueNegativeCount() const
{
  return mTrueNegativeCount;
}
double OmgModelTest::rocScore() const
{
  return mRocScore; 
}
double OmgModelTest::maxOmission() const
{
  return mMaxOmission; 
}
double OmgModelTest::areaRatio() const
{
  return mAreaRatio; 
}
int OmgModelTest::numBackgroundPoints() const
{
  return mNumBackgroundPoints;
}
double OmgModelTest::accuracy() const
{
  return mAccuracy; 
}
int OmgModelTest::usablePresenceCount() const
{
  return mTruePositiveCount + mFalseNegativeCount;
}
int OmgModelTest::usableAbsenceCount() const
{
  return mTrueNegativeCount + mFalsePositiveCount;
}

OmgLocalities OmgModelTest::getLocalities()
{
  return mLocalities;
}

bool OmgModelTest::checkConfusionMatrix() const
{
  if (0==truePositiveCount() && 0==falsePositiveCount() && 
      0==falseNegativeCount() && 0==trueNegativeCount()) 
  {
    return false;
  }
  else 
  {
    return true;
  }
}

//
// These methods after Fielding and Bell 1997
//
float OmgModelTest::calculatePrevalence() const
{
  if (!checkConfusionMatrix()) return -1;
  if (0==usablePresenceCount()) return -1; //prevent divide by zero
  return static_cast<float>((truePositiveCount() + falseNegativeCount())) / usablePresenceCount();
}
float OmgModelTest::calculateGlobalDiagnosticPower() const
{
  if (!checkConfusionMatrix()) return -1;
  if (0==usablePresenceCount()) return -1; //prevent divide by zero
  return static_cast<float>((falsePositiveCount() + trueNegativeCount())) / usablePresenceCount();
}
float OmgModelTest::calculateRateOfCorrectClassification() const
{
  if (!checkConfusionMatrix()) return -1;
  if (0==usablePresenceCount()) return -1; //prevent divide by zero
  return static_cast<float>((truePositiveCount() + trueNegativeCount())) / usablePresenceCount();
}
float OmgModelTest::calculateSensitivity() const
{
  if (!checkConfusionMatrix()) return -1;
  if (0==truePositiveCount() && 0==falseNegativeCount())  return -1; //prevent divide by zero
  return static_cast<float>(truePositiveCount()) / (truePositiveCount() + falseNegativeCount());
}
float OmgModelTest::calculateSpecificity() const
{
  if (!checkConfusionMatrix()) return -1;
  if (0==trueNegativeCount() && 0==falsePositiveCount()) return -1; //prevent divide by zero
  return static_cast<float>(trueNegativeCount()) / (falsePositiveCount() + trueNegativeCount());
}
float OmgModelTest::calculateCommisionRate() const //false positives
{
  if (!checkConfusionMatrix()) return -1;
  if (0==falsePositiveCount() && 0==trueNegativeCount()) return -1; //prevent divide by zero
  return static_cast<float>(falsePositiveCount()) / (falsePositiveCount() + trueNegativeCount());
}
float OmgModelTest::calculateOmmissionRate() const //false negatives
{
  if (!checkConfusionMatrix()) return -1;
  if (0==falseNegativeCount() && 0==truePositiveCount()) return -1; //prevent divide by zero
  return static_cast<float>(falseNegativeCount()) / (truePositiveCount() + falseNegativeCount());
}
//
// End of Fielding and Bell methods
//

float OmgModelTest::calculateKappa() const
{
  // Kappa requires presence and absence data
  if (0==usablePresenceCount()) return -1;
  if (0==usableAbsenceCount()) return -1;

  int total = usablePresenceCount() + usableAbsenceCount();

  int group1 = truePositiveCount()+trueNegativeCount();

  int group2 = (truePositiveCount()+falseNegativeCount())*(truePositiveCount()+falsePositiveCount());
  
  int group3 = (falsePositiveCount()+trueNegativeCount())*(falseNegativeCount()+trueNegativeCount());

  float numerator = static_cast<float>(group1 - ((group2+group3)/total));

  float denominator = static_cast<float>(total - ((group2+group3)/total));

  if (0.0==denominator) return -1; // avoid divide by zero

  return numerator/denominator;
}

QString OmgModelTest::toSamplerXml() const
{
  QString myPresencesXml = "<Presence Label=\""+mpModel->taxonName()+"\">";
  myPresencesXml += "<CoordinateSystem>"+mpModel->coordinateSystem()+"</CoordinateSystem>";

  QString myAbsencesXml = "<Absence Label=\""+mpModel->taxonName()+"\">";
  myAbsencesXml += "<CoordinateSystem>"+mpModel->coordinateSystem()+"</CoordinateSystem>";

  bool myHasAbsences = false;

  for (int i=0;i<mLocalities.count();i++)
  {
    OmgLocality myLocality = mLocalities.at(i);
    if (myLocality.abundance() > 0.0)
    {
      myPresencesXml+=myLocality.toXml();
    }
    else
    {
      myHasAbsences = true;
      myAbsencesXml+=myLocality.toXml();
    }
  }

  myPresencesXml += "</Presence>";
  myAbsencesXml += "</Absence>";

  QString mySamplerXml = "<Sampler>" + mpModel->creationLayerSet().toXml() + myPresencesXml;

  if (myHasAbsences)
  {
    mySamplerXml += myAbsencesXml;
  }

  mySamplerXml += "</Sampler>";

  return mySamplerXml;
}

void OmgModelTest::setShapefileName(const QString theShapefileName)
{
  QFileInfo myFileInfo(theShapefileName);
  mShapefileName = myFileInfo.fileName();
}

QString OmgModelTest::shapefileName() const
{
	//qDebug("mShapefileName: " + this->mShapefileName );
	QFileInfo myFileInfo(mShapefileName);
	QString myFileName=myFileInfo.fileName();
	return myFileName;
}
