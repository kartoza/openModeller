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
#ifndef OMGMODEL_H
#define OMGMODEL_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QVector>
#include <QList>
#include <QPair>
#include <QThread>
#include <QDomElement>
#include "omgalgorithm.h"
#include "omglocality.h"
#include "omgserialisable.h"
#include "omglayerset.h"
#include "omglayer.h"
#include "omgmodeltest.h"
#include "omgprojection.h"
#include "omgguid.h"
#include "omgdataseries.h"
#include "omgmodellerplugininterface.h"


/** \ingroup library
 * \brief A representation of a model. Can be serialised to xml that openmodeller
 * lib understands and can store rich model info returned by openModeller lib. 
 */
class OMG_LIB_EXPORT OmgModel : public QThread,
                                public OmgSerialisable, 
                                public OmgGuid
{

  Q_OBJECT; //needed for QPointer guarded pointer support
public:

  OmgModel();
  ~OmgModel();
  /** Overloaded setGuid of OmgGuid superclass to ensure that projections 
   * also get their model id set.
   * @param theGuid - the unique id to set for this model.
   */
  void setGuid(QString theGuid="");
  /** An enumerated type defining possible types of localities filter */
  enum LocalitiesFilterType {NO_FILTER,SPATIALLY_UNIQUE,ENVIRONMENTALLY_UNIQUE};
  /** Run this model. If running in threaded mode will be called by start()
   * which is implmented in QThread. */
  void run();

  /** Add a new test to this model and run the new test.
  */
  void runTest( OmgModelTest * thepTest );
  /** Return a string representation of the model.
   * Currently only intended for debuggin purposes.
   */
  QString toString() const;
  /** Create an html formatted sumary of the model
   * and save it to disk.
   * @param bool flag indicating whether to force 
   * recreating the report or whether to just 
   * return the existing one if it was made previously.
   * If the file does not exist the flag is ignored
   * and the report is created anyway.
   * By default this flag is false.
   * @param theForceGraphRenderFlag rendering graphs in a non gui context can
   * cause a crash
   * @return QString with the filename of the html report
   */
  QString toHtml(bool theForceFlag=false, bool theForceGraphRenderFlag=false) const;
  /** Get the print ready reoport for this model. It
   * excludes some portions of the on screen report
   * such as model log. It also uses absolute paths
   * for images so they are properly include into the 
   * printout.
   * @param theForceFlag - force recreation of the 
   * report and its graphs etc.
   * @return QString with the html report contents
   */
  QString toPrintHtml(bool theForceFlag=false) const ;
  /** Reset the model marking it as not started, not errored etc. */
  void reset();
  /** Parse the model xml provided and get the samples and add them to
   * the model. Also add the serialised model.
   * @param QString theModelXml a model as returned by the modeller plugin
   * @return bool true on success.
   */
  bool  parseModelResult(QString theModelXml);

  /** Clear all the independent tests for this model */
  void clearTests();
  /** Add a test to the list of test compartments */
  void addTest(OmgModelTest * thepTest);
  //
  // mutators
  //

  /** Set the modeller plugin adapter to use for
   * actually running models.
   */
  void setModellerPlugin(OmgModellerPluginInterface * thePlugin);
  
  /** Set the number of independent testing compartments to use. This
   * is only used at model creation time. After that the OmgTest qlist
   * member will be used as a canonical reference to how many independent 
   * tests were carried out. The number of compartments specified will 
   * alter the model creation routines post processing behaviour in the 
   * following way:
   *  - Case 1 we use 1 or more 'compartments' to carry out
   *    external testing on a randomly split set of samples
   *    with a specified train/test proportion.
   *  - Case 2 no external testing, model test is generated 
   *    using the default model confusion matrix and roc 
   *    score created at model creation time (compartment count 
   *    is <= 0)
   */
  void setTestCompartmentCount(int theCount);
  /** Set and Get TrainingProportion.
  /* A value between 0 and 1 to specify split proportions
  /* e.g. 0.9 will use 90% points for training and 10% for testing
  */
  void setTrainingProportion( float proportion );
  float getTrainingProportion();

  /** Set the coordinate system in WKT format */
  void setCoordinateSystem(QString theCoordinateSystem);
  /** Set the user friendly name for the WKT format */
  void setCoordinateSystemName (QString theName);
  /** Set the file containing occurrence data. This will 
   * populate the Localities collection.
   * @param theSpeciesFile - filename containing locality data.
   */
  void setOccurrencesFile(QString theSpeciesFile);
  /** Set the species file to be used for the model. I will change 
   * this later so that it is not file based but rather the points
   * will be added directly to the model. For now, calling this method will
   * also cause the points to be loaded into the OmgLocalities collection
   * associated with this model. */
  void setSpeciesFile(QString theFileName );
  void setShapefileName(QString theFileName);
  void setCsvFileName(QString theFileName);
  void setTaxonName(QString theName);
  void setLocalities(OmgLocalities theLocalities);
 
  /** Set the output model raster format - the name should
   * be in line with the names used in the openModeller library */
  void setModelRasterFormat(QString theFormat);
  /** Define the layerset that will be used for creationing this model.
   * @param theLayerSet An OmgLayerSet that contains layers and mask for model creationing.
   */
  void setCreationLayerSet(OmgLayerSet theLayerSet);
  
  /** Set the algorithm for this model */
  void setAlgorithm(OmgAlgorithm theAlgorithm);
  /** Set the completion state for this model  */
  void setCompleted(bool theFlag);
  /** Set the error state for this model */
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
  void setWorkDir(QString theDir);
  /** this is what is returned from om_create process. 
   * at the moment we just store this in a string member, but in the future
   * we ill parse it out and store the individual constituents 
   * as properties of the model object */
  void setModelDefinition(QString theDefinition);
  /** Some algs will return a serialised normalization definition
   * (since om lib supports different normalizer) */
  void setNormalizationDefinition(QString theDefinition);
  /** Set whether the model creation should use 
   * only one sample per distinct coordinate
   * @param LocalitiesFilterType Indicating the state of this setting (default to none)
   */
  void setLocalitiesFilterType(LocalitiesFilterType theFilterType=NO_FILTER);
  
  //accessors
 
  const QList<OmgProjection *> projectionList() const;
  const QList<OmgModelTest *> getModelTests() const;

  /** @see setTestCompartmentCount(int theCount) */
  int testCompartmentCount();
  QString coordinateSystem() const;
  QString coordinateSystemName() const;
  QString speciesFile() const;
  QString shapefileName() const;
  QString csvFileName() const;
  QString taxonName() const;
  OmgLayerSet creationLayerSet() const;
  QString creationMaskLayerName() const;
  QStringList creationLayerNames() const;
  OmgAlgorithm algorithm() const;
  bool isCompleted() const;
  bool hasError() const;
  QDateTime startDateTimeStamp() const;
  QDateTime endDateTimeStamp() const;

  /** The directory where the working files were stored 
   * for this model */
  QString workDir() const;
  /** Get the definition (as returned by libom) for this model */
  QString modelDefinition() const;
  QString normalizationDefinition() const;
  /** Whether the model creation should use a filter
   * on the samples to remove duplicates
   * @return LocalitiesFilterType Indicating the state of this setting
   */
  LocalitiesFilterType localitiesFilterType() const;
  OmgLocalities localities() const;
  QString modelLog() const;

  
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
  bool fromXml_108(const QString theXml);
  /** Backwards compatibility support for reading openModellerDesktop 
  * xml from versions Less Than or Equal to v 1.0.7. It will create a single 
  * projection in the projections list since v <= 1.0.7 did not support 
  * multirojections.
  * @return falso if the xml could not be parsed properly
  */
  bool fromXml_LTE107(const QString theXml);

  /** Parse the projections out of this model's xml
   * @return true on success
   */
  bool projectionsFromXml(QDomElement &theRootElement);
  /** This is a special case serialiser needed for when preparing an xml representation
   * of an model to send to the openmodeller library (or openmodeller web service
   * interface).
   * The xml produced will include no ancilliary meta information - only the essentials
   * needed for the model. 
   * @return a string containing the model creation request xml representation or if something 
   * went wrong an empty string.
   */
  QString toModelCreationXml() const;

  QString toSerializedModelXml() const;

  QString elementToXML(const QDomElement element);
  QString attributesToXML(const QDomNamedNodeMap attributes);
  
  /** A helper function for getting a CSV representation of the samples (sites plug environmental values)
   * @param theDelimter - A QString containing the required delimiter 
   * @param theSuppressHeaderFlag - A bool indicating whether to write a header row or not. Defaults to true.
   * @return A QString holding the CSV matrix
   */
  QString localitiesToCsv(QString theDelimiter="\t", bool theSuppressHeaderFlag=true) const;

  /** A helper method to easily write a shapefile to disk.
  * @param theFileName - the filename to be created or overwritten
  * @return bool - false if the file could not be written
  */
  bool toShapefile(QString theShapefileName) const;

  /** Perform post processing tasks on the model.
      This will generate a pseudocolor preview image,
      csv localities file and other products from the
      model output.
      @param theForceGraphRenderFlag rendering graphs in a non gui context can
      cause a crash
      @return bool true on success
      */
  bool postProcess(bool theForceGraphRenderFlag=false);

  /** Add a new projection to the model.
   * @param a pointer to an OmgProjection. Note the pointer will 
   * now be owned by the OmgModel that the projection is added to.
   */
  void addProjection(OmgProjection * thepProjection);
  /** The number of presence localities. */
  int presenceCount() const;
  /** The number of presence localities. Any localities 
   * that were excluded due to masking or spatial / environmental 
   * uniqueness filtering.*/
  int usablePresenceCount() const;
  /** The number of absence localities. */
  int absenceCount() const;
  /** The number of absence localities. Any localities 
   * that were excluded due to masking or spatial / environmental 
   * uniqueness filtering.*/
  int usableAbsenceCount() const;

  void splitLocalities(float trainingProportion);
  
 public slots:
  /** Append a message to the model log */
  void appendToLog(QString theMessage);
  /** Set the log to the provided message (overwriting any data that was 
   * previously there */
  void setLog(QString theLog);
  /** Get the average roc score for all the independent tests.
   * If no independent tests were carried out, the figure returned 
   * will be for the roc score of the original training data 
   * single roc test.
   * @return double representing the average roc score.*/
  double averageRocScore() const;
  /** Get the average accuracy score for all the independent tests.
   * If no independent tests were carried out, the figure returned 
   * will be for the stats of the original training data 
   * single test.
   * @return double representing the average roc score.*/
  double averageAccuracy() const;
#if WITH_QWT
  void createSummaryQwtRocGraph() const;
#endif
private:

  //!todo - make a class to represent a coordsys rather
  QString mCoordinateSystem;
  QString mCoordinateSystemName;
  QString mSpeciesFile;
  QString mTaxonName;
  OmgLayerSet mCreationLayerSet;
  OmgAlgorithm mAlgorithm;
  bool mCompletedFlag;
  bool mErrorFlag;
  //this type is defined at the bottom of omglocality.h
  OmgLocalities mLocalities;

  QDateTime mStartDateTimeStamp;
  QDateTime mEndDateTimeStamp;
  QString mShapefileName;
  QString mCsvFileName;
  QString mWorkDir;
  QString mModelDefinition;
  QString mNormalizationDefinition;
  QString mModelLog;
  LocalitiesFilterType  mLocalitiesFilterType;
  QList<OmgProjection *> mProjectionList;
  /** The adapter that will be used to run the models */
  OmgModellerPluginInterface * mpModellerPlugin;
  int mTestCompartmentCount;
  /** @see setTrainingProportion( float proportion ) */
  float mTrainingProportion;
  QList<OmgModelTest *> mModelTests;
};

#endif //OMGMODEL_H
