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
#ifndef OMGMODELLERWEBSERVICESPLUGIN_H
#define OMGMODELLERWEBSERVICESPLUGIN_H

//qt includes
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#include "omgalgorithm.h"
#include "omgmodel.h"
#include "omgprojection.h"
#include "omgmodellerplugininterface.h"
//gsoap includes
#include "stdsoap2.h"

/**
  The OmgModellerWebServicesplugin implements a soap client for the
  openModeller Web Services Interface standard API. To better understand the
  implementation code in this class, it is suggested to look at soapClient.cpp
  to see the native c++ type names, and om_soap_server.cpp for the gsoap name
  mappings.  These files are available in the om/src/soap directory in the
  openModeller cvs repository.
  @author Tim Sutton
*/
class OMG_PLUGIN_EXPORT OmgModellerWebServicesPlugin : public QObject, public OmgModellerPluginInterface
{
  Q_OBJECT;
  Q_INTERFACES(OmgModellerPluginInterface);
  public:
  OmgModellerWebServicesPlugin( QObject* parent=0);
  ~OmgModellerWebServicesPlugin();
  /** Create an initial model definition
   *  @param OmgModel pointer to the model object that will be used as a basis for the mode
   */
  QString createModel(OmgModel * thepModel);
  /** Test the latest model created
   *  @param &OmgModel Reference to the model object
   *  @param &OmgModelTest Reference to the test object
   */
  void testModel(OmgModel * thepModel, OmgModelTest * thepTest);
  /** Project a model into a climate dataset.
   * @NOTE the model passed in as a parameter must have a valid model definition
   * @param OmgProjection pointer to the projection object that will be used to
   * project this model.
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
  /** Get a list of layers by recursively searching a directory heirachy and testing if each
   * file is a gdal loadable file or not. The actual implementation of how this is done is up to the 
   * plugin - remote services may have a preconfigured list of available layers that is
   * simply returned without any filesystem scanning.
   * @param baseDirectory This is the directory from which to start scanning. This parameter may be
   * ignored by the plugin if the remote resource does not support user specified base directories.
   * In the case of the web services plugin the parameter is ignored.
   * @return QString An xml document providing teh list of available layers.
   */
   const QString getLayers(QString theBaseDir);

   /** Get the user friendly name for this plugin
    *  @return QString - the name of this plugin
    **/
   const  QString getName() { return QString("Web Services Modeller Plugin"); };

  //
  // These are public for use by unit tests
  //

   /** A helper method to convert a std char array into a wchar array */
   wchar_t* convertToWideChar( const char* p );
   struct soap * getSoapConnection();
  public slots:

  private slots:
  
  signals:
  void error(QString);
  /** Notifies any listeners of an updated log message from openmodeller. 
   * @param theMessage The message received by the log callback
   */
  void logMessageUpdated(QString theMessage);
  /** Notifies any listeners that the currently running model is finished 
   * @param theFilename of the completed model. If the model was 
   * executed on a remote server the plugin will ensure that it is copied 
   * to the local file system first.
   * 
   **/
  void modelDone(QString theFileName);
  /** Notifies any listeners how far the current model has progressed
   * (progress is expressed as a percentage) 
   * @param theProgress 
   * */
  void modelProgressUpdate(int theProgress);
  private:
  /**
   * Private method called by any ctor on initial start up. 
   * It will go and find the algorithm list etc.
   */
  bool initialise();
  int mModelProgress;
  int mMapProgress;


};

#endif //OMGMODELLERWEBSERVICESPLUGIN_H

