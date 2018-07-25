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
#ifndef OMGEXPERIMENTITEMMODEL_H 
#define OMGEXPERIMENTITEMMODEL_H

#include "omgexperiment.h"
#include <QAbstractItemModel>
#include <QPointer>

/** \ingroup library
  * \brief The OmgExperimentItemModel class implements the abstract data
  * model QAbstractItemModel to provide a data model to use
  * with Qt graphical view classes. Using this aproach provides
  * a more performant way to let the user display and experiment in 
  * a tree view or list view rather than
  * simply populating a QTreeWidget with data. OmgExperimentItemModel
  * is a non gui class and its expected to be used with a
  * QTreeView in a gui context.
  */
class OMG_LIB_EXPORT OmgExperimentItemModel : public QAbstractItemModel
{
  public:
    OmgExperimentItemModel();
    ~OmgExperimentItemModel();
    //
    // accessors
    //

    //
    // Mutators
    //
    /** Set the experiment.
     * @param OmgExperiment * const - pointer to a constant omgexperiment which 
     * contains the localities to be represented in this table model 
     */
    void setExperiment(OmgExperiment * const thepExperiment);

    //
    // Methods that implement the QAbstractItemModel interface
    //

    /** Get the parent of the supplied model */
    QModelIndex parent(const QModelIndex&) const;
    /** Returns the data item and row,coloum */
    QModelIndex index ( int theRow, int theColumn, const QModelIndex & theParent = QModelIndex() ) const ;
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
    /** The experiment associated with this model. */
    QPointer<OmgExperiment> mpExperiment;

};

#endif //OMGEXPERIMENTITEMMODEL_H
