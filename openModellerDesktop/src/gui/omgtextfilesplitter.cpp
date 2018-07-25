/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   aps02ts@macbuntu   *
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
#include "omgtextfilesplitter.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qstring.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qsettings.h>
#include <qapplication.h>
#include <qcursor.h>
#include <QRadioButton>
#include <QMessageBox>

OmgTextFileSplitter::OmgTextFileSplitter() : QDialog()
{
  setupUi(this);
  QSettings myQSettings;
  leFileName->setText(myQSettings.value("/TextFileSplitter/leFileName").toString());
  leDirName->setText(myQSettings.value("/TextFileSplitter/leDirName").toString());
}


OmgTextFileSplitter::~OmgTextFileSplitter()
{}

void OmgTextFileSplitter::split(QString theFileNameString, QString theOutputDirString, InputType theInputType=Default)
{
  //
  // Now that we have the localities text file, we need to parse it
  // and split it into seperate files
  //
  //first build a regex to match text at the beginning of the line
  QRegExp myQRegExp;
  if (theInputType==Default)
  {
    qDebug( "Genus species headers file" );
    myQRegExp =  QRegExp("^[^#][a-zA-Z][ a-zA-Z\t]*") ; //second caret means 'not'
  }
  else 
  {
    qDebug("Comma Delimited File or openModeller format file");
    myQRegExp=QRegExp("");
  }
  QString myTaxonName;
  QFile myInputFile (theFileNameString);
  // open the output file initially to the input file
  // the first time a taxon is encountered the open file will
  // be changed to an output file for that taxon
  QFile myOutputFile (theFileNameString);
  QTextStream myOutputTextStream( &myOutputFile );
  if ( myInputFile.open( QIODevice::ReadOnly ) )
  {
    // set the cursor to hour glass
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    //now we parse the loc file, checking each line for its taxon
    QTextStream myInputTextStream( &myInputFile );
    QString myCurrentLineQString;
    QString myLastTaxonName = "";
    int myFileCount=0; //store how many files we output
    while ( !myInputTextStream.atEnd() )
    {
      QString	myLong, myLat;
      QStringList myList; 

      myCurrentLineQString = myInputTextStream.readLine(); // line of text excluding '\n'
      // if line is just blank dont bother doing any work
      if (myCurrentLineQString.simplified().isEmpty())
      {
        continue;
      }

      //
      // Clean up the line a litle
      //
      QString myLine = myCurrentLineQString;
      myLine = myLine.simplified().replace("\t"," ");
      myLine = myLine.trimmed();

      //
      //see if this line contains a taxon
      //
      if (theInputType==Default)
      {
        //qDebug("Input type default");
        myList = myLine.split(" ");
        if (myQRegExp.lastIndexIn(myCurrentLineQString) != -1)
        {
          //            qDebug("Found name");
          QStringList myMatchesQStringList = myQRegExp.capturedTexts();
          QStringList::Iterator myIterator = myMatchesQStringList.begin();
          myTaxonName=*myIterator;

          myTaxonName=myTaxonName.simplified();
          if (myTaxonName != "" && myMatchesQStringList.size() > 0)
          {
            //make sure there are not tabs separating words.
            myTaxonName=myTaxonName.replace( QRegExp("\t"), " " );
            //make sure there are only single spaces separating words.
            myTaxonName=myTaxonName.replace( QRegExp(" {2,}"), " " );
            //get rid of latitude and longitude if they exist.
            myTaxonName=myTaxonName.replace( QRegExp("Latitude"), "" );
            myTaxonName=myTaxonName.replace( QRegExp("Longitude"), "" );
            myTaxonName=myTaxonName.simplified();
          }
        }
      }
      else if (theInputType==OpenModeller) //openmodeller  delimted format
      {
        myList = myLine.split(" ");
        if (myList.size() < 4)
        {
          continue;
        }
        myTaxonName=myList.at(0).simplified() + " " + myList.at(1).simplified();
      }
      else //comma delimted
      {
        myList = myLine.split(",");
        if (myList.size() < 4)
        {
          continue;
        }
        myTaxonName=myList.at(0).simplified() + " " + myList.at(1).simplified();
      }
      //qDebug(myLastTaxonName.toAscii() + " " + myTaxonName.toAscii());
      if (myLastTaxonName!=myTaxonName)
      {
        ++myFileCount;
        //Make a filename from the species name to output its associated localities into
        QString myOutputFileName = myTaxonName;
        myOutputFileName.replace(" ","_");
        myOutputFileName = myOutputFileName+".txt";
        //qDebug("Found Taxon Name: " + myTaxonName + "...Saving its localities into: " + theOutputDirString + myOutputFileName );
        // create the output file for this taxon
        myOutputFile.close();
        myOutputFile.setFileName(theOutputDirString + QDir::separator() + myOutputFileName);
        //note file is not appended to but overwritten!
        myOutputFile.open(QIODevice::WriteOnly ); 
        myOutputTextStream.setDevice(&myOutputFile);
      }  //line is a new taxon name

      myLastTaxonName=myTaxonName;
      if (theInputType==Default)	
      {
        if (myList.size() < 2 )
        {
          continue;
        }	
        if (radLongitudeFirst->isChecked())
        {
          myLong = myList.at(0);
          myLat = myList.at(1);
        }
        else
        {
          myLong = myList.at(1);
          myLat = myList.at(0);
        }
      }
      else //comma or openmodeller delim
      {
        if (myList.size() < 4 )
        {
          continue;
        }	
        if (radLongitudeFirst->isChecked())
        {
          //  qDebug("Getteing lat and long");
          myLong = myList.at(2);
          myLat = myList.at(3);
        }
        else
        {
          myLong = myList.at(3);
          myLat = myList.at(2);
        }

      }
      //qDebug( myTaxonName.toAscii()); 
      myOutputTextStream << myTaxonName.toLocal8Bit() << " " <<  myLong << " " << myLat << "\r\n";
    }
    myInputFile.close();
    myOutputFile.close();
    QApplication::restoreOverrideCursor();
    QMessageBox::information( this,tr("openModeller Desktop"),tr("File splitter completed successfully. ") +
            QString::number(myFileCount) + tr(" new files were created.") );
  }
}

