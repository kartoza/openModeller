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

#ifndef OMGLOCALITIESOPTIONS_H
#define OMGLOCALITIESOPTIONS_H

#include <ui_omglocalitiesoptionsbase.h>
//
// QT includes
//
#include <QDialog>

/** \ingroup gui
 *  \brief A dialog for setting how localities will be read from disk.
 * @author Tim Sutton
 * */

class OmgLocalitiesOptions : public QDialog, private Ui::OmgLocalitiesOptionsBase
{
  Q_OBJECT;

public:
  OmgLocalitiesOptions(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgLocalitiesOptions();

  double getTrainingProportion();

  void hideIndependentTest();

private slots:
  void accept();
  void on_pbnSetCrs_clicked();

};
#endif //OMGLOCALITIESOPTIONS_H
