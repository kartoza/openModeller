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

#ifndef OMGTERMSANDCONDITIONS_H
#define OMGTERMSANDCONDITIONS_H

#include <ui_omgtermsandconditionsbase.h>
//
// QT includes
//

/** \ingroup gui
 * \brief This class is used to check the user is willing to agree to license terms
 * @author Tim Sutton
 **/
class OmgTermsAndConditions : public QDialog, private Ui::OmgTermsAndConditionsBase
{
  Q_OBJECT;

public:
  OmgTermsAndConditions(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgTermsAndConditions();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
  void checkAgreement();

protected slots:
  /*$PROTECTED_SLOTS$*/
  void reject();
  void accept();

signals:


private:
};

#endif

