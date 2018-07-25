 /*   Copyright (C) 2005 by Tim Sutton   *
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
#ifndef OMGPROJECTION_H
#define OMGPROJECTION_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QVector>
#include <QList>
#include <QPair>
//#include "omgmodel.h"
#include "omglocality.h"
#include "omgserialisable.h"
#include "omglayerset.h"
#include "omglayer.h"
#include "omgguid.h"
#include "omgdataseries.h"

/** \ingroup library
 * \brief A representation of a model. Can be serialised to xml that openmodeller
 * lib understands and can store rich model info returned by openModeller lib. 
 */
class OMG_LIB_EXPORT OmgProjection : public OmgSerialisable, public OmgGuid
{

public:

  OmgProjection();
  ~OmgProjection();
  const bool isValid() const;
  /** Return a string representation of the model.
   * Currently only intended for debuggin purposes.
   */
  QString toString() const;
  /** Create an abbreviated html formatted summary of the model
   * and save it to disk.
   * @return QString containing the html snippet for the summary report.
   */
  QString toSummaryHtml() const;
  /** Create an html formatted summary of the model
   * and save it to disk.
   * @param bool flag indicating whether to force 
   * recreating the report or whether to just 
   * return the existing one if it was made previously.
   * If the file does not exist the flag is ignored
   * and the report is created anyway.
   * By default this flag is false.
   * @return QString containing the full file name and 
   * path to the summary report.
   */
  QString toHtml(bool theForceFlag=false) const ;
  /** A convenience method to get the projection layers in xml representation.
   * @return a string containing xml describing the projection layerset
   */
  QString projectionLayersXml() ;
  void reset();

  //mutators
  void setOutputFormatLayerName(QString theName);
  /** Define the layerset that will be used for projecting this model.
   * @param theLayerSet An OmgLayerSet that contains layers and mask for model projection.
   */
  void setLayerSet(OmgLayerSet theLayerSet);
  
  //
  // The following two mutators will be deprecated in favour of the above
  //
  
  void setLayerNames(QStringList theNames);
  void setMaskLayerName(QString theName);
  
  void setAlgorithm(OmgAlgorithm theAlgorithm);
  void setModelGuid(QString theGuid);
  void setModelDefinition(QString theDefinition);
  /** Some algs will return a serialised normalization definition
   * (since om lib supports different normalizer) */
  void setNormalizationDefinition(QString theDefinition);
  void setTaxonName(QString theTaxonName);
  void setCompleted(bool theFlag);
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
  
  void setThumbnailFileName(QString theFileName);
  void setPreviewFileName(QString theFileName);
  void setLegendFileName(QString theFileName);
  void setRawImageFileName(QString theFileName);
  void setColouredImageFileName(QString theFileName);
  void setOmRasterFormat(QString theFormat);
  void setWorkDir(QString theDir);
  /** set the accuracy for this model (from confusion matrix) */
  void setAccuracy(double theAccuracy);
  /** set the percent cells present for this model (from confusion matrix) */
  void setPercentCellsPresent(double thePercent );
  /** set the totalCells for this model (from confusion matrix) */
  void setTotalCells(double theTotal);
  
  //accessors
 
  /** Retreive the globally unique identifier for this model */
  OmgLayerSet layerSet() const;
  QString maskLayerName() const;
  OmgLayer outputFormatLayer() const;
  QString outputFormatLayerName() const;
  QStringList projectionLayerNames() const;
  //OmgModel * model() const;
  OmgAlgorithm algorithm() const;
  QString modelGuid() const;
  QString modelDefinition() const;
  QString normalizationDefinition() const;
  QString taxonName() const;
  bool isCompleted() const;
  bool hasError() const;
  QDateTime startDateTimeStamp() const;
  QDateTime endDateTimeStamp() const;
  QString thumbnailFileName() const;
  QString previewFileName() const;
  QString legendFileName() const;
  QString rawImageFileName() const;
  QString colouredImageFileName() const;
  QString omRasterFormat() const;
  /** The directory where the working files were stored 
   * for this model */
  QString workDir() const;
  /** Get the percent cells present for this model (from confusion matrix) */
  double percentCellsPresent() const;
  /** Get the totalCells for this model (from confusion matrix) */
  double totalCells() const;
  
  QString projectionLog() const;
  //
  // General helper functions
  //

  /** Create the thumbnail preview of the model for the thumbnail 
   * browser. */
  void createThumbnail();
  /** Perform post processing tasks on the model.
      This will generate a pseudocolor preview image,
      csv localities file and other products from the
      model output.
      @return bool true on success
      */
  bool postProcess();

  //
  // implement the serialisable interface:
  //

  /** This method is required for the serialiseable interface. It will 
   * encapsulate *all* model properties so that its complete state can
   * be serialised and later deserialised.
   * @see toModelCreationXml()
   */
  QString toXml() const;
  /** Implments the serialisable interface method to reinstate a model.
   * It will try to populate all members of this class and fail gracefully when
   * they are not available (typically by leaving the member tagged as [Not Set]
   */
  bool fromXml(const QString theXml);

  /** This is a special case serialiser needed for when preparing an xml
   * representation of an model projection to send to the openmodeller library
   * (or openmodeller web service interface).  The xml produced will include no
   * ancilliary meta information - only the essentials needed for the
   * projection.  
   * @return a string containing the model projection xml
   * representation, or if something went wrong an empty string.
   */
  QString toProjectionXml();


 public slots:
  /** Append a message to the model log */
  void appendToLog(QString theMessage);
  /** Set the log to the provided message (overwriting any data that was 
   * previously there */
  void setLog(QString theLog);
private:

  OmgLayer mOutputFormatLayer;
  OmgLayerSet mProjectionLayerSet;
  //OmgModel * mpModel;
  OmgAlgorithm mAlgorithm;
  QString mModelDefinition;
  QString mNormalizationDefinition;
  QString mModelGuid;
  QString mTaxonName;
  bool mCompletedFlag;
  bool mErrorFlag;
  QDateTime mStartDateTimeStamp;
  QDateTime mEndDateTimeStamp;
  QString mThumbnailFileName;
  QString mPreviewFileName;
  QString mLegendFileName;
  QString mRawImageFileName;
  QString mColouredImageFileName;
  QString mOmRasterFormat; //passed to om e.g. GrayTiff
  QString mWorkDir;
  double mPercentCellsPresent;
  double mTotalCells;
  QString mProjectionLog;
};

#endif //OMGPROJECTION_H

