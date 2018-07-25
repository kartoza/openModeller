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

#ifndef OMGEXPERIMENTPRINTER_H
#define OMGEXPERIMENTPRINTER_H


//qt includes
#include <QVector>
#include <QList>
#include <QObject>
#include <QMap>
#include <QMutex>
#include <QThread>
#include <QPrinter>
#include <QWaitCondition>


class OmgExperiment;

/** \ingroup library
* \brief An experimentprinter prepares an exeriment print job in a separate 
* thread since the process can be time consuming. Print destination can
* be to pdf file or print device.
* @author Tim Sutton, 2005
*/
class OMG_LIB_EXPORT OmgExperimentPrinter : public QThread
{
  Q_OBJECT;

public:
  OmgExperimentPrinter();
  ~OmgExperimentPrinter();

  public slots:
  /** A flag indicating if the experimentprinter  is running
   * or stopped / completed.
   * @return boolean - false if the experimentprinter  is stopped / 
   * completed or true if it is currently running.
   */
  bool isRunning();
  /** Start running the experiment */
  void run() ;
  /** Stop running anymore models */
  void stop();
  /** Set the output mode - pdf, ps or printer 
   * @param thePrinter QPrinter passed by value so the calling fn can
   * close */
  void setPrinter(QPrinter &thePrinter);
  /** Set the experiment that we are trying to print */
  void setExperiment(OmgExperiment * theExperiment);

signals:
  /** signal for showing how much work needs to be done ... */
  void maximum(int);
  /** signal for showing current progress  */
  void progress(int);
  /** signal to show we are done */
  void done();
private:
  bool mRunningFlag;
  OmgExperiment * mpExperiment;
  QPrinter mPrinter;
  QMutex mMutex;
};
#endif //OmgExperimentPrinter
