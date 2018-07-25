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

#include "omgexperimentitemmodel.h" 
#include <QStringList>
#include <QFileInfo>

OmgExperimentItemModel::OmgExperimentItemModel() : QAbstractItemModel()
{

}
OmgExperimentItemModel::~OmgExperimentItemModel()
{

}
void OmgExperimentItemModel::setExperiment(OmgExperiment * const thepExperiment)
{
  //first clear the table
  beginRemoveRows(QModelIndex(),0,rowCount()-1);
  endRemoveRows();
  beginRemoveColumns(QModelIndex(),0,columnCount()-1);
  endRemoveColumns();
  //update the localities
  mpExperiment = thepExperiment;;
  //now repopulate it
  beginInsertRows(QModelIndex(),0,rowCount()-1); //notify any views of data updates
  endInsertRows(); //notify any views of data updates
  beginInsertColumns(QModelIndex(),0,columnCount()-1); //notify any views of data updates
  endInsertColumns(); //notify any views of data updates
}

QModelIndex OmgExperimentItemModel::parent(const QModelIndex&) const
{
  return QModelIndex();

}
QModelIndex OmgExperimentItemModel::index ( int theRow, 
     int theColumn, 
     const QModelIndex & theParent /*= QModelIndex() */) const 
{
  if (!mpExperiment)
  {
    return QModelIndex();
  }
  if (!theParent.isValid())
  {
    return QModelIndex();
  }
  if (!mpExperiment->count())
  {
    return QModelIndex();
  }

  //return QVariant(mpExperiment->getModel(theRow)->name());
  return QModelIndex();
}

int OmgExperimentItemModel::columnCount ( const QModelIndex & theParent /*= QModelIndex()*/ ) const
{
    return 1;
}
int OmgExperimentItemModel::rowCount ( const QModelIndex & theParent /*= QModelIndex()*/ ) const
{
  if (!mpExperiment)
  {
    return 0;
  }
  if (!mpExperiment->count())
  {
    return 0;
  }
  return mpExperiment->count();
}
QVariant OmgExperimentItemModel::data ( const QModelIndex & theIndex, 
    int theRole /*= Qt::DisplayRole*/ ) const
{
  if (!mpExperiment)
  {
    return QVariant();
  }
  if (!theIndex.isValid())
  {
    return QVariant();
  }
  if (theRole != Qt::DisplayRole)
  {
    return QVariant();
  }
  if (!mpExperiment->count())
  {
    return QVariant();
  }
  return QVariant(mpExperiment->getModel(theIndex.row())->name());
}


QVariant OmgExperimentItemModel::headerData(
    int theSection, 
    Qt::Orientation theOrientation,
    int theRole) const
{
  if (!mpExperiment)
  {
    return QVariant();
  }
  if (theRole != Qt::DisplayRole)
  {
    return QVariant();
  }
  if (theOrientation == Qt::Horizontal)
  {
    return QString(tr("Experiment"));
  }
  else
  {
    // do nothing for row headers
    return QString("%1").arg(theSection);
  }
  return QVariant();
}

