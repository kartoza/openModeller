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

#ifndef OMGEXPERIMENTPROGRESS_H
#define OMGEXPERIMENTPROGRESS_H

#include <ui_omgexperimentprogressbase.h>

//
// QT includes
//
#include <QDialog>
#include <QHash>
#include <QTimer>
class QString;
class OmgExperiment;
class QProgressBar;
class QResizeEvent;
class QTableWidgetItem;
 /** \ingroup gui
 * \brief A simple widget used to show the user the progress of their experiment.
 * @author Tim Sutton 2005
 * */

class OmgExperimentProgress : public QDialog, private Ui::OmgExperimentProgressBase

{
Q_OBJECT;

public :

  OmgExperimentProgress(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgExperimentProgress();
  /** Set the experiment that this dialog will be monitoring */
  void setExperiment(OmgExperiment * theExperiment);

public slots:
  void setModelCreationProgress (QString theModelGuid,int theProgress);
  void setModelProjectionProgress (QString theProjectionGuid,int theProgress);
  void setExperimentProgress (int theProgress);
  void setExperimentMaximum (int theMaximum);
  /** Usually used by the omgexperiment class to report general messages */
  void logMessage(QString theMessage);
  /** Usually receives mesages from modeller plugins via the 
   * plugin messenger instance */
  void logMessage(QString theModelGuid, QString theMessage);
  /** Usually receives mesages from modeller plugins via the 
   * plugin messenger instance */
  void logError(QString theModelGuid, QString theMessage);
  void on_pbnCancel_clicked();
  void on_toolShowLog_clicked();
  void on_toolHideLog_clicked();
private:
  /** Event handler for when the form is resized -
   * it will ensure the column widths remain reasonable */
  void resizeEvent ( QResizeEvent * theEvent );
  /** The experiment that this progress dialog will watch */
  OmgExperiment * mpExperiment;
  /** A flag to keep track of wether the experiment pointer has
   * been set or not */
  bool mExperimentSetFlag;
  /** A hash to keep track of each progress bar linked to each model execution 
   * the key for the hash is model.id
   * the value is a progressbar pointer that will be displayed in the tblQueue
   */
  typedef QHash <QString,QProgressBar * > ProgressBarsHash;
  ProgressBarsHash mProgressBarsHash;
  /** A hash to keep track of each icon linked to each model execution 
   * the key for the hash is model.id
   * the value is a QTableWidgetItem pointer that will have the icon in the tblQueue
   */
  typedef QHash <QString,QTableWidgetItem * > IconsHash;
  IconsHash mIconWidgetsHash;
  /** A timer so we can ensure the display is updated regularly */
  QTimer * mpTimer;
};

#endif //OMGEXPERIMENTPROGRESS_H
