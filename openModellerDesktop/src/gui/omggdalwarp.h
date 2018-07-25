/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton and Peter Brewer   *
 *   tim@linfiniti.com and dovecote@pembshideaway.co.uk  *
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

#ifndef OMGGDALWARP_H
#define OMGGDALWARP_H

#include <ui_omggdalwarpbase.h>

class OmgGdal;

//
// QT includes
//
#include <QDialog>

/** \ingroup gui
 * \brief A dialog providing access to gdal_warp functions (experimental)
 * @author Tim Sutton
 * */

class OmgGdalWarp : public QDialog, private Ui::OmgGdalWarpBase
{
  Q_OBJECT;

public:
  OmgGdalWarp(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgGdalWarp();

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
  OmgGdal * mpOmgGdal;
  //! List of supported GDAL file filters
  QString mFilterList;
QStringList getListWidgetEntries(const QListWidget * theListWidget);
};
#endif //OMGOPTIONS_H
