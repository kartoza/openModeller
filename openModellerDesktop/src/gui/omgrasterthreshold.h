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

#ifndef OMGGDALRASTERTHRESHOLD_H
#define OMGGDALRASTERTHRESHOLD_H

#include <ui_omgrasterthresholdbase.h>
#include <omggdal.h>
#include <omgexperiment.h>
//
// QT includes
//
#include <QDialog>
#include <QPointer>

/** \ingroup gui
 * \brief This dialog provides an interface for carrying out thresholding and
 * hotspot analysis. 
 *
 * Thresholding is the process of converting continuous
 * probability value rasters into boolean layers. Hotspotting is the process
 * of aggregating thresholded layers together to create a composite view of
 * many models where the output is a raster indicating the number of
 * thresholded models.
 * @author Tim Sutton
 * */

class OmgRasterThreshold : public QDialog, private Ui::OmgRasterThresholdBase
{
  Q_OBJECT;

public:
  OmgRasterThreshold(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgRasterThreshold();
  void setExperiment(OmgExperiment * const thepExperiment);

private slots:
  void on_pbnThresholdOutputDir_clicked();
  void on_pbnSelectThresholdAggregateFile_clicked();

  /** Runs when process button is pressed.
  * Does not close the form afterwards.
  */
  void accept();
  void error(QString theError);
  void updateProgress (int theCurrentValue, int theMaximumValue);
  void on_cboSortBy_currentIndexChanged(QString theString);
  void populateTree();
  void resizeEvent ( QResizeEvent * theEvent );

private:
  void readSettings();
  void writeSettings();
  OmgGdal mOmgGdal;
  QPointer<OmgExperiment> mpExperiment;
  enum SortMode {BY_TAXON,BY_ALGORITHM};
  SortMode mSortMode;
};
#endif //OMGRASTERTHRESHOLD_H
