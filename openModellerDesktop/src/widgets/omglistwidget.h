/***************************************************************************
 *   Copyright (C) 2006 by Tim Sutton   *
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

#ifndef OMGLISTWIDGET_H
#define OMGLISTWIDGET_H

#include <ui_omglistwidgetbase.h>
//
// QT includes
//

class QListWidgetItem;
class QStringList;

class OMG_LIB_EXPORT OmgListWidget : public QWidget, private Ui::OmgListWidgetBase
{
  Q_OBJECT;

public:
  OmgListWidget(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgListWidget();
  
  //////////////////////////////////
  // All other public methods should use exactly the api format of QListWidget !!
  /////////////////////////////////
  /** Find out how many items are in the list widget 
   * @return integer representing item count */
  int count();
  /** Find out how many items in the list widget are checked.
   * @return an integer containing the count of checked items.
   */
  int checkedItemCount();
  QListWidgetItem * item(int theItem);
  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/
  ///// Slots that are unique to the listwidget
  void selectAll();
  void invertSelection();
  void selectNone();
  /** Get a QStringList containing the text for all the checked items.
   *  @return A list of the checked items. If no items are chekced the list
   *  will be empty.
   */
  QStringList checkedItems();
  /** Get a QStringList containing the text from the data field 
   * for all the checked items. Data field in this context means QListWidgetItem.data()
   *  @return A list of the checked items. If no items are chekced the list
   *  will be empty.
   */
  QStringList checkedDataItems();
  /** Hide the options button */
  void hideOptionsTool();
  /** Show the options button */
  void showOptionsTool();
  /** Hide the '+' tool usually used by a developer to link to a dialog
   *  to load a list of entries into the list widget. The implementation
   *  of this is completely up to the developer, but it may not always be 
   *  needed so we can hide it if we want.
   *  @sa showAddItemTool
   */
  void hideAddItemTool();
  /** Show the '+' tool usually used by a developer to link to a dialog
   *  to load a list of entries into the list widget. The implementation
   *  of this is completely up to the developer, but it may not always be 
   *  needed so we can hide it if we want.
   *  @sa hideAddItemTool
   */
  void showAddItemTool();
  //////////////////////////////////
  // All other slots should use exactly the api format of QListWidget !!
  /////////////////////////////////
  void addItem ( const QString & theLabel, bool theCheckedFlag );
  void addItem ( QListWidgetItem * thepItem, bool theCheckedFlag );
  void addItems ( const QStringList & theLabels, bool theCheckedFlag );
  void clear();
  void insertItem ( int theRow, QListWidgetItem * thepItem, bool theCheckedFlag );
  void insertItem ( int theRow, const QString & theLabel, bool theCheckedFlag );
  void insertItems ( int theRow, const QStringList & theLabels, bool theCheckedFlag );
protected:
  /*$PROTECTED_FUNCTIONS$*/

public slots:
  /*$PROTECTED_SLOTS$*/
  /** Used for internal purposes to propogate the 
   * itemPressed signal from the list widget out to any listeners AND
   * to toggle item checked state of the selected item. 
   */
  void itemPressedSlot(QListWidgetItem * theItem);

signals:
  
  void itemPressed(QListWidgetItem * thepItem);
  void addItemClicked();
  void configureClicked();

private:

private slots:
  /////////////////////////////////
  // Slots inherited from the parent
  ////////////////////////////////
  void on_toolSelectAll_clicked();
  void on_toolSelectNone_clicked();
  void on_toolInvertSelection_clicked();
  void on_toolAddItem_clicked();
  void on_toolConfigure_clicked();
};

#endif

