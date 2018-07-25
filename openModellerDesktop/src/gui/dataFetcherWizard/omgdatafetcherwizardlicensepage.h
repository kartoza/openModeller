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

#ifndef OMGDATAFETCHERWIZARDLICENSEPAGE_H
#define OMGDATAFETCHERWIZARDLICENSEPAGE_H

#include <QWizard>

/** \ingroup gui
 * \brief The first page of a wizard that allows the 
 * user to find occurrence data from online databases.
 * This page asks the user to accept license terms.
 * @author Tim Sutton
 * */
class OmgDataFetcherWizardLicensePage : public QWizardPage
{
  Q_OBJECT;
  public:
  OmgDataFetcherWizardLicensePage(QWidget *parent);
};

#endif

