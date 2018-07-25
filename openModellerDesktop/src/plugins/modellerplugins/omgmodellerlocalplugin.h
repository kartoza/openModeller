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
#ifndef OMGMODELLERLOCALPLUGIN_H
#define OMGMODELLERLOCALPLUGIN_H

//qt includes
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTextStream>

//#include "qlogcallback.h"
#include "omgalgorithm.h"
#include "omgmodel.h"
#include "omgprojection.h"
#include "omgomlogcallback.h"
#include "omgpluginmessenger.h"

#include <omgmodellerplugininterface.h>
class OpenModeller;
class QDomDocument;
class QDomElement;

/**
@author Tim Sutton
*/
  /** Callback function passed to oM to keep track of model creation  progress
   * @param theProgress Passed to this method by om, 
   * telling us how far through the job we are
   * @param thePlugin We need to pass an plugin to om in the setModelCallback 
   * call of the initialise function. This OmgOmPlugin is then 
   * returned to us by the callback.
   * By default its 'this' object, and it will relay the progress 
   * update over to setProgress, which in turn
   * will emit a signal to any clients of this class. 
   * I know this is a bit circuituos but we dont have too much 
   * choice!
   */

  void modelCallback( float theProgress, void *thePlugin );

  /** Callback function passed to oM to keep track of map projection progress
   * @param theProgress Passed to this method by om, telling us how far 
   * through the job we are
   * @param thePlugin We need to pass an plugin to om in the setMapCallback
   * call of the initialise function. This OmgOmPlugin is then returned to 
   * us by the callback.
   * By default its 'this' object, and it will relay the progress 
   * update over to setProgress, which in turn
   * will emit a signal to any clients of this class. I know this is 
   * a bit circuituos but we dont have too much 
   * choice!
   */

  void mapCallback( float theProgress, void *thePlugin );
  
class OMG_PLUGIN_EXPORT OmgModellerLocalPlugin : public QObject, public OmgModellerPluginInterface
{
  Q_OBJECT;
  Q_INTERFACES(OmgModellerPluginInterface);
  public:
  OmgModellerLocalPlugin( QObject* parent=0);
  ~OmgModellerLocalPlugin();
  /** Create an initial model definition
   *  @param &OmgModel Reference to the model object that
   *  will be used as a basis for the model
   */
  QString createModel(OmgModel * thepModel);
  /** Test the latest model created
   *  @param &OmgModel Reference to the model object
   *  @param &OmgModelTest Reference to the test object
   */
  void testModel(OmgModel * thepModel, OmgModelTest * thepTest);
  /** Project a model into a climate dataset.
   * @NOTE the projection passed in as a parameter must have a valid model
   * definition
   * @param OmgProjection the projection object 
   * that will be used to project this model.
   */
  void  projectModel(OmgProjection * thepProjection);

  /**
   * Get a list of the algorithm names.
   * Use getAlgorithmId with one of the returned names to find out an algs id
   * @return QStringList A String List of the algorithm names
   */
  const QStringList getAlgorithmList();
  /**
  * Get a collection of algorithm objects in the form of an AgorithmSet.
  * The collection represents every algorithm that is available from the
  * plugin implementing this interface
  * @return OmgAlgorithmSet - a set of algorithms
  */
  const OmgAlgorithmSet getAlgorithmSet();
  /** Get an algorithm given its id.
   * @return OmgAlgorithm An empty algorithm will be 
   * returned if the algorithm id could not be found.
   */
  const OmgAlgorithm getAlgorithm( QString theAlgorithmId );
  /** Mutator for creation progress variable
   * Mainly intended to be used by callbacks.
   * Will emit a modelCreationProgressUpdate signal if the
   * new value differs from the old one.
   * @param int theProgress
   */
   void setCreationProgress (int theProgress);
  /** Mutator for projection progress variable
   * Mainly intended to be used by callbacks.
   * Will emit a modelCreationProgressUpdate signal if the
   * new value differs from the old one.
   * @param int theProgress
   */
   void setProjectionProgress (int theProgress);

  /** Get a list of layers by recursively searching a directory heirachy and testing if each
   * file is a gdal loadable file or not. The actual implementation of how this is done is up to the 
   * plugin - remote services may have a preconfigured list of available layers that is
   * simply returned without any filesystem scanning.
   * @param baseDirectory This is the directory from which to start scanning. This parameter may be
   * ignored by the plugin if the remote resource does not support user specified base directories.
   * In the case of the local modeller plugin the parameter is accepted.
   * @return QString An xml document providing teh list of available layers.
   */
   const QString getLayers(QString theBaseDir);

   /** Get the user friendly name for this plugin
    * @return QString - the name of this plugin 
    */
  const  QString getName() { return QString("Local Modeller Plugin"); };

  public slots:

  private slots:
  /** Slot for recordng messages from the plugin messenger into a string
   * that can then be appended to the model log.
   * @param theGuid of the model as passed by the messenger
   * @param theMessage
   * @NOTE the first parameter is silently dropped but its needed to match the signal
   */
  void appendToLog(QString theGuid,QString theMessage);

  private:
  /**
   * Private method called by any ctor on initial start up. 
   * It will go and find the algorithm list etc.
   */
  bool initialise();
  OpenModeller * mpOpenModeller;
  OmgOmLogCallback * mpLogCallBack;
  QTextStream mTextStream;
  int mModelProgress; //as %
  int mMapProgress; // as %
  /** Globally unique id of the model currently being executed */
  QString mModelGuid;
  /** Log for the model currently being executed */
  QString mModelLog;

  /** A flag to see whether the Om Aglorithm Factory has already been initialised.
   * The factory should only be initialised once in 
   * the entire lifetime of you application.
   * If you do it more than once you will get duplicate 
   * entries when you read the alg list.
   * Because of this we use teh flag below to determin 
   * if the Factory has previously been initialised.
   * @NOTE C++ does not guarantee the initialised 
   * state of a variable, so variable is initialised 
   * outside the class (see top of accompanying .cpp file).
   */
  static bool mAlgFactoryStartedFlag;

  /** This is used by the om callbacks to let 
   * us know of model creation progress changes
   * @param theProgress how far the task has progressed.
   */
  void setMapProgress(int theProgress);

  /** This is used by the om callbacks to let us know of map projection progress changes
   * @param theProgress how far the task has progressed.
   */
  void setModelProgress(int theProgress);
  

};

#endif
