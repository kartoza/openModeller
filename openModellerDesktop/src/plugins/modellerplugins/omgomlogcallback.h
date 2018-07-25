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
#ifndef QOMGOMLOGCALLBACK_H
#define QOMGOMLOGCALLBACK_H

//om includes
#include <openmodeller/Log.hh>

// std includes
#include <string>

// Qt includes
#include <QTextStream>
class QString;

/** OmgLogCallback is  a special purpose class to proved a Qt signal
* style interface to the openModeller Log callback mechanism.
*
* @author Tim Sutton 2005
*/

class OMG_PLUGIN_EXPORT OmgOmLogCallback :  public QObject, public Log::LogCallback 
{
  Q_OBJECT;
public:
  OmgOmLogCallback();
  OmgOmLogCallback(QObject * parent);
  virtual ~OmgOmLogCallback();
  /** Constructor for log call back required to fit the om callback pattern.
  * @note Although the call back needs a text stream in the ctor, 
  * all we want is to get the msg.c_str() 
  * from the logger, so having the textstream is just so that we fit the 
  * g_log callback model, but its not actually used anywhere
  * @param QTextStream a text stream - not actually used anywhere!
  */
  OmgOmLogCallback( QTextStream& theTextStream );
  /** Overloading the () operator is required because om uses this approach
  * to push out log messages. We will simplyg capture the passed in message
  * and emit it as a QString that any connected classes can then use.
  * @note the log level parameter passed in by om is ignored at this time.
  * @param Log::Level the log level (ie urgency) of the message
  * @param string the message passed to us from om
  * @return void 
  */
  void operator()(Log::Level theLevel, const std::string& theMessage );
  /** Set the globally unique identifier for the model that is associated with this callback */
  void setModelGuid(QString theModelGuid);

signals:
  /** Signal passed on from here to other qt derived classes with the log message from om 
   *  @param theModelGuid - guid of the model that this message is associated with
   *  @param theLogMessage - the message from the log callback
   * */
  void omLogMessage(QString theModelGuid, QString theLogMessage);
  /** Signal passed on from here to other qt derived classes with the log ERROR  message from om 
   *  @param theModelGuid - guid of the model that this message is associated with
   *  @param theLogMessage - the message from the log callback
   */
  void omLogError(QString theModelGuid, QString theLogMessage);

private:

  /** The globally unique identifier for the model that this 
   * callback wrapper is associated with */
  QString mModelGuid;

};
#endif //OMGOMLOGCALLBACK_H
