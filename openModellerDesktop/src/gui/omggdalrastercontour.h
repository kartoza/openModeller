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

#ifndef OMGGDALRASTERCONTOUR_H
#define OMGGDALRASTERCONTOUR_H

#include <ui_omggdalrastercontourbase.h>
#include <omggdal.h>


//
// QT includes
//
#include <QDialog>

/** \ingroup gui
 * \brief A dialog to generate a contour map from a gdal raster (experimental)
 * @author Tim Sutton
 * */

class OmgGdalRasterContour : public QDialog, private Ui::OmgGdalRasterContourBase
{
  Q_OBJECT;

public:
  OmgGdalRasterContour(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgGdalRasterContour();

private slots:
  /** Runs when the close button is pressed and saves current form state
  *
  */
  void accept();
  void on_pbnSelectFile_clicked();
  void on_pbnSelectDir_clicked();
  /** Runs when process button is pressed.
  * Does not close the form afterwards.
  */
  void on_pbnProcess_clicked();
  void error(QString theError);
  void updateProgress (int theCurrentValue, int theMaximumValue);

private:
  void readSettings();
  void writeSettings();
  OmgGdal mOmgGdal;
};
#endif //OMGOPTIONS_H
