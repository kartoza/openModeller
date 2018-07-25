
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
#include "omglayerselector.h"
#include <omgmodellerplugininterface.h>
#include <omgui.h>
#include <omgmodellerpluginregistry.h>

//qt includes
#include <QAbstractItemView>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QPixmap>
#include <QResizeEvent>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QTreeWidget>
//standard includes
#include <iostream>




OmgLayerSelector::OmgLayerSelector( QString theBaseDir, QWidget* parent, Qt::WFlags fl )
 : QDialog(parent,fl),
 mBaseDirName(theBaseDir)
{
  setupUi(this);
  //show the user some useful help info so they know what is going on...
  showInfo();
  show();
  mFolderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                                     QIcon::Normal, QIcon::Off);
  mFolderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                                          QIcon::Normal, QIcon::On);
  mProjectionOkIcon.addPixmap(QPixmap(":/projectionOk.png"));
  mProjectionErrorIcon.addPixmap(QPixmap(":/projectionError.png"));
  
  treeFiles->setRootIsDecorated(true);
  QStringList myList;
  myList << "File Name";
  treeFiles->setColumnCount(1);
  treeFiles->setHeaderLabels(myList);
  treeFiles->setAlternatingRowColors ( true );
  treeFiles->header()->setResizeMode(0,QHeaderView::Stretch);
  //this hides the title row in the tree view
  //treeFiles->header()->setHidden(1);

  // When using a local plugin we can browse the filesystem
  // This is is a hard coded way to keep track of that
  // but I cant think of a neater way to do that at the moment
  QSettings mySettings;
  QString myModellerAdapterName = mySettings.value("openModeller/modellerPluginType", "Local Modeller Plugin" ).toString();
  bool myPluginIsLocalFlag=false;
  if (myModellerAdapterName =="Local Modeller Plugin")
  {
    toolDirectorySelector->setEnabled(true);
    lblBaseDir->setText(tr("Base Dir: ") + mBaseDirName);
    myPluginIsLocalFlag=true;
  }
  else
  {
    toolDirectorySelector->setEnabled(false);
    lblBaseDir->setText(myModellerAdapterName);
    toolSupportedFileFormats->hide();
  }
  
  QFileInfo myFileInfo(mBaseDirName);
  if ( (myFileInfo.exists() && !mBaseDirName.isEmpty()) || (!myPluginIsLocalFlag) )
  {
    //std::cout << "Provided directory found - traversing subdirectories" << std::endl;
    treeFiles->clear();
    mpListParent = new QTreeWidgetItem(treeFiles);
    mpListParent->setText(0,myModellerAdapterName);
    buildTree(mBaseDirName,mpListParent);
    treeFiles->setItemExpanded(mpListParent,true);
  }
  else
  {
    //std::cout << "Provided directory does not exist - prompting for directory" << std::endl;
    mBaseDirName=QCoreApplication::applicationDirPath();
    on_toolDirectorySelector_clicked();
  }
  tbNotes->hide();
}

OmgLayerSelector::~OmgLayerSelector()
{

}

void OmgLayerSelector::resizeEvent ( QResizeEvent * theEvent )
{
  treeFiles->header()->resizeSection(0,(theEvent->size().width()));
}
void OmgLayerSelector::on_toolDirectorySelector_clicked()
{
  toolDirectorySelector->setEnabled(false);
  //@NOTE: Why am I not using QFileDialog::getExistingDir static call?
  //under windows and Qt4.2 doing that was causing an assert deep in Qt
  //wheras using an object as below resolves the issue	
  QFileDialog myDialog(this);
  myDialog.setDirectory(mBaseDirName);
  myDialog.setFileMode(QFileDialog::DirectoryOnly);
  if (myDialog.exec())
  {
    mBaseDirName = myDialog.selectedFiles()[0];
  }
  else
  {
    // TODO: Discover how to programatically close this dialog
    reject();
    return;
  }
  lblBaseDir->setText(tr("Base Dir: ") + mBaseDirName);
  tbNotes->show();
  treeFiles->clear();
  mpListParent = new QTreeWidgetItem(treeFiles);
  mpListParent->setText(0,mBaseDirName);
  buildTree(mBaseDirName,mpListParent,true); //force rescan of the layers dir
  treeFiles->setItemExpanded(mpListParent,true);
  tbNotes->hide();
  toolDirectorySelector->setEnabled(true);
}

