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

#include "omglocality.h"  //provides omglocalities, omgsamples
#include "omglocalitiesmodel.h" 
#include <QStringList>
#include <QFileInfo>

OmgLocalitiesModel::OmgLocalitiesModel()
{

}
OmgLocalitiesModel::~OmgLocalitiesModel()
{

}
void OmgLocalitiesModel::setModel(OmgModel * const thepNicheModel)
{
  if (rowCount() > 0)
  {
    //first clear the table
    beginRemoveRows(QModelIndex(),0,rowCount()-1);
    endRemoveRows();
  }
  if (columnCount() > 0)
  {
    beginRemoveColumns(QModelIndex(),0,columnCount()-1);
    endRemoveColumns();
  }
  //update the localities
  mpNicheModel = thepNicheModel;;
  //now repopulate it - dont remove the if statements even if they look
  //unintuitive...
  if (rowCount() > 1)
  {
    beginInsertRows(QModelIndex(),0,rowCount()-1); //notify any views of data updates
    endInsertRows(); //notify any views of data updates
  }
  if (columnCount() > 1)
  {
    beginInsertColumns(QModelIndex(),0,columnCount()-1); //notify any views of data updates
    endInsertColumns(); //notify any views of data updates
  }
}
int OmgLocalitiesModel::columnCount ( const QModelIndex & theParent /*= QModelIndex()*/ ) const
{
  //to calculate we get the first locality element,
  //then find out how meny samples it contains and add 4 for
  // - id
  // - lat
  // - long
  // - abundance
  // if there are no localities simlpy return 0
  if (!mpNicheModel)
  {
    return 0;
  }

  if (mpNicheModel->localities().count() < 1)
  {
    return 0;
  }
  else
  {
    int myCount = mpNicheModel->localities().at(0).samples().count() + 4;
    return myCount;
  }
}
int OmgLocalitiesModel::rowCount ( const QModelIndex & theParent /*= QModelIndex()*/ ) const
{
  if (!mpNicheModel)
  {
    return 0;
  }
  if (!mpNicheModel->localities().count())
  {
    return 0;
  }
  return mpNicheModel->localities().count();
}
QVariant OmgLocalitiesModel::data ( const QModelIndex & theIndex, 
    int theRole /*= Qt::DisplayRole*/ ) const
{
  if (!mpNicheModel)
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
  //to get teh data we need to first check if the 
  //theIndex.row() is valid and then if the column specified
  //in the index is valid.
  //The column is mapped to the locality like this
  //0 - id
  //1 - lon
  //2 - lat
  //3 - abundance
  //>3 - samples.at(column-4)
  if (!mpNicheModel->localities().count())
  {
    return QVariant();
  }
  if (theIndex.row() > mpNicheModel->localities().count())
  {
    return QVariant();
  }
  if (theIndex.column() == 0 ) //no need to get the samples
  {
    // Id
    return QVariant(mpNicheModel->localities().at(theIndex.row()).id());
  }
  if (theIndex.column() == 1 ) //no need to get the samples
  {
    //long
    return QVariant(mpNicheModel->localities().at(theIndex.row()).longitude());
  }
  if (theIndex.column() == 2 ) //no need to get the samples
  {
    //lat
    return QVariant(mpNicheModel->localities().at(theIndex.row()).latitude());
  }
  if (theIndex.column() == 3 ) //no need to get the samples
  {
    //abundance
    return QVariant(mpNicheModel->localities().at(theIndex.row()).abundance());
  }
  //must be looking for the actual sample data so 
  //subtract 3 to offset the above 3 recs
  unsigned int mySampleNumber = theIndex.column() - 4;
  return QVariant(mpNicheModel->localities().at(theIndex.row()).samples().at(mySampleNumber));
}


QVariant OmgLocalitiesModel::headerData(int theSection, Qt::Orientation theOrientation,
                                           int theRole) const
{
  if (!mpNicheModel)
  {
    return QVariant();
  }
  if (theRole != Qt::DisplayRole)
  {
    return QVariant();
  }
  if (theOrientation == Qt::Horizontal)
  {
    //first three cols have fixed names...
    if (theSection==0)
    {
      return QString(tr("ID"));
    }
    if (theSection==1)
    {
      return QString(tr("Lon"));
    }
    if (theSection==2)
    {
      return QString(tr("Lat"));
    }
    if (theSection==3)
    {
      return QString(tr("Count"));
    }

    QStringList myList = mpNicheModel->creationLayerNames();
    //we offset the section no by 3 to take into account
    //the three non layer name headers above
    if (myList.count() > (theSection-4))
    {
      QFileInfo myFileInfo(myList.at(theSection-4));
      return QString(myFileInfo.baseName());
    }
    else
    {
      //final stopgap measure
      return QString("Layer %1").arg(theSection);
    }
  }
  else
  {
    // do nothing for row headers
    return QString("%1").arg(theSection);
  }
  return QVariant();
}
