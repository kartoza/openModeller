/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "omgexperimentselector.h"
#include <omgui.h>

//qt includes
#include <QApplication>
#include <QAbstractButton>
#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QFileDialog>
#include <QProgressBar>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QXmlStreamReader> //efficient xml parser not using dom or sax
#include <QXmlStreamAttributes>
//standard includes
#include <iostream>


OmgExperimentSelector::OmgExperimentSelector( QWidget* parent, Qt::WFlags fl )
 : QDialog(parent,fl)
{
  setupUi(this);
  QPushButton * btnChangeDir = new QPushButton(tr("&Change Directory"));
  buttonBox->addButton(btnChangeDir, QDialogButtonBox::ActionRole);
  connect(btnChangeDir, SIGNAL(clicked()), this, SLOT(selectDirectory()));
  
  QSettings mySettings;
  QString myWorkDir = mySettings.value("dataDirs/dataDir",QDir::homePath() + QString("/.omgui/modelOutputs/")).toString();
  
  QFileInfo myFileInfo(myWorkDir);
  if ( (myFileInfo.exists() && !myWorkDir.isEmpty()) )
  {
    updateExperimentList();
  }
  else
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),tr("No existing experiments could be found in your work directory") + " (" + myWorkDir + ")");
  }

  QString myPreWorkDir = mySettings.value("dataDirs/PreDataDir",QDir::homePath() + QString("/.omgui/preOutputs/")).toString();
  QFileInfo myPreFileInfo(myPreWorkDir);
  if ( (myFileInfo.exists() && !myPreWorkDir.isEmpty()) )
  {
    updatePreAnalysisList();
  }
  else
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),tr("No existing pre-analysis could be found in your work directory") + " (" + myPreWorkDir + ")");
  }
}

OmgExperimentSelector::~OmgExperimentSelector()
{

}

void OmgExperimentSelector::updateExperimentList()
{
  lstExperiments->clear();
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  QStringList myList = Omgui::getExperimentsList();
  QStringListIterator myIterator(myList);
  while (myIterator.hasNext())
  {
    QString myFileName = myIterator.next();
    QFile myFile( myFileName );
    if ( !myFile.open( QIODevice::ReadOnly ) )
    {
      continue;
    }
    else
    {
#ifdef WIN32
      //there is currently an encoding problem reading experiments
      //that is win32 specific ... this is a temporary workaroumd
      QString myString = myFile.readLine();
      myString += "</Experiment>";
      //qDebug(myString.toLocal8Bit());
      QDomDocument myDocument("mydocument");
      QString myError;
      int myLine=0;
      int myCol=0;
      bool myResult = myDocument.setContent(myString,&myError,&myLine,&myCol);
      if (!myResult)
      {
        qDebug("Error setting document content in OmgExperimentSelector:");
        qDebug(myError.toLocal8Bit());
        qDebug("Line " + QString::number(myLine).toLocal8Bit());
        qDebug("Col " + QString::number(myCol).toLocal8Bit());
      }
      QDomElement myRootElement = myDocument.firstChildElement("Experiment");
      if (!myRootElement.isNull())
      {
        QString myName = Omgui::xmlDecode(myRootElement.attribute("Name"));
        QString myGuid = myRootElement.attribute("Guid");
        QListWidgetItem * mypItem = new QListWidgetItem(myName,lstExperiments);
        mypItem->setData(Qt::UserRole,myGuid);
        QIcon myIcon;
        myIcon.addFile(":/filenewExperiment.png");
        mypItem->setIcon(myIcon);
      }



#else
      // Parse the document using QXmlStreamReader which is more efficient
      // that dom or sax (see qt docs)
      // loop till we find an experiment attribute, get its
      // name and guid attribute then exit the parse
      bool myExperimentFoundFlag = false;
      QXmlStreamReader myReader;
      myReader.setDevice(&myFile);
      while (!myReader.atEnd() && !myExperimentFoundFlag && !myReader.hasError()) 
      {
        QXmlStreamReader::TokenType myToken = myReader.readNext();
        if (myToken == QXmlStreamReader::StartElement)
        {
          if (myReader.name() == "Experiment")
          {
            QString myName = myReader.attributes().value("Name").toString();
            QListWidgetItem * mypItem = new QListWidgetItem(myName,lstExperiments);
            mypItem->setData(Qt::UserRole,myReader.attributes().value("Guid").toString());
            //display an icon - at the moment I use teh same icon
            //for all items - later I will implement that show experiment
            //status (complete, incomplete etc)
            QIcon myIcon;
            myIcon.addFile(":/filenewExperiment.png");
            mypItem->setIcon(myIcon);
            myExperimentFoundFlag = true;
          }
        }
      }//end of xml stream reader loop
      if (myReader.hasError()) 
      {
        // do error handling
        qDebug("Error loading experiment for experiment selector:");
        qDebug(myFileName);
        qDebug(myReader.errorString());
        qDebug("Line: " + QString::number(myReader.lineNumber()).toLocal8Bit());
        qDebug("Column: " + QString::number(myReader.columnNumber()).toLocal8Bit());
        qDebug("Char Offset: " + QString::number(myReader.characterOffset()).toLocal8Bit());
      }//has error
#endif
      myFile.close();
    }//end of file open test
  }//end of myList Iterator
  QApplication::restoreOverrideCursor();
}