void OmgLayerSelector::on_toolRefresh_clicked()
{
  tbNotes->show();
  treeFiles->clear();
  mpListParent = new QTreeWidgetItem(treeFiles);
  mpListParent->setText(0,mBaseDirName);
  buildTree(mBaseDirName,mpListParent,true); //force rescan of the layers dir
  treeFiles->setItemExpanded(mpListParent,true);
  tbNotes->hide();
}

void OmgLayerSelector::updateFileList()
{
   mSelectedLayersList.clear();
   QListIterator<QTreeWidgetItem*> myIterator(treeFiles->selectedItems());
   while (myIterator.hasNext())
   {
     QString myId = myIterator.next()->data(0,Qt::UserRole).toString();
     if (!myId.isEmpty())
     {
       mSelectedLayersList << myId;
     }
   }
}

void OmgLayerSelector::accept()
{
  qDebug("LayerSelector::accept() called");
  updateFileList();
  done(1);
}


void OmgLayerSelector::reject()
{
  qDebug("LayerSelector::reject() called");
  mSelectedLayersList.clear();
  done(0);
}

QStringList OmgLayerSelector::getSelectedLayers() 
{
   return mSelectedLayersList;
}

QString OmgLayerSelector::getBaseDir() 
{
  return mBaseDirName;
}

void OmgLayerSelector::setSelectionMode(QAbstractItemView::SelectionMode theMode)
{
   treeFiles->setSelectionMode(theMode); 
}

void OmgLayerSelector::buildTree(const QString& theDirName, 
    QTreeWidgetItem* theParentListViewItem, 
    bool theForceScanFlag)
{
  if (theDirName.isEmpty()) return;
  if (!theParentListViewItem) return;
  //try to read the cache file first otherwise get the modeller plugin to give us
  //the file list xml
  QString myCachePath = Omgui::fileSelectorCachePath();
  QString myCacheFileName = myCachePath + QDir::separator() + "layerSelectorCache.xml";
  QString myDocumentText;
  //just read the cache file
  bool myResult = false;
  if (!theForceScanFlag)
  {
    QFile myFile( myCacheFileName );
    if ( myFile.open( QIODevice::ReadOnly ) )
    {
      myDocumentText = myFile.readAll();
      myFile.close();
      myResult=true;
    }
    else
    {
      qDebug("Failed to open "  + myCacheFileName.toLocal8Bit() + " layer selector cache file ");
      //@TODO Error handler!
      myResult=false;
    }
  }
  // if above failed we use the plugin to get the file list
  if (!myResult)
  {
    tbNotes->show();
    OmgModellerPluginInterface * mypModellerPlugin = OmgModellerPluginRegistry::instance()->getPlugin();
    //qDebug("Layer selector testing if returned modeller plugin is ok");
    if(!mypModellerPlugin)
    {
      //this is bad! TODO notify user he has no useable adapters
      //TODO handle this more gracefully than asserting!
      //qDebug("LayerSelector no valid modelling adapters could be loaded");
      QMessageBox::critical( this,tr("openModeller Desktop"),tr("No modelling plugins could be found.\nPlease report this problem to you system administrator or the openModeller developers."));
      //assert ("Undefined adapter type in __FILE__  , line  __LINE__");
      return;
    }
    else
    {
      //qDebug("Plugin is good to go....");
    }

    connect (mypModellerPlugin->getMessenger(), SIGNAL(refresh()),this,SLOT(refresh()));
    myDocumentText=mypModellerPlugin->getLayers(theDirName);
    tbNotes->hide();
  }
  //cache the file tree
  Omgui::createTextFile(myCachePath + QDir::separator() + "layerSelectorCache.xml", myDocumentText);
  QString myError;
  int myErrorLine;
  int myErrorColumn;
  QDomDocument myDocument;
  if (!myDocument.setContent(myDocumentText.toAscii(), true, &myError, &myErrorLine, &myErrorColumn)) 
  {
    QMessageBox::information(window(), tr("Available Layers"),
        tr("Parse error at line %1, column %2:\n%3")
        .arg(myErrorLine)
        .arg(myErrorColumn)
        .arg(myError));
    return;
  }
  QDomElement myRoot = myDocument.documentElement();
  QDomElement myChild = myRoot.firstChildElement("LayersGroup");
  //qDebug("LayerSelector: set root element to " + myChild.attribute("Id").toLocal8Bit());
  while (!myChild.isNull()) 
  {
    parseLayerGroup(myChild,theParentListViewItem);
    myChild = myChild.nextSiblingElement("LayersGroup");
  }
  //this next part is a kludge because for some reason the root node is not
  //expandable until the list is sorted.
  //@TODO work out how to get rid of this hack
  treeFiles->sortItems(0,Qt::AscendingOrder);
  treeFiles->setItemExpanded(theParentListViewItem,true);
}   

