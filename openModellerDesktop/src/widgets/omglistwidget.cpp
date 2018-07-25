/***************************************************************************
 *   Copyright (C) 2006 Tim Sutton   *
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

//local includes
#include "omglistwidget.h"
//qt includes
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
#include <QListWidgetItem>

  OmgListWidget::OmgListWidget(QWidget* parent, Qt::WFlags fl)
:  QWidget(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  lstItems->setEnabled(true);
  //we want this widget to behave list a QListWidget, so item clicked events in 
  //the internal list must be propogated
  connect (lstItems,SIGNAL(itemPressed(QListWidgetItem *)),
		  this,SLOT(itemPressedSlot(QListWidgetItem *)));
   
}

OmgListWidget::~OmgListWidget()
{}

//this is a private slot whose only job is to propogate teh 
//itemClick event of the lstItems internal widget out beyond
//this custom widget
void OmgListWidget::itemPressedSlot(QListWidgetItem * thepItem)
{
  emit itemPressed(thepItem);
  /* Commented out for now since it causes a conflict
   * with checkbox toggle behaviour. Clients of this
   * class can still use the signal itemPressed if they want to

  if (thepItem->checkState()==Qt::Checked)
  {
    thepItem->setCheckState(Qt::Unchecked);
  }
  else
  {
    thepItem->setCheckState(Qt::Checked);
  }
  qDebug ("Item pressed: " + thepItem->text().toLocal8Bit());
  */
}

void OmgListWidget::selectAll()
{
  qDebug("OmgListWidget::selectAll");
  for ( int myCounter = 0; myCounter < lstItems->count(); myCounter++ )
  {
    QListWidgetItem *  mypItem = lstItems->item(myCounter);
    mypItem->setCheckState(Qt::Checked);
    qDebug(mypItem->text().toLocal8Bit() + " has been checked");
  }
}

void OmgListWidget::invertSelection()
{
  qDebug("OmgListWidget::invertSelection");
  for ( int myCounter = 0; myCounter < lstItems->count(); myCounter++ )
  {
    QListWidgetItem *  mypItem = lstItems->item(myCounter);
    if (mypItem->checkState()==Qt::Checked)
    {
      mypItem->setCheckState(Qt::Unchecked);
      qDebug(mypItem->text().toLocal8Bit() + " has been unchecked");
    }
    else
    {
      mypItem->setCheckState(Qt::Checked);
      qDebug(mypItem->text().toLocal8Bit() + " has been checked");
    }
  }
}

void OmgListWidget::selectNone()
{
  qDebug("OmgListWidget::selectNone");
  for ( int myCounter = 0; myCounter < lstItems->count(); myCounter++ )
  {
    QListWidgetItem *  mypItem = lstItems->item(myCounter);
    mypItem->setCheckState(Qt::Unchecked);
    qDebug(mypItem->text().toLocal8Bit() + " has been unchecked");
  }
}

int OmgListWidget::checkedItemCount()
{
  return checkedItems().count();
}

QStringList OmgListWidget::checkedItems()
{
  QStringList myList;
  for ( int myCounter = 0; myCounter < lstItems->count(); myCounter++ )
  {
    QListWidgetItem *  mypItem = lstItems->item(myCounter);
    if (mypItem->checkState()==Qt::Checked)
    {
      myList << mypItem->text();
    }
  }
  return myList;
}
QStringList OmgListWidget::checkedDataItems()
{
  QStringList myList;
  for ( int myCounter = 0; myCounter < lstItems->count(); myCounter++ )
  {
    QListWidgetItem *  mypItem = lstItems->item(myCounter);
    if (mypItem->checkState()==Qt::Checked)
    {
      myList << mypItem->data(Qt::UserRole).toString();
    }
  }
  return myList;
}
void OmgListWidget::hideOptionsTool()
{
  toolConfigure->hide();
}
void OmgListWidget::showOptionsTool()
{
  toolConfigure->show();
}
void OmgListWidget::hideAddItemTool()
{
  toolAddItem->hide();
}
void OmgListWidget::showAddItemTool()
{
  toolAddItem->show();
}
//////////////////////////////////
// All other slots should use exactly the api format of QListWidget !!
/////////////////////////////////
void OmgListWidget::addItem ( const QString & theLabel , bool theCheckedFlag=false)
{
    QListWidgetItem *mypItem = new QListWidgetItem(theLabel,lstItems);
    //delegate
    addItem (mypItem,theCheckedFlag);
}
void OmgListWidget::addItem ( QListWidgetItem * thepItem, bool theCheckedFlag=false )
{
  //for our custom widget, always display a checkbox
    if (!theCheckedFlag)
    {
      thepItem->setCheckState(Qt::Unchecked);
    }
    else
    {
      thepItem->setCheckState(Qt::Checked);
    }
    lstItems->addItem(thepItem);
}
void OmgListWidget::addItems ( const QStringList & theLabels , bool theCheckedFlag=false)
{
  //using qts java style iterator...
  QStringListIterator myIterator(theLabels);
  while (myIterator.hasNext())
  {
    //get next iterator and delegate
    addItem(myIterator.next(),theCheckedFlag);
  }
}
void OmgListWidget::clear()
{
	lstItems->clear();
}
int OmgListWidget::count()
{
	return lstItems->count();
}

QListWidgetItem * OmgListWidget::item(int theItem)
{
	return lstItems->item(theItem);
}
void OmgListWidget::insertItem ( int theRow, QListWidgetItem * thepItem , bool theCheckedFlag=false)
{
    if (!theCheckedFlag)
    {
      thepItem->setCheckState(Qt::Unchecked);
    }
    else
    {
      thepItem->setCheckState(Qt::Checked);
    }
    lstItems->insertItem(theRow,thepItem);
}
void OmgListWidget::insertItem ( int theRow, const QString & theLabel , bool theCheckedFlag=false)
{
    QListWidgetItem *mypItem = new QListWidgetItem(theLabel,lstItems);
    //delegate
    insertItem (theRow,mypItem,theCheckedFlag);
}
void OmgListWidget::insertItems ( int theRow, const QStringList & theLabels , bool theCheckedFlag=false)
{
  //using qts java style iterator...
  QStringListIterator myIterator(theLabels);
  while (myIterator.hasNext())
  {
    //get next iterator and delegate
    insertItem(theRow++,myIterator.next(),theCheckedFlag);
  }
  
}


//
// Private slots
//

void OmgListWidget::on_toolSelectAll_clicked()
{
  //delegate
  selectAll();
}
void OmgListWidget::on_toolSelectNone_clicked()
{
  //delegate
  selectNone();
}

void OmgListWidget::on_toolInvertSelection_clicked()
{
  //delegate
  invertSelection();
}

void OmgListWidget::on_toolAddItem_clicked()
{
  qDebug("on_toolAddItem_clicked()");
  emit addItemClicked();
}

void OmgListWidget::on_toolConfigure_clicked()
{
  emit configureClicked(); 
}
