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

#ifndef OMGPUBLISHTOCATALOGUE_H
#define OMGPUBLISHTOCATALOGUE_H

#include <ui_omgpublishtocataloguebase.h>
//
// QT includes
//
class OmgModel;
/** \ingroup gui
 * \brief This class is used to publish a model to the web catalogue developed by Karla Fook 
 * @author Tim Sutton
 **/
class OmgPublishToCatalogue : public QDialog, private Ui::OmgPublishToCatalogueBase
{
  Q_OBJECT;

public:
  OmgPublishToCatalogue(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgPublishToCatalogue();
  /*$PUBLIC_FUNCTIONS$*/
  void setModel(OmgModel * thepModel);
   
public slots:
  /*$PUBLIC_SLOTS$*/

protected:

protected slots:

  void accept();

signals:


private:
  OmgModel * mpModel;
};

#endif

