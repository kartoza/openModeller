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
#ifndef OMGPREANALYSIS_H
#define OMGPREANALYSIS_H

#include "omgmodel.h"
#include "omgmodellerplugininterface.h"
#include "omgserialisable.h"
#include "omgguid.h"
#include <omgpreanalysisalgorithm.h>
#include <omgpreanalysisalgorithmset.h>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/Sampler.hh>

//qt includes
#include <QVector>
#include <QList>
#include <QObject>
#include <QMap>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>


class OMG_LIB_EXPORT OmgPreAnalysis : public QThread, public OmgSerialisable, public OmgGuid
{
  Q_OBJECT;

public:
  OmgPreAnalysis();
  ~OmgPreAnalysis();
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
   *  @param theWorkDir - a QString with full path to the dir. Dir must exist!
   * */
  void setWorkDir(QString theWorkDir);
  /** Start running the experiment */
  void run();
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
  /** Test and set to see if the pre-analysis is completed or not.
   * @return false if any models are not yet completed and have no errors.
   */
  bool isCompleted();
  void setCompleted(bool newCompleted);

  /** Save the pre-analysis */
  void save();

  /** get the name of the Pre-Analysis Algorithm */
  QString algorithmName();
  /** set the name of the Pre-Analysis Algorithm */
  void setAlgorithmName(QString theName);

  /** get the name of the taxon */
  QString taxonName();
  /** set the name of the taxon */
  void setTaxonName(QString name);

  void setPreAnalysisAlgorithm(PreAlgorithm *theAlgorithm);
  void setOccurrenceDataFile(QString occurrenceDataFile);
  void setLayerSet(QString layerSet);
  void configureSamplePtr();
  template<typename ObjectT>
  void setParameterValue(QString &parameterName, ObjectT value) { params->store(parameterName.toStdString(), value); }
  template<typename ObjectT>
  void getParameterValue(QString &parameterName, ObjectT &value) { params->retrieve(parameterName.toStdString(), value); }
  template<typename ObjectT>
  void getParameterValue(std::string parameterName, ObjectT &value) { params->retrieve(parameterName, value); }
  template<typename ObjectT>
  void getLayerResult(QString layerId, QString parameterName, ObjectT &value) 
        { PreParameters result; mAlgorithm->getLayerResult(layerId.toStdString(), result); result.retrieve(parameterName, value);}
  template<typename ObjectT>
  void getLayerResult(QString layerId, std::string parameterName, ObjectT &value) 
        { PreParameters result; mAlgorithm->getLayerResult(layerId.toStdString(), result); result.retrieve(parameterName, value);}
  /** Implments the serialisable interface method to reinstate a pre-analysis
   */
  bool fromXml(const QString theXml) ;
  /** This method is required for the serialiseable interface.
   */
  QString toXml() const ;
signals:
  /** Notify all listeners that a pre-analysis completed (including
      all post processing) 
      @param QString - guid of completed model.*/
  void preAnalysisCompleted();
  /** Notify listeners of a new log message */
  void logMessage(QString);

private:
  /** A helper function used by the reporting functions to get
   * stats for the experiment. */
  QString getStatsHtml();
  /** A name for this experiment */
  QString mName;
  /** A description for this experiment */
  QString mDescription;
  /** The working dir for the experiment. This will be set when the experiment
   * is restored or created and not serialised to disk as part of the
   * experiment. */
  QString mWorkDir;

  PreAlgorithm *mAlgorithm;
  QString mOccurrenceDataFile;
  QString mLayerSet;
  QStringList layersFileNames;
  QString mMaskLayerName;
  QString mAlgorithmName;
  QString mTaxonName;
  PreParameters *params;
  SamplerPtr sampler;
  bool mCompleted;
};

#endif //OMGPREANALYSIS_H