void OmgExperimentSelector::updatePreAnalysisList()
{
  lstPreAnalysis->clear();
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  QStringList myList = Omgui::getPreAnalysisList();
  QStringListIterator myIterator(myList);
  while (myIterator.hasNext())
  {
    QString myFileName = myIterator.next();
    QFile myFile( myFileName );
    if ( !myFile.open( QIODevice::ReadOnly ) )
    {
      continue;
    }
    else
    {
      // Parse the document using QXmlStreamReader which is more efficient
      // that dom or sax (see qt docs)
      // loop till we find an experiment attribute, get its
      // name and guid attribute then exit the parse
      bool myExperimentFoundFlag = false;
      QXmlStreamReader myReader;
      myReader.setDevice(&myFile);
      while (!myReader.atEnd() && !myExperimentFoundFlag && !myReader.hasError()) 
      {
        QXmlStreamReader::TokenType myToken = myReader.readNext();
        if (myToken == QXmlStreamReader::StartElement)
        {
          if (myReader.name() == "PreAnalysis")
          {
            QString myName = myReader.attributes().value("Name").toString();
            QListWidgetItem * mypItem = new QListWidgetItem(myName,lstPreAnalysis);
            QIcon myIcon;
            myIcon.addFile(":/filenewExperiment.png");
            mypItem->setIcon(myIcon);
            myExperimentFoundFlag = true;
          }
        }
      }//end of xml stream reader loop
      if (myReader.hasError()) 
      {
        // do error handling
        qDebug("Error loading experiment for pre-analysis selector:");
        qDebug(myFileName);
        qDebug(myReader.errorString());
        qDebug("Line: " + QString::number(myReader.lineNumber()).toLocal8Bit());
        qDebug("Column: " + QString::number(myReader.columnNumber()).toLocal8Bit());
        qDebug("Char Offset: " + QString::number(myReader.characterOffset()).toLocal8Bit());
      }//has error
      myFile.close();
    }//end of file open test
  }//end of myList Iterator */
  QApplication::restoreOverrideCursor();
}


void OmgExperimentSelector::accept()
{
  if ( tabWidget->currentIndex() == 0 )
  {
    chooseExperiment();
  }
  else
  {
    choosePreAnalysis();
  }
}

void OmgExperimentSelector::chooseExperiment()
{
  OmgExperiment * mypExperiment = new OmgExperiment();
  QSettings mySettings;
  QString myFileName;
  QString myWorkDir = mySettings.value("dataDirs/dataDir",QDir::homePath()+QDir::separator()+".omgui" ).toString()
                    + QString("/modelOutputs/");
  if (lstExperiments->currentRow() >=0)
  {
    //add the guid of the selected item to determin teh actual xml file name
    QString myGuid = lstExperiments->currentItem()->data(Qt::UserRole).toString();
    QString myDirName = lstExperiments->currentItem()->text();
    if (!myGuid.isEmpty())
    {
      myFileName = myWorkDir + myDirName + QDir::separator() + myGuid + ".xml";
      myFileName = QDir::toNativeSeparators (myFileName);
      if (QFile::exists(myFileName))
      {
        QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
        mypExperiment->fromXmlFile(myFileName);
        mypExperiment->setWorkDir(myWorkDir + QDir::separator() + myDirName + QDir::separator());
        QApplication::restoreOverrideCursor();
        emit loadExperiment(mypExperiment);
        done(1);
      }
      else
      {
        QMessageBox::information(window(),tr("Error"), tr("Error opening the experiment file : ") + 
          "\n" + myFileName + "\n" + tr("(File does not exist)."));
      }
    }
  }
  else
  {
    //if any of the above didnt work just close
    close();
  }
}


void OmgExperimentSelector::choosePreAnalysis()
{
  OmgPreAnalysis * mypPreAnalysis = new OmgPreAnalysis();
  QSettings mySettings;
  QString myFileName;
  QString myWorkDir = mySettings.value("dataDirs/PreDataDir",QDir::homePath() + QString("/.omgui/preOutputs/")).toString();
  if (lstPreAnalysis->currentRow() >=0)
  {
    QString myDirName = lstPreAnalysis->currentItem()->text();
    myFileName = myWorkDir + myDirName + QDir::separator() + "preanalysis.xml";
    myFileName = QDir::toNativeSeparators (myFileName);
    if (QFile::exists(myFileName))
    {
      QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
      mypPreAnalysis->fromXmlFile(myFileName);
      mypPreAnalysis->setWorkDir(myWorkDir + QDir::separator() + myDirName + QDir::separator());
      mypPreAnalysis->setCompleted(true);
      QApplication::restoreOverrideCursor();
      emit loadPreAnalysis(mypPreAnalysis);
      done(1);
    }
    else
    {
      QMessageBox::information(window(),tr("Error"), tr("Error opening the experiment file : ") + 
        "\n" + myFileName + "\n" + tr("(File does not exist)."));
    }
  }
  else
  {
    //if any of the above didnt work just close
    close();
  }
}

void OmgExperimentSelector::refresh()
{
  QCoreApplication::processEvents();
}

void OmgExperimentSelector::selectDirectory()
{
  QSettings mySettings;
  QString myOriginalWorkDir = 
    mySettings.value("dataDirs/dataDir",QDir::homePath() + 
        QString("/.omgui/modelOutputs/")).toString();
  QString myNewWorkDir = QFileDialog::getExistingDirectory(
      this,
      tr("Select a directory where your data will be saved") , //caption
      myOriginalWorkDir //initial dir
    );
  QFileInfo myFileInfo(myNewWorkDir);
  if ( (myFileInfo.exists() && !myNewWorkDir.isEmpty()) )
  {
    mySettings.setValue("dataDirs/dataDir",myNewWorkDir);
    mySettings.setValue("dataDirs/PreDataDir",myNewWorkDir);
    updateExperimentList();
    updatePreAnalysisList();
  }
  else
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("No existing experiments could be found in your work directory") 
        + " (" + myNewWorkDir + ")");
  }
}

void OmgExperimentSelector::hidePreAnalysis()
{
  tabWidget->removeTab(1);
}
