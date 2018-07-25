 /*   Copyright (C) 2008 by Tim Sutton                                     *
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
#ifndef OMGMODELTEST_H
#define OMGMODELTEST_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QVector>
#include <QList>
#include <QPair>
#include <QThread>
#include <QDomElement>
#include "omgserialisable.h"
#include "omgguid.h"
#include "omgdataseries.h"
#include "omglocality.h"

class OmgModel;
class QwtPlotCurve;

/** \ingroup library
 * \brief A representation of a model test (its confusion matrix, roc table etc).
 */
class OMG_LIB_EXPORT OmgModelTest : public OmgSerialisable, 
                                    public OmgGuid
{

public:

  OmgModelTest(const OmgModel * const thepModel);
  ~OmgModelTest();
  /** Create a plain text string report of the test output */
  QString toString() const;
  /** Create an html formatted sumary of the model
   * and save it to disk.
   * @param bool flag indicating whether to force 
   * recreating the report or whether to just 
   * return the existing one if it was made previously.
   * If the file does not exist the flag is ignored
   * and the report is created anyway.
   * By default this flag is false.
   * @return QString with the filename of the html report
   */
  QString toHtml(bool theForceFlag=false) const;
  /** Get the print ready reoport for this model test.
   * @param theForceFlag - force recreation of the 
   * report and its graphs etc.
   * @return QString with the html report contents
   */
  QString toPrintHtml(bool theForceFlag=false) const;
  /** Create an image of the roc curve. If QWT is available 
   * it will delegate to createQwtRocGraph(). If not, it will 
   * draw a more simple graph representation.
   * The output image will be <guid>_roc_curve.png where the
   * guid is the globally unique identifier for this test.
   * @parameter the work directory in which the graph will be saved.
   * @parameter the taxon name used to label the chart. This
   * parameter will be unused unless delegating to the QWT 
   * renderer.
   **/
  void createRocGraph(QString theWorkDir, QString theTaxonName) const;

  //
  // mutators
  //
  void setLabel(QString theLabel);

  void setCompleted(bool theFlag);
  void setInternal(bool theFlag);
  void setError(bool theFlag);
  
  /** Set the start of run time stamp, using the current time */
  void setStartDateTimeStamp();
  /** Set the start of run time stamp, using a time supplied 
   * @param theDataTimeStamp - the time to use for the start of run mark
   */
  void setStartDateTimeStamp(QDateTime theDateTimeStamp);
  /** Set the end of run time stamp, using a time supplied 
   * @param theDataTimeStamp - the time to use for the end of run mark
   */
  void setEndDateTimeStamp();
  /** Set the end of run time stamp, using a time supplied 
   * @param theDataTimeStamp - the time to use for the end of run mark
   */
  void setEndDateTimeStamp(QDateTime theDateTimeStamp);

  /** set the option to use lowest training probability as the threshold (confusion matrix) */
  void setUseLowestThreshold(bool theChoice);
  /** set the threshold (confusion matrix) */
  void setThreshold(double theThreshold);
  /** set the omission score for this model test (from confusion matrix) */
  void setOmission(double theOmission);
  /** set the commission score for this model test (from confusion matrix) */
  void setCommission(double theCommission);
  /** Set the number of samples counted as predicted that actualy were present */ 
  void setTruePositiveCount(int theCount); //#of samples
  /** Set the number of samples counted as predicted that actualy were *NOT* present */ 
  void setFalsePositiveCount(int theCount); //#of samples
  /** Set the number of samples counted as absent that actualy *WERE* present */ 
  void setFalseNegativeCount(int theCount); //#of samples
  /** Set the number of samples counted as absent that actualy *WERE* absent */ 
  void setTrueNegativeCount(int theCount); //#of samples
  /** Set the rocScore for this model test (from roc analysis) */
  void setRocScore(double theRocScore);
  /** Set the accuracy for this model test (from confusion matrix) */
  void setAccuracy(double theAccuracy);

  /** set the maximum omission for this model test (partial ROC). -1 means NULL */
  void setMaxOmission(double theMaxOmission);
  /** set the area ratio for this model test (partial ROC). -1 means NULL */
  void setAreaRatio(double theAreaRatio);
  /** Set the number of background points to calculate the ROC curve. -1 means NULL */
  void setNumBackgroundPoints(int theNumber);

  /** set the localities for this model test */
  void setLocalities(OmgLocalities theLocalities);

  void setShapefileName(QString theFileName);
  
  //
  // Accessors
  //
  QString label() const;

  bool isCompleted() const;
  bool isInternal() const;
  bool hasError() const;
  QDateTime startDateTimeStamp() const;
  QDateTime endDateTimeStamp() const;

  /** Indicate if the lowest training probability should be used as the threshold 
      (from confusion matrix) */
  bool useLowestThreshold() const;
  /** Get the threshold (from confusion matrix) */
  double threshold() const;
  /** Get the omission score for this model (from confusion matrix) */
  double omission() const;
  /** Get the commission score for this model (from confusion matrix) 
   * @return -1 if the commision is invalid ( in the case of
   * models run with no absence data)
   */
  double commission() const;
  /** The number of samples counted as predicted that actualy were present */ 
  int truePositiveCount() const; //#of samples
  /** The number of samples counted as predicted that actualy were *NOT* present */ 
  int falsePositiveCount() const; //#of samples
  /** The number of samples counted as absent that actualy *WERE* present */ 
  int falseNegativeCount() const; //#of samples
  /** The number of samples counted as absent that actualy *WERE* absent */ 
  int trueNegativeCount() const; //#of samples
  /** Get the rocScore for this model (from roc analysis) */
  double rocScore() const;
  /** Get the maximum omission used to calculate partial ROC. -1 means NULL */
  double maxOmission() const;
  /** Get the area ratio for a partial ROC. -1 means NULL */
  double areaRatio() const;
  /** Get the number of background points to calculate the ROC curve. -1 means NULL */
  int numBackgroundPoints() const;
  /** Get the accuracy for this model (from confusion matrix) */
  double accuracy() const;

  /** get localities of this model test */
  OmgLocalities getLocalities();

  QString shapefileName() const;

  //
  // Serialisable
  //

  
  /** This method is required for the serialiseable interface. It will 
   * encapsulate *all* model test properties so that its complete state can
   * be serialised and later deserialised.
   * @see toModelCreationXml()
   */
  QString toXml() const;
  /** Implments the serialisable interface method to reinstate a model.
   * It will try to populate all members of this class and fail gracefully when
   * they are not available (typically by leaving the member tagged as [Not Set]
   */
  bool fromXml(const QString theXml);

  /** Add a point to the list of roc xy values for the roc graph.
   * @param QPair<double,double> a pair of doubles representing the
   * x,y values that are added to the roc curve.
   */
  void addRocPoint( QPair <double,double> thePair );
  /** Clear the list of roc xy values for the roc graph */
  void clearRocPoints();
  /** Get the roc graph as an omgdataseries */
  OmgDataSeries getRocPoints() const;
  /** The number of presence localities. Excludes any localities 
   * that were excluded due to masking or spatial / environmental 
   * uniqueness filtering.*/
  int usablePresenceCount() const;
  /** The number of absence localities. Excludes ny localities 
   * that were excluded due to masking or spatial / environmental 
   * uniqueness filtering.*/
  int usableAbsenceCount() const;
  
  // These methods after Fielding and Bell 1997
  //
  /** Test whether the confusion matrix appears to be valid. 
   * This is done by checking that the sum of mTruePositiveCount, mFalsePositiveCount,
   * mFalseNegativeCount, mTrueNegativeCount is not zero. 
   * @return bool true if the sum of components is > 0. */;
  bool checkConfusionMatrix() const;
  /** Calculate model prevalence (after Fielding and Bell, 1997).
   * Prevalence is expressed as:
   * (truePositiveCount + falseNegativeCount) / usable presence count
   * @return a float expressing model prevalence.
   */
  float calculatePrevalence() const;
  /** Calculate model diagnostic power (after Fielding and Bell, 1997).
   * Diagnostic power is expressed as:
   * (falsePositiveCount + trueNegativeCount) / usable presence count
   * @return a float expressing model diagnostic power.
   */
  float calculateGlobalDiagnosticPower() const;
  /** Calculate rate of correct classification (after Fielding and Bell, 1997).
   * Rate of correct classification is expressed as:
   * (truePositiveCount + trueNegativeCount) / usable presence count
   * @return a float expressing rate of correct classification..
   */
  float calculateRateOfCorrectClassification() const;
  /** Calculate model sensitivity (after Fielding and Bell, 1997).
   * Model sensitivity is expressed as:
   * truePositiveCount / (truePositiveCount + falseNegativeCount)
   * @return a float expressing model sensitivity.
   */
  float calculateSensitivity() const;
  /** Calculate model specificity (after Fielding and Bell, 1997).
   * Model specificity is expressed as:
   * trueNegativeCount / (falsePositiveCount + trueNegativeCount)
   * @return a float expressing model specificity.
   */
  float calculateSpecificity() const;
  /** Calculate model falsePositiveCount rate (rate of commission errors, after
   * Fielding and Bell, 1997).
   * Commission rate is expressed as:
   * falsePositiveCount / (falsePositiveCount + trueNegativeCount)
   * @return a float expressing model commission rate.
   */
  float calculateCommisionRate() const; //false positives
  /** Calculate model falseNegativeCount rate (rate of ommission errors, after
   * Fielding and Bell, 1997).
   * falseNegativeCount / (truePositiveCount + falseNegativeCount)
   * @return a float expressing model prevalence.
   */
  float calculateOmmissionRate() const; //false negatives 
  //
  // End of Fielding and Bell methods
  //

  /** Kappa */ 
  float calculateKappa() const;

#if WITH_QWT
  /** Get just the curve without the graph (usefull for other classes 
   * that want to summarise multiple tests into a single graph)
   */
  QwtPlotCurve * getRocCurve() const;

#endif //WITH_QWT

  /** This method returns an XML representation compatible with the openModeller sampler
   * class based on the localities and on the model.
   */
  QString toSamplerXml() const;

 public slots:
  /** Append a message to the test log */
  void appendToLog(QString theMessage);
  /** Set the log to the provided message (overwriting any data that was 
   * previously there */
  void setLog(QString theLog);

private:

#if WITH_QWT
  /** Create an image of the roc curve using QWT. This function cannot
   * be called directly, by is delegated to myCreateRocGraph if 
   * QWT is available.
   * The output image will be <guid>_roc_curve.png where the
   * guid is the globally unique identifier for this test.
   * @parameter the work directory in which the graph will be saved.
   * @parameter the taxon name used to label the chart
   **/
  void createQwtRocGraph(QString theWorkDir, QString theTaxonName) const;

#endif //WITH_QWT

  /** Model used in the test */
  const OmgModel * const mpModel;

  /** Localities used in the test */
  OmgLocalities mLocalities;

  QString mLabel;

  bool mCompletedFlag;
  bool mInternalFlag;
  bool mErrorFlag;
  QDateTime mStartDateTimeStamp;
  QDateTime mEndDateTimeStamp;

  QString mLog;

  QString mShapefileName;

  //
  // Model statistics
  //
  /* Indicates whether the lowest probability of all training points should 
     be used as the threshold */
  bool mUseLowestThreshold;
  /* Confusion matrix threshold */
  double mThreshold;
  /* Percentage ommission */
  double mOmission;
  /** -1 indicates invalid since commission only
   * applies to presence/absence models */
  double mCommission;
  /** Number of samples counted as predicted that actualy were present */ 
  int mTruePositiveCount;
  /** Number of samples counted as predicted that actualy were *NOT* present */ 
  int mFalsePositiveCount;
  /** Number of samples counted as absent that actualy *WERE* present */ 
  int mFalseNegativeCount;
  /** Number of samples counted as absent that actualy *WERE* absent */ 
  int mTrueNegativeCount;
  /** Roc score for the model test. */
  double mRocScore;
  /** Maximum omission (only for partial Roc). Value between 0 and 1. NULL is -1 */
  double mMaxOmission;
  /** Area Ratio for points below the maximum omission (only for partial Roc). NULL is -1 */
  double mAreaRatio;
  /** Number of background points to be generated for calculating the ROC curve in case 
      there are no absences. -1 means NULL */ 
  int mNumBackgroundPoints;
  /** Accuracy for the model. */
  double mAccuracy;
  /** A list of points for the roc table */ 
  QList < QPair <double,double> > mRocPoints;
};

#endif // OMGMODELTEST_H
