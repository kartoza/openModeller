/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
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

#ifndef OMGMODELTESTOPTIONS_H
#define OMGMODELTESTOPTIONS_H

#include <ui_omgmodeltestoptionsbase.h>

/** \ingroup gui
 * \brief a dialog that allows a user to set up model test options.
 * @author Alexandre Copertino Jardim
 * */

class OmgModelTestOptions : public QDialog, private Ui::OmgModelTestOptionsBase
{
  Q_OBJECT;

public:
  OmgModelTestOptions(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgModelTestOptions();

  // get methods
  double getThreshold();

  bool getUseLowestThreshold();

  int getNumBackgroundPoints();

  double getMaxOmission();

  void hideLowestPrecenseThreshold();

private slots:

  /* from qt4 documentation
  Qt::Unchecked         0 The item is unchecked.
  Qt::PartiallyChecked  1 The item is partially checked. Items in hierarchical models
                          may be partially checked if some, but not all, of their children 
                          are checked.
  Qt::Checked           2 The item is checked.
  */
  void on_cbxLowestPrecenseThreshold_cliked( int state );

  void accept();
};

#endif
