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

#include "omggbifconsolesearch.h"
#include <QString>


OmgGbifConsoleSearch::OmgGbifConsoleSearch(QObject *parent)
{

}

OmgGbifConsoleSearch::~OmgGbifConsoleSearch()
{

}

bool OmgGbifConsoleSearch::search(QString theFileName)
{
  setSearchList(theFileName);
  mBusyFlag = true;
  mListPosition = 0;
  mListSize = static_cast<unsigned int>(mNameList.count());
  QStringListIterator myIterator(mNameList);
  while (myIterator.hasNext())
  {
    qDebug(myIterator.next().toLocal8Bit());
  }
  processNextTaxon();
  //while (mBusyFlag==true)
  //{
  //  sleep(1000);
  //}
  return true;
}


void OmgGbifConsoleSearch::processNextTaxon()
{
  qDebug("processNextTaxon Called");
  mOmgScraperGbif = new OmgScraperGbif(this);
  connect(mOmgScraperGbif, SIGNAL(gettingTaxonID()),
          this, SLOT(gettingTaxonID()));
  connect(mOmgScraperGbif, SIGNAL(gettingBody()),
          this, SLOT(gettingBody()));
  connect(mOmgScraperGbif, SIGNAL(fileWritten(QString, QString, QString,int)),
          this, SLOT(fileWritten(QString, QString,QString, int)));
  connect(mOmgScraperGbif, SIGNAL(fileNotWritten(QString)),
          this, SLOT(fileNotWritten(QString)));
  connect(mOmgScraperGbif, SIGNAL(statusChanged(QString)),
          this, SLOT(setStatus(QString)));
  connect(mOmgScraperGbif, SIGNAL(error(QString)),
          this, SLOT(setError(QString)));

  QString mySearchString=mNameList.at( mListPosition );
  qDebug("processNextTaxon searching for" + mySearchString.toLocal8Bit());
  //QString myFileName = leOutputPath->text() + QDir::separator() +mySearchString + ".shp";
  QString myFileName = "/tmp/" +mySearchString + ".shp";
  myFileName.replace(" ","_");
  setStatus("Creating : " + myFileName.toLocal8Bit());
  mOmgScraperGbif->search(mySearchString, myFileName);
  mListPosition++;
}




void OmgGbifConsoleSearch::setSearchList(QString theFileName)
{
  //
  // Add all search strings from text file
  //
  //first build a regex to match text at the beginning of the line
  QRegExp myQRegExp( "^[^#][ a-zA-Z]*" ); //seconf caret means 'not'
  QStringList myTaxonList;
  QFile myQFile( theFileName );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the file, checking each line for its taxon string
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLineQString;
    while ( !myQTextStream.atEnd() )
    {
      myCurrentLineQString = myQTextStream.readLine(); // line of text excluding '\n'
      int  myPosInt = myQRegExp.indexIn( myCurrentLineQString,0 );
      if (myPosInt<0) continue;
      QStringList myMatchesQStringList = myQRegExp.capturedTexts();
      QStringList::Iterator myIterator = myMatchesQStringList.begin();
      QString myTaxonQString=*myIterator;
      //strip ending and leading whitespace
      QString myTaxonName=myTaxonQString.simplified();
      if (myTaxonName != "")
      {
        //make sure there are only single spaces separating words.
        myTaxonQString=myTaxonQString.replace( QRegExp(" {2,}"), " " );
        myTaxonList.append(myTaxonQString);
      }
    }
    myQFile.close();
    //sort the taxon list alpabetically
    myTaxonList.sort();
    //now find the uniqe entries in the qstringlist and
    //add each entry to the list
    QString myLastTaxon="";
    mNameList.clear();
    QStringList::Iterator myIterator= myTaxonList.begin();
    while( myIterator!= myTaxonList.end() )
    {
      QString myCurrentTaxon=*myIterator;
      if (myCurrentTaxon!=myLastTaxon)
      {
        mNameList << myCurrentTaxon;
      }
      myLastTaxon=*myIterator;
      ++myIterator;
    }
  }
  else
  {
    std::cerr << "Localities Fetcher  Error\n" << "The file is not readable. Check you have the neccessary file permissions and try again." << std::endl;
    return;
  }

}


void OmgGbifConsoleSearch::gettingTaxonID()
{
  setStatus("Connecting...");
}

void OmgGbifConsoleSearch::gettingBody()
{
  setStatus("Searching...");
}

void OmgGbifConsoleSearch::fileWritten(QString theShapeFile, QString theTextFile,QString theTaxonName,int theCount)
{
  disconnect(mOmgScraperGbif);
  delete mOmgScraperGbif;
  setStatus("Saving...");
  
  //show the retrieved points in the gui - disabled for now as its too verbose
  //showPoints(theTextFile); 
  setStatus(QString(theTaxonName + "(" + QString::number(theCount) + ")"));
  
  
  if (mListPosition<mListSize)
  {
    processNextTaxon();
  }
  else
  {
    mBusyFlag = false;
    setStatus("Search complete!");
    emit searchDone();
   
  }
}

void OmgGbifConsoleSearch::fileNotWritten(QString theTaxonName)
{
  disconnect(mOmgScraperGbif);
  delete mOmgScraperGbif;
  setStatus("Saving failed...");
  setStatus("No results found, file not written");
  setStatus(theTaxonName + "(0)");
  
  if (mListPosition<mListSize)
  {
    processNextTaxon();
  }
  else
  {
    mBusyFlag = false;
    emit searchDone();
    setStatus("Search complete!");
  }
}


void OmgGbifConsoleSearch::showPoints(QString theFileName)
{
  QFile myFile( theFileName );
  if ( myFile.open( QIODevice::ReadOnly ) )
  {
    //teResults->append(myFile.readAll());
    myFile.close();
  }
  else
  {
    setError(tr("Could not open the generated Points file") + " (" + theFileName.toLocal8Bit() + ")");
  }
  return ;
}

void OmgGbifConsoleSearch::setStatus(QString theStatus)
{
  qDebug ( theStatus.toLocal8Bit() );
}


void OmgGbifConsoleSearch::appendLogMessage(QString theMessage)
{
  qDebug ( theMessage.toLocal8Bit() );
}

void OmgGbifConsoleSearch::setError(QString theError)
{
qDebug ("********************");
qDebug ("*  ERROR           *");
qDebug ("********************");
qDebug (theError.toLocal8Bit() );
qDebug ("********************");
}