/*
   Although it is easy to implement a custom slot in the dialog and connect it in the constructor, we could instead use uic's auto-connection facilities to connect the OK button's clicked() signal to a slot in our subclass. To do this, we only need to declare and implement a slot with a name that follows a standard convention:

   void on_<widget name>_<signal name>(<signal parameters>);
   */

void OmgTextFileSplitter::on_pbnFileSelector_clicked()
{
  qDebug("File Selector Button clicked");
  QFileInfo myFileInfo(leFileName->text());
  QString myDirPath = myFileInfo.dir().dirName();
  QString myFileName = QFileDialog::getOpenFileName(
      this,
      "Choose a file containing occurrence data", 
      myDirPath,
      "Text Files (*.txt *.csv *.asc)");
  if (!myFileName.isEmpty())
  {
    leFileName->setText(myFileName);
  }

}
/*
   Although it is easy to implement a custom slot in the dialog and connect it in the constructor, we could instead use uic's auto-connection facilities to connect the OK button's clicked() signal to a slot in our subclass. To do this, we only need to declare and implement a slot with a name that follows a standard convention:

   void on_<widget name>_<signal name>(<signal parameters>);
   */
void OmgTextFileSplitter::on_pbnDirectorySelector_clicked()
{
  qDebug("Directory Selector Button clicked");
  QString myDirPath = QFileDialog::getExistingDirectory(
      this,
      "Choose a directory for the split files",
      leDirName->text()
      );
  if (!myDirPath.isEmpty())
  {
    leDirName->setText(myDirPath);
  }
}
void OmgTextFileSplitter::accept()
{
  QSettings myQSettings;
  myQSettings.setValue("/TextFileSplitter/leFileName",leFileName->text());
  myQSettings.setValue("/TextFileSplitter/leDirName",leDirName->text());
  if (radCommaDelimited->isChecked())
  {
    split(leFileName->text(),leDirName->text(),CommaDelimited);
  }
  else if (radOpenModeller->isChecked())
  {
    split(leFileName->text(),leDirName->text(),OpenModeller);
  }
  else //genus species headers
  {
    split(leFileName->text(),leDirName->text(),Default);
  }
  close();
}
