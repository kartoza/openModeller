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

#ifndef OMGQGISPLUGINOPTIONS_H
#define OMGQGISPLUGINOPTIONS_H

#include <ui_omgqgispluginoptionsbase.h>
//
// QT includes
//
#include <QDialog>

/** \ingroup gui
 * \brief The options dialog allows the user to set various global
 * configuration options such as model output directory, language,
 * output image format etc.
 * @author Tim Sutton
 * */

class OmgQgisPluginOptions : public QDialog, private Ui::OmgQgisPluginOptionsBase
{
  Q_OBJECT;

public:
  OmgQgisPluginOptions(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgQgisPluginOptions();

private slots:
  void on_toolSelectLogFile_clicked();
  void on_cboModellerPluginName_currentIndexChanged();
  void on_cboOutputFormat_currentIndexChanged();
  void accept();
  void on_toolSelectDataDirectory_clicked();
  //void on_toolSelectContextLayer_clicked();
  void on_toolPresenceColour_clicked();
  void on_toolAbsenceColour_clicked();
  //void on_toolContextColour_clicked();


};
#endif //OMGQGISPLUGINOPTIONS_H
