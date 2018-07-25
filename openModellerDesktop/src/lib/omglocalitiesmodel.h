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
#ifndef OMGLOCALITIESMODEL_H 
#define OMGLOCALITIESMODEL_H

#include "omgmodel.h"
#include <QAbstractTableModel>
#include <QPointer>

/** \ingroup library
  * \brief The OmgLocalitiesModel class implements the abstract data
  * model QAbstractTableModel to provide a data model to use
  * with Qt graphical view classes. Using this aproach provides
  * a more performant way to let the user display samples than
  * simply populating a QTableWidget with data. OmgLocalitiesModel
  * is a non gui class and its expected to be used with a
  * QTableModel in a gui context.
  */
class OMG_LIB_EXPORT OmgLocalitiesModel : public QAbstractTableModel
{
  public:
    OmgLocalitiesModel();
    ~OmgLocalitiesModel();
    //
    // accessors
    //

    //
    // Mutators
    //
    /** Set the localities.
     * @param OmgModel * const - pointer to a constant omgmodel which 
     * contains the localities to be represented in this table model 
     */
    void setModel(OmgModel * const thepModel);

    //
    // Methods that implement the QAbstractTableModel interface
    //
    /** Returns the number of columns for the children of the given parent. */
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    /** Returns the number of rows for the children of the given parent. */
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    /** Returns the data stored under the given role for the 
     * item referred to by the index. */
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    /** Returns the headerData for the specified section (row or col) */
    QVariant headerData(int theSection, Qt::Orientation theOrientation,
                                           int theRole) const;
  private:
    /** The model that has the localities (with their associated sampled environment
     * values if available). */
    QPointer<OmgModel> mpNicheModel;

};

#endif //OMGLOCALITIES_H
