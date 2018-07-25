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
#ifndef OMGOMMODELLERPLUGININTERFACE_H
#define OMGOMMODELLERPLUGININTERFACE_H

//qt includes
#include <QPluginLoader>//needed for the qt plugin macros
//#include "qlogcallback.h"
//forward declarations
#include "omgalgorithm.h"
#include "omgalgorithmset.h"
#include "omgpluginmessenger.h"
#include <QString>
#include <QStringList>
#include <QMap>
class OmgModel;
class OmgModelTest;
class OmgProjection;

/** \ingroup library
 * \brief An abstract base class for all modeller plugins. It defines
 * the interface that all modeller plugins must adhere to.
 * @author Tim Sutton
*/
class OMG_LIB_EXPORT OmgModellerPluginInterface 
{
public:
  virtual ~OmgModellerPluginInterface() ;
  /** Create an initial model definition
  *  @param OmgModel pointer to the model object that will be used as a basis for the mode
  *  @return QString The return xml from the model plugin.
  */
  virtual QString createModel(OmgModel * thepModel)=0;
  /** Test the latest model created
   *  @param &OmgModel Reference to the model object
   *  @param &OmgModelTest Reference to the test object
   */
  virtual void testModel(OmgModel * thepModel, OmgModelTest * thepTest)=0;
  /** Project a model into a climate dataset.
  * @NOTE the model passed in as a parameter must have a valid model definition
  * @param OmgModel pointer to the model object that will be used to project this model.
  */
  virtual void  projectModel(OmgProjection * thepProjection)=0;

  /**
  * Get a list of the algorithm names.
  * Use getAlgorithmId with one of the returned names to find out an algs id
  * @return QStringList A String List of the algorithm names
  */
  virtual const QStringList getAlgorithmList()=0;
  /**
   * Get a collection of algorithm objects in the form of an AgorithmSet.
   * The collection represents every algorithm that is available from the
   * plugin implementing this interface
   * @return OmgAlgorithmSet - a set of algorithms
   */
  virtual const OmgAlgorithmSet getAlgorithmSet()=0;
  /** Get a list of layers by recursively searching a directory heirachy and testing if each
   * file is a gdal loadable file or not. The actual implementation of how this is done is up to the 
   * plugin - remote services may have a preconfigured list of available layers that is
   * simply returned without any filesystem scanning.
   * @param baseDirectory This is the directory from which to start scanning. This parameter may be
   * ignored by the plugin if the remote resource does not support user specified base directories.
   * @return QString An xml document providing teh list of available layers.
   */
  virtual const QString getLayers(QString theBaseDir)=0;
  /** Get the name of this plugin instance - useful for displaying the plugin name in guis etc 
   * @return QString with a user friendly name e.g. 'Local Modeller Plugin'
   */
  virtual const QString getName()=0;
  /** Get a pointer to the plugin messenger instance associated with this plugin.
   * The pluginmessenger is used to relay signals between the application using the
   * plugin and the plugin itself. Ideally we would just make the plugin interface
   * inherit qobject and implement the signals directly in the interface, but this
   * approach only works on mac and linux at the moment so we resort to this slightly
   * more cumbersome appraoch.
   * @return OmgPluginMessenger *
  */
  virtual const OmgPluginMessenger * getMessenger() ;
  /** Parse a model returned from the modelling backend and extract the <model/> from the algorithm
   * @param theXml The xml as returned from the modeller engine. The xml should contain the <model>.
   * @return QString - containing just the model snipped out from the entire document.
   */
  virtual const QString getModelDefinition(QString theModelXml);

protected:  
  /** The default constructor is protected so that no besides inheriting classes
   * can use it! */ 
  OmgModellerPluginInterface() {};
  /** The plugin messenger instance associated with this plugin */
  OmgPluginMessenger mMessenger;
  
};


// Declare plugin interfaces for Qt plugin loader
Q_DECLARE_INTERFACE(OmgModellerPluginInterface, "org.openmodeller.OmgModellerPluginInterface/1.0")

#endif

