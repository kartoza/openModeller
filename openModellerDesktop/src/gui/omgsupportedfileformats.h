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

#ifndef OMGSUPPORTEDFILEFORMATS_H
#define OMGSUPPORTEDFILEFORMATS_H

#include <ui_omgsupportedfileformatsbase.h>

/** \ingroup gui
 * \brief a dialog that allows a user to set up model test options.
 * @author Alexandre Copertino Jardim
 * */

class OmgSupportedFileFormats : public QDialog, private Ui::OmgSupportedFileFormatsBase
{
  Q_OBJECT;

public:
  OmgSupportedFileFormats(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgSupportedFileFormats();

private slots:
  void accept();
};

#endif //OMGSUPPORTEDFILEFORMATS_H