void OmgLayerSelector::parseLayerGroup(const QDomElement &theElement, QTreeWidgetItem *thepParentItem)
{
  QTreeWidgetItem * mypItem = createItem(theElement, thepParentItem);

  QString myLabel = theElement.firstChildElement("Label").text();
  if (myLabel.isEmpty())
  {
    myLabel = QObject::tr("Unnamed Folder");
  }
  //qDebug("LayerSelector found layer group: " + myLabel.toLocal8Bit());
  mypItem->setIcon(0, mFolderIcon);
  mypItem->setText(0, myLabel);
  treeFiles->setItemExpanded(mypItem,true);


  QDomElement myChildElement = theElement.firstChildElement();
  while (!myChildElement.isNull()) 
  {
    if (myChildElement.tagName() == "LayersGroup") 
    {
      parseLayerGroup(myChildElement,mypItem);
    }
    else if (myChildElement.tagName() == "Layer") 
    {
      QTreeWidgetItem *mypChildItem = createItem(myChildElement,mypItem);
      QString myLabel = myChildElement.firstChildElement("Label").text();
      if (myLabel.isEmpty())
      {
        myLabel = QObject::tr("Unnamed Layer");
      }
      //qDebug("LayerSelector found layer: " + mypChildItem->text(0).toLocal8Bit());
      mypChildItem->setFlags(mypItem->flags() | Qt::ItemIsEditable);
      if (myChildElement.attribute("HasProjection")=="1")
      {
        mypChildItem->setIcon(0, mProjectionOkIcon);
      }
      else
      {
        mypChildItem->setIcon(0, mProjectionErrorIcon);
      }
      mypChildItem->setText(0, myLabel);
      mypChildItem->setData(0, Qt::UserRole, myChildElement.attribute("Id"));
      
    } 
    myChildElement = myChildElement.nextSiblingElement();
  }
}

QTreeWidgetItem * OmgLayerSelector::createItem(const QDomElement &theElement,
                                            QTreeWidgetItem * thepParentItem)
{
  QTreeWidgetItem * mypItem=0;
  if (thepParentItem) 
  {
    //qDebug("OmgLayerSelector::createItem with parent  " + thepParentItem->text(1).toLocal8Bit());
    mypItem = new QTreeWidgetItem(thepParentItem);
  } 
  return mypItem;
}

void OmgLayerSelector::refresh()
{
  QCoreApplication::processEvents();
}

void OmgLayerSelector::showInfo()
{
  QString myStyle = Omgui::defaultStyleSheet(); 
  tbNotes->document()->setDefaultStyleSheet(myStyle);
  QString myInfo = 
    "<h2>" + tr("Scanning for GIS layers") + "</h2>" +
    tr("openModeller Desktop is now searching for"
      " valid gis layers. This may take a while. The layers list"
      " will be cached so the next time you use this layer selector"
      " tool you will not need to wait.");
  tbNotes->setHtml(myInfo);
}

void OmgLayerSelector::on_toolSupportedFileFormats_clicked()
{
  mySupportedFileFormats.exec();
}
