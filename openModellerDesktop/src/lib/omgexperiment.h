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
#ifndef OMGEXPERIMENT_H
#define OMGEXPERIMENT_H

#include "omgmodel.h"
#include "omgmodellerplugininterface.h"
#include "omgserialisable.h"
#include "omgguid.h"

//qt includes
#include <QVector>
#include <QList>
#include <QObject>
#include <QMap>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

/** \ingroup library
* \brief An experiment represents a collection of models that need to be processed.
* Rather than setting up models and running them individually, you can add them
* to the experiment, which (where possible) will parelelise the process, keep 
* track of overall experiment progress and perform other house keeping jobs for you.
* @note This class inherits QThread
* @author Tim Sutton, 2005
*/
class OMG_LIB_EXPORT OmgExperiment : public QThread, public OmgSerialisable, public OmgGuid
{
  Q_OBJECT;

public:
  OmgExperiment();
  ~OmgExperiment();
  /** get the name of this experiment */
  QString name();
  /** set the name of this experiment */
  void setName(QString theName);
  /** get the description of this experiment */
  QString description();
  /** set the description of this experiment */
  void setDescription(QString theDescription);
  /** get the workDir of this experiment */
  QString workDir();
  /** Set the workDir of this experiment 
   *  if the experiment has any models associated
   *  with it their work dirs will be updated
   *  too. Any new models added to the experiment will
   *  have their workdir set to this value.
   *  @param theWorkDir - a QString with full path to the dir
   * */
  void setWorkDir(QString theWorkDir);
  /** Get the current status of the experiment.
   *  Example responses are:
   * "Creating model definition for Acacia mearnsii"
   * "Projecting model for Acacia mearnsii"
   * "Experiment cancelling..."
   * etc.
   * @return a String with the experiment status.
   */
  QString currentStatus() {return mCurrentStatus;};
  /** Reset the experiement - mark all models reset
   * and clear all internal vars.
   */
  void reset();
  /** A flag indicating if the experiment is running
   * or stopped / completed.
   * @return boolean - false if the experiment is stopped / 
   * completed or true if it is currently running.
   */
  bool isRunning();
  /** Add a new model to the experiment queue */
  void addModel(OmgModel  * thepModel) ;
  /** Get a model from experiment given its number. 
   * Be careful
   * about modifying models while the experiment is
   * running. Doing so can put the model into an 
   * undefined state.
   * @return theModel if it exists or NULL */
  OmgModel * getModel(int theModelNo);

  /** Overloaded convenience function to Get a model 
   * from experiment given its guid. Applies a mutex
   * lock while searching for the model. Be careful
   * about modifying models while the experiment is
   * running. Doing so can put the model into an 
   * undefined state.
   * @param the guid of the model to find
   * @return An * OmgModel or NULL if no match found. */
  OmgModel * getModel(QString theGuid);
  
  /** Get a model from experiment given a label (species name?) and 
   * the algorithm name.
   * @return theModel if it exists or NULL */
  OmgModel * getModel(QString theLabel, QString theAlgorithm);

  /** Get a projection from the experiment given its guid.
   * @return a const pointer of the projection or NULL if no match found
   */
  OmgProjection const * getProjection(QString theGuid) const;
  /** Start running the experiment */
  void run();
  /** Stop running anymore models */
  void stop();
  /** Then number of models to be run in the experiment */
  int count();
  /** Clear the model queue for the experiment */
  void clear() ;
  /** Remove a model from the experiment given its Guid */
  void removeModel(QString theGuid);
  /** Remove all models that match the algorithm Name */
  void removeAlgorithm(QString theName);
  /** Remove all models that match the taxon Name */
  void removeTaxon(QString theName);
  /** Remove all completed models from the queue */
  void removeCompleted() ;
  /** Get the active modeller plugin */
  OmgModellerPluginInterface * modellerPlugin();
  /** Tell the experiment which modeller adapter instance to use */
  void setModellerPlugin(OmgModellerPluginInterface * theAdapter);
  /** Return a string representation of the whole experiment
   * mainly intended for debugging purposes */
  const QString toString();
  /** Find out if the experiment abort flag has been set. */
  bool isAborted();
  /** Create an html formatted sumary of the experiment
   * and save it to disk.
   * @param bool flag indicating whether to force 
   * recreating the report or whether to just 
   * return the existing one if it was made previously.
   * If the file does not exist the flag is ignored
   * and the report is created anyway.
   * By default this flag is false.
   * @return QString containing the full file name and 
   * path to the summary report.
   * */
  QString toSummaryHtml(bool theForceFlag=false);
  /** Create an html formatted sumary of the experiment
   * and return it as a string for use in printing. 
   * The paths to image files will be absolute so that
   * the print properly in the QTextDocument..
   * @param bool flag indicating whether to force 
   * recreating the report or whether to just 
   * return the existing one if it was made previously.
   * If the file does not exist the flag is ignored
   * and the report is created anyway.
   * By default this flag is false.
   * @return QString containing the full html 
   * of the summary report.
   * */
  QString toPrintHtml(bool theForceFlag=false);
  /** A helper function to make algorithm summary graphs 
    * @param theWorkDir - the workingdir for the output graphs
    */
  void makeAlgorithmSummaryGraphs(QString theWorkDir); 
  
