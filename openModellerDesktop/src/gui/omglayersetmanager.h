/***************************************************************************
 *   Copyright (C) 2005 by Peter Brewer   *
 *   peter@pembshideaway.co.uk   *
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

#ifndef OMGLAYERSETMANAGER_H
#define OMGLAYERSETMANAGER_H

#include <ui_omglayersetmanagerbase.h>
#include <omglayerset.h>
#include <omgui.h>
//
// QT includes
//
#include <QMap>
/** \ingroup gui
 * \brief A dialog for creating, modifying and deleting collections of layers 
 * that can be used for modelling.
 */
class OmgLayerSetManager : public QDialog, private Ui::OmgLayerSetManagerBase
{
  Q_OBJECT;

public:
  OmgLayerSetManager(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgLayerSetManager();
  /*$PUBLIC_FUNCTIONS$*/

protected slots:
  /*$PUBLIC_SLOTS$*/

  void on_pbnImport_clicked();
  void on_toolDelete_clicked();
  void on_toolNew_clicked();
  void on_toolCopy_clicked();
  void on_toolAddLayers_clicked();
  void on_toolDeleteLayers_clicked();
  void on_toolNewMaskLayer_clicked();
  void cellClicked(int theRow, int theColumn);

  /** Show the details for the given layerset.xml file 
   * in the gui. Note that the full path should not be given
   * only the filename in the format <guid>.xml
   * @param layerset to load (basename plus extension only)
   */
  void selectLayerSet(QString theFileName);
  void on_pbnApply_clicked();
  /** Refresh the list of algorithms in the alg table 
   * @param theGuid an optional parameter specifying the
   * algorithm that should be selected after the table is refreshed.
   * Leave it blank and the first cell will be selected by default
   * */
  void refreshLayerSetTable(QString theGuid="");
  void resizeEvent ( QResizeEvent * theEvent );
signals:



private:
  OmgLayerSet mLayerSet;
  Omgui::LayerSetMap mLayerSetMap;
};

#endif

