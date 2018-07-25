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

#ifndef OMGPLUGINMESSENGER_H
#define OMGPLUGINMESSENGER_H

//QT Includes
#include <QObject>
#include <QString>

/** \ingroup library
 * \brief A delegate class for passing messages between plugins and the application.
 * qt plugins should not inherit qobject and thus dont support signals and
 * slots. Thus we need this messenger delegate to connect signals from the
 * plugin to slots in the parent app. In fact the qtplugins inheriting qobject do
 * work under lin and mac but they dont work under win.
 */
class OMG_LIB_EXPORT OmgPluginMessenger : public QObject
{
Q_OBJECT;
public:
  OmgPluginMessenger(QObject *parent=0); 
  virtual ~OmgPluginMessenger();
public slots:
  /** Delegate for the error  signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theError The error message to be emitted as a signal.
   *  @return void
   *  @see error(QString)
   */
  void emitError(QString theError);
  /** Delegate for the message signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theMessage The message to be emitted as a signal.
   *  @return void
   *  @see message(QString)
   */
  void emitMessage(QString theMessage);
  /** Delegate for the modelError signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theModelId The model id that the message is associated with
   *  @param theMessage The message to be emitted as a signal.
   *  @return void
   *  @see modelError(QString,QString)
   */
  void emitModelError(QString theModelGuid,QString theMessage);
  /** Delegate for the modelMessage signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theModelId The model id that the message is associated with
   *  @param theMessage The message to be emitted as a signal.
   *  @return void
   *  @see modelMessage(QString,QString)
   */
  void emitModelMessage(QString theModelGuid,QString theMessage);
  /** Delegate for the modelDone signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theModelId The model id that the message is associated with
   *  @return void
   *  @see modelDone(int)
   */
  void emitModelDone(QString theModelGuid);
  /** Delegate for the modelCreationProgress signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theModelId The model id that the message is associated with
   *  @param theProgress The current progress of the model creation (0-100)%.
   *  @return void
   *  @see modelCreationProgress(int,QString)
   */
  void emitModelCreationProgress(QString theModelGuid,int theProgress);
  /** Delegate for the modelProjectionProgress signal (calling this method will 
   *  cause that signal to be emitted)
   *  @param theModelId The model id that the message is associated with
   *  @param theProgress The current progress of the model projection (0-100)%.
   *  @return void
   *  @see modelCreationProgress(int,QString)
   */
  void emitModelProjectionProgress(QString theModelGuid,int theProgress);
  /** Delegate for refresh signal to push a refresh instruction out to any listeners. */
  void emitRefresh();

  //these next slots are intended for use with scraper plugins

  /** Delegate for the fileWritten signal (calling this method will cause that signal to be
   * emitted). Notifies any listeners that a scraper plugin has written a file.
   * @param theShapeFile - name of the shapefile that was written
   * @param theTextFile - name of the text file that was written
   * @param theTaxonName - name of the taxon that is associated with these files
   * @param theCount - the number of locality recs written
   * @return void
   */
  void emitFileWritten(QString theShapeFile, QString theTextFile, QString theTaxonName,int theCount);
  /** Delegate for the fileNotWritten signal (calling this method will cause that signal to be
   * emitted.) Notifies any listeners that a scraper plugin has failed to write a file.
   * @param theTaxonName - name of the taxon that is associated with this failure
   * @return void
   */
  void emitFileNotWritten(QString theTaxonName);
signals:
   /** Notifies any listeners of an error message.
   *  @param theError The error message to be emitted as a signal.
   *  @return void
   */
  void error(QString);
   /** Notifies any listeners of message.
   *  @param theError The message to be emitted as a signal.
   *  @return void
   */
  void message(QString);
  /** Notifies any listeners of an updated log error from openmodeller. 
  * @param theMessage The message received by the log callback
  * @param theModelGuid Global identifier of the mode associated with this message
  */
  void modelError(QString theModelGuid,QString theMessage);
  /** Notifies any listeners of an updated log message from openmodeller. 
  * @param theMessage The message received by the log callback
  * @param theModelGuid Global identifier of the mode associated with this message
  */
  void modelMessage(QString theModelGuid,QString theMessage);
  /** Notifies any listeners that the currently running model is finished 
   * @param theModeId - id of the model that was completed
   * 
   **/
  void modelDone(QString theModelGuid);
  /** Notifies any listeners how far the current model CREATION has progressed
   * (progress is expressed as a percentage) 
   * @param theProgress 
   * @see setCreationProgress(int theProgress)
   * */
  void modelCreationProgress(QString theModelGuid,int theProgress);
  /** Notifies any listeners how far the current model PROJECTION has progressed
   * (progress is expressed as a percentage) 
   * @param theProgress 
   * @see setProjectionProgress(int theProgress)
   * */
  void modelProjectionProgress(QString theModelGuid,int theProgress);
  /** Notifies any listeners they should refresh their display */
  void refresh();
  
  // These next signals are intended for use with scraper plugins

  /** Notifies any listeners that a scraper plugin has written a file.
   * @param theShapeFile - name of the shapefile that was written
   * @param theTextFile - name of the text file that was written
   * @param theTaxonName - name of the taxon that is associated with these files
   * @param theCount - the number of locality recs written
   * @return void
   */
  void fileWritten(QString theShapeFile, QString theTextFile, QString theTaxonName,int theCount);
  /** Notifies any listeners that a scraper plugin has failed to write a file.
   * @param theTaxonName - name of the taxon that is associated with this failure
   * @return void
   */
  void fileNotWritten(QString theTaxonName);
};

#endif //OMGPLUGINMESSENGER
