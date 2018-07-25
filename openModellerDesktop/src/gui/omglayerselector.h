/***************************************************************************
 *   Copyright (C) 2003 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   Gyps - Species Distribution Modelling Toolkit                         *
 *   This toolkit provides data transformation and visualisation           *
 *   tools for use in species distribution modelling tools such as GARP,   *
 *   CSM, Bioclim etc.                                                     *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef OMGLAYERSELECTOR_H
#define OMGLAYERSELECTOR_H


#include <ui_omglayerselectorbase.h>

#include <omgsupportedfileformats.h>

#include <QWidget>
#include <QStringList> 
#include <QTreeWidget>
#include <QDialog>
#include <QTreeWidgetItem>
#include <QIcon>
class QDomElement;
class QString;

/** \ingroup gui
 * \brief A dialog to let the user select one or more gdal rasters from the file system
@author Tim Sutton
*/
class OmgLayerSelector : public QDialog, private Ui::OmgLayerSelectorBase
{
Q_OBJECT
public:
    OmgLayerSelector( QString theBaseDir, QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~OmgLayerSelector() ;
    
public slots:
    void on_toolDirectorySelector_clicked();
    void on_toolRefresh_clicked();
    void on_toolSupportedFileFormats_clicked();
    void accept();
    void reject();
    QStringList getSelectedLayers();
    QString getBaseDir();
    /** Set the select mode for the layer selector. Typically you will want to use either
     * QAbstractItemView::ExtendedSelection
     * or
     * QAbstractItemView::SingleSelection
     * @param theBehavior the desired select behaviour as described above.
     */
    void setSelectionMode(QAbstractItemView::SelectionMode theMode);
    /** Let plugins scanning directories send regular progress updates */
    void refresh();
private:
    void resizeEvent ( QResizeEvent * event );
    QTreeWidgetItem * mpListParent; 
    QString mBaseDirName;
    QStringList mSelectedLayersList;
    void updateFileList();
    void buildTree(const QString& dirname, QTreeWidgetItem* theListViewItem, bool theForceScanFlag=false);
    void parseLayerGroup(const QDomElement &theElement, QTreeWidgetItem *theParentItem);
    void showInfo();
    QTreeWidgetItem * createItem(const QDomElement &theElement, QTreeWidgetItem * thepParentItem);
    QIcon mFolderIcon;
    QIcon mProjectionOkIcon;
    QIcon mProjectionErrorIcon;
    OmgSupportedFileFormats mySupportedFileFormats;
signals:
};

#endif
