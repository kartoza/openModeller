/***************************************************************************
 *   Copyright (C) 2006 by Peter Brewer, Tim Sutton   *
 *   peter@pembshideaway.co.uk tim@linfiniti.com   *
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

#ifndef OMGREPORTDIALOG_H
#define OMGREPORTDIALOG_H

#include <ui_omgreportdialogbase.h>
/** \ingroup gui
 * \brief A dialog to display the experiment summary report.
 */
class OmgReportDialog : public QDialog, private Ui::OmgReportDialogBase
{
  Q_OBJECT;

public:
  OmgReportDialog(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgReportDialog();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

signals:


private:
};

#endif