  /** Save the exeriment */
  void save();

  /** Get a list of unigue Taxon names for this experiment
   * @return QStringList a list of unique taxon names.
   */
  QStringList taxonNames();
  /** Get a list of unigue algorithm names for this experiment
   * @return QStringList a list of unique algorithm names.
   */
  QStringList algorithmNames();
  
  /** Test to see if the experiment is completed or not.
   * @return false if any models are not yet completed and have no errors.
   */
  bool isCompleted();
  //
  // implement the serialisable interface:
  //

  /** This method is required for the serialiseable interface. It will 
   * encapsulate *all* model properties so that its complete state can
   * be serialised and later deserialised.
   * @see toModelCreationXml()
   */
  QString toXml() const ;
  /** Implments the serialisable interface method to reinstate a model.
   * It will try to populate all members of this class and fail gracefully when
   * they are not available (typically by leaving the member tagged as [Not Set]
   */
  bool fromXml(const QString theXml) ;

public slots:
  /** Set the abort flag for this experiment. The experiment will end as
   * soon as the current model is done or sooner if the adapter allows that. */
  void abort();
signals:
  /** Notify all observers of the experiment size */
  void experimentMaximum(int);
  /** Notify all observers how far the total experiment has progressed */
  void experimentProgress(int);
  /** Notify all listeners how far the creation of the current model has progressed */
  void modelCreationProgress(int);
  /** Notify all listeners how far the projection of the current model has progressed */
  void modelProjectionProgress(int);
  /** Notify listeners of a new log message */
  void logMessage(QString);
  /** Notify all observers that the experiment has stopped */
  void experimentStopped();
  /** Notify all listeners that a model completed (including
      all post processing) 
      @param QString - guid of completed model.*/
  void modelCompleted(QString);
private:
  /** A helper function used by the reporting functions to get
   * stats for the experiment. */
  QString getStatsHtml();
  /** A helper function used by the reporting functions to get
   * graphs html for the experiment */
  QString getGraphsHtml(QString theImagePrefix ="", bool theForceFlag =false);
  /** A helper function to add a new record to the
   * map holding data for a summary graph. Typically
   * used by makeAlgorithmSummaryGraphs. If no key exists
   * in theMap for theAlgorithmName, one will be added
   * and a new associated vector will be created.
   * @see makeAlgorithmSummaryGraphs
   * @param &theMap - a map (passed by reference) containing
   *                 algorithm names for its keys and
   *                 QLists for its values. The list
   *                 in turn contains double
   *                 where the value in the list
   *                 art the y axis value that is to be
   *                 to be graphed. This functions purpose
   *                 is to append a new entry to this 
   *                 list.
   *                 QMap<QString,QList <double> > 
   * @param theAlgorithmName - a string used to identify
   *                 which vector the value should be added
   *                 to.
   * @param theValue - the value to be added to the vector
   *                 associated with theAlgoritithmName.
   */
  void addValueToSummaryGraph(
    QMap<QString,QList <double > > & theMap,
    QString theAlgorithmName,
    double theValue); 
  /** A helper function to generate a graph from a QMap.
  * @see makeAlgorithmSummaryGraphs
  * @param &theMap - a map (passed by reference) containing
  *                 algorithm names for its keys and
  *                 QLists for its values. The list
  *                 in turn contains double
  *                 where the value in the list
  *                 art the y axis value that is to be
  *                 to be graphed. This functions purpose
  *                 is to render the graph as a graphic.
  *                 QMap<QString,QList <double> > 
  * @param theWorkDir - a directory where the generated image
  *                 should be placed after the graph is generated.
  * @param theImageSuffix - some text to be appended to each image
  *                 the first part of the image name will be the
  *                 Key of the QMap.
  *                 e.g. somekeyval_<theImageSuffix>.png
  */
  void summaryMapToGraph( QMap<QString,QList < double > > &theMap, 
           QString theWorkDir, 
           QString theImageSuffix );

  //
  //
  //  Members ...
  //
  //
  QMutex mMutex;
  QWaitCondition mCondition;
  /** A name for this experiment */
  QString mName;
  /** A description for this experiment */
  QString mDescription;
  /** The container for all the models in the queue */
  QVector <OmgModel * > mModelVector;
  /** The position in the queue of the last model that was executed */
  int mLastPos;
  /** The adapter that will be used to run the models */
  OmgModellerPluginInterface * mpModellerPlugin;
  /** A flag that gets set when a request has been made to cancel the experiment
   * run. The experiment will abort at the earliest convenient time, depending
   * on the modeller adapter in use. */
  bool mAbortFlag;
  /** The working dir for the experiment. This will be set when the experiment
   * is restored or created and not serialised to disk as part of the
   * experiment. */
  QString mWorkDir;
  /** A string with the current status of the experiment. This property is
   * read only and not serialised. Example responses are:
   * "Creating model definition for Acacia mearnsii"
   * "Projecting model for Acacia mearnsii"
   * "Experiment cancelling..."
   * etc.
   */
  QString mCurrentStatus;
  /** A flag to indicate if the experiment is currently running */
  bool mRunningFlag;
};

#endif //OMGEXPERIMENT_H
