/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   aps02ts@macbuntu   *
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
#ifndef OMGTEXTFILESPLITTER_H 
#define OMGTEXTFILESPLITTER_H

#include <ui_omgtextfilesplitterbase.h>
#include <QDialog>
/** \ingroup gui
 * \brief A dialog that lets the user select an occurrences file and then
 * have it split into multiple discrete files - one for each species
 * name in the original file.
 *
 * Note that the species names must be grouped together.
 * @author Tim Sutton
*/
class OmgTextFileSplitter : public QDialog, private Ui::OmgTextFileSplitterBase
{
  Q_OBJECT;
    public:
  enum InputType
  {
    Default,
    CommaDelimited,
    OpenModeller,
    GenusSpeciesHeaders = Default
  };

  OmgTextFileSplitter() ;
  ~OmgTextFileSplitter();
  void split(QString theFileNameString,QString theOutputDirString, InputType theInputType);

  public slots:
    virtual void on_pbnFileSelector_clicked();
    virtual void on_pbnDirectorySelector_clicked();
    virtual void accept();

};

#endif
