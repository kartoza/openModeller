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

#include "omgscrapergbif.h"
#include "omgwebpagefetcher.h"
#include "omgui.h"

//QT Includes
#include <QTimer>
#include <QRegExp>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
//needed for Q_EXPORT_PLUGIN macro at the end of this file
#include <QtPlugin> 
OmgScraperGbif::OmgScraperGbif(QObject * parent) : 
  QObject(parent),
  OmgScraperPluginInterface()
{
  qDebug("OmgScraperGbif constructor called...");
}


OmgScraperGbif::~OmgScraperGbif()
{
}

const QString  OmgScraperGbif::getName() 
{ 
  return QString("GBIF Plugin"); 
}
const QString OmgScraperGbif::getLicense()
{
  QFile myQFile( ":/gbif_terms.txt" );
  QString myString;
  if ( myQFile.open( QIODevice::ReadOnly ) ) 
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myStream( &myQFile );
    myString = myStream.readAll();
    myQFile.close();
  }
  else
  {
    myString="Terms and conditions document for GBIF could not be retrieved.";
  }
  return myString; 
}

bool OmgScraperGbif::search(QString theTaxonName, QString theFileName)
{

  mTaxonName = theTaxonName;
  mFileName = theFileName;

  if (mTaxonName.isEmpty())
  {
    mMessenger.emitError("Taxon name is empty!");
    return false;
  }

  if (mFileName.isEmpty())
  {
    mMessenger.emitError("File name is empty!");
    return false;
  }

  mTaxonName = theTaxonName;
  mFileName = theFileName;
  QString mySearchString=theTaxonName.simplified();
  mySearchString=mySearchString.replace(" ","+");
  QString myUrlString = "http://www.secretariat.gbif.net/portal/ecat_search.jsp?";
  myUrlString += "termsAccepted=true&search=";
  myUrlString += mySearchString;
  myUrlString += "&countryKey=0&searchType=1&searchMode=1";

  qDebug("\n\n ------------------------ part 1 of gbif search ------------------- \n\n");
  qDebug ("URL for Search string = " + myUrlString.toLocal8Bit());
  OmgWebPageFetcher myWebPageFetcher;
  connect(&myWebPageFetcher, SIGNAL(statusChanged(QString)),
          this, SLOT(setStatus(QString)));
  QString myFirstPage = myWebPageFetcher.getPage(myUrlString);
  QString mySecondUrl = taxonIdRequestDone(myFirstPage);
  if (!mySecondUrl.isEmpty())
  {
    //ideally we should just be able to reuse the previously made fetcher
    OmgWebPageFetcher myWebPageFetcher2;
    connect(&myWebPageFetcher2, SIGNAL(statusChanged(QString)),
        this, SLOT(setStatus(QString)));
    qDebug("\n\n ------------------------ part 2 of gbif search ------------------- \n\n");
    qDebug("URL for Search Result string = " + mySecondUrl.toLocal8Bit());
    QString mySecondPage = myWebPageFetcher2.getPage(mySecondUrl);
    localitiesRequestDone(mySecondPage);
  }
  return true;
}


//
// This is run once the web page has been retrieved.
//

QString OmgScraperGbif::taxonIdRequestDone(QString theString)
{
  if (theString.isEmpty())
  {
    mMessenger.emitMessage(tr("Error getting species id"));
    mMessenger.emitFileNotWritten(mTaxonName);
    return false;
  }
  QRegExp myPhraseRegex( "taxonKey=[0-9]*&" );
  QRegExp myIdRegex( "[0-9]*" );

  //
  // Taxon id regex
  //
  if (!myPhraseRegex.isValid())
  {
    mMessenger.emitError(tr("Gbif scraper:: The taxon id regex is invalid. Fix it and try again" ));
    return false;
  }
  else
  {
    //make sure greedy matches are off so if you have a string like
    // <b>blah</b><b>blahblah</b>
    // and your regex is <b>.*</b>
    // non greedy match will return <b>blah</b>
    myPhraseRegex.setMinimal(true);
  }
  //
  // Taxon id only regex (for trimming out just the numbers from above
  //
  if (!myIdRegex.isValid())
  {
    mMessenger.emitError(tr("Gbif scraper:: The taxon id 'only' regex  is invalid. Fix it and try again" ));
    return false;
  }
  else
  {
    //make sure greedy matches are off so if you have a string like
    // <b>blah</b><b>blahblah</b>
    // and your regex is <b>.*</b>
    // non greedy match will return <b>blah</b>
    myIdRegex.setMinimal(true);
  }

  //
  // Main parsing loop
  //

  //new way of doing a qt4 regexp.search
  //a little test to see if we can get the taxon id out ok
  int  myPosInt = myPhraseRegex.indexIn( theString,0 );

  if ( myPosInt >= 0 )
  {
    QString myPhraseLineString =
      theString.mid(myPosInt,myPhraseRegex.matchedLength());
    QString myIdString = myPhraseLineString;
    int myIdPosInt = myIdRegex.indexIn( myPhraseLineString,0);
    if ( myIdPosInt >= 0 )
    {
      //this wont work for me
      //QString myIdString = myPhraseLineString.mid(myIdPosInt,myIdRegex.matchedLength());
      //so I do this kludge
      myIdString = myIdString.replace("taxonKey=","");
      myIdString = myIdString.replace("&","");
      qDebug() <<   "Taxon id found! : " << myIdString ;
      //
      // Set the next part of our query in motion
      //
      QString myUrl = "http://www.secretariat.gbif.net/portal/download_issue.jsp?";
      myUrl += "termsAccepted=true&";
      myUrl += "taxonKey=" + myIdString + "&";
      myUrl += "countryKey=0&";
      myUrl += "resourceKey=0&";
      myUrl += "georeferencedOnly=false&";
      myUrl += "concepts=DateLastModified&";
      myUrl += "concepts=InstitutionCode&";
      myUrl += "concepts=CollectionCode&";
      myUrl += "concepts=CatalogNumber&";
      myUrl += "concepts=ScientificName&";
      myUrl += "concepts=Genus&";
      myUrl += "concepts=Species&";
      myUrl += "concepts=Subspecies&";
      myUrl += "concepts=YearCollected&";
      myUrl += "concepts=Country&";
      myUrl += "concepts=StateProvince&";
      myUrl += "concepts=County&";
      myUrl += "concepts=Locality&";
      myUrl += "concepts=Longitude&";
      myUrl += "concepts=Latitude&";
      myUrl += "concepts=Notes&indexOnly=true&";
      myUrl += "format=0&";
      myUrl += "download=Accept+terms";
      
      return myUrl;
    }
    else
    {
      mMessenger.emitFileNotWritten(mTaxonName);
      return QString();
    }
  }
  //rather dont do this as it blocks the ui
  qDebug ("No id found for name, cancelling search for this taxon!");
  //emit error("Gbif scraper:: The taxon id couldnt find a match in the page returned." );
  mMessenger.emitFileNotWritten(mTaxonName);
  return QString();
}

bool OmgScraperGbif::localitiesRequestDone(QString theString)
{
  //qDebug() << theString;
  if (theString.isEmpty())
  {
    qDebug ("Localities request returned empty");
    mMessenger.emitError("Localities request returned empty");
    return false;
  }

  // System.out.println(myInputLineString);
  // tokenise each line that comes back and search through the
  // tokens...

  //now we parse the file looking for lat long occurrences
  QRegExp myRecordQRegExp(".*\n" );
  //
  //check each regex is valid and if not bail out
  //
  //
  // Record
  //
  if (!myRecordQRegExp.isValid())
  {
    mMessenger.emitError(tr( "GBIF scraper:: the record regex is invalid. Fix it and try again" ));
  }
  else
  {
    //make sure greedy matches are off so if you have a string like
    // <b>blah</b><b>blahblah</b>
    // and your regex is <b>.*</b>
    // non greedy match will return <b>blah</b>
    myRecordQRegExp.setMinimal(true);
  }
  //
  // Main parsing loop
  //
  int myPosInt = 0;    // where we are in the string
  int myCountInt = 0;  // how many matches we find
  bool myFirstRowFlag=true;
  while ( myPosInt >= 0 )
  {
    myPosInt = myRecordQRegExp.indexIn( theString,myPosInt );

    QString myGenusString = "";
    QString mySpeciesString = "";
    QString mySubSpeciesString = "";
    QString myLatitudeString = "";
    QString myLongitudeString = "";
    QString myInstitutionString = "";
    QString myCollectionCodeString = "";
    QString myAccessionCodeString = "";

    if ( myPosInt >= 0 )
    {
      //qDebug() <<   "Match found from pos " << myPosInt << " to " << myPosInt << myRecordQRegExp.matchedLength() ;
      QString myRecordQString =  theString.mid(myPosInt,myRecordQRegExp.matchedLength());
      //skip the length of the matched string
      myPosInt += myRecordQRegExp.matchedLength();

      //first row is header
      if (myFirstRowFlag==true)
      {
        myFirstRowFlag=false;
        continue;
      }

      QStringList myStringArray = myRecordQString.split("\t");
      //some debugging:
      //QListIterator<QString> i(myStringArray);
      //while (i.hasNext())
      //{
      //  qDebug() << i.next() << endl;
      //}

      if (myStringArray.size() < 15)
      {
        continue;
      }
      else
      {
        myGenusString = myStringArray[5];
        mySpeciesString = myStringArray[6];
        mySubSpeciesString = myStringArray[7];
        myLongitudeString = myStringArray[13];
        myLatitudeString = myStringArray[14];
        myInstitutionString =  myStringArray[1];
        myCollectionCodeString = myStringArray[2];
        myAccessionCodeString = myStringArray[3];
      }
      //create a OmgLocality struct and add it to the vector
      if (!myGenusString.isEmpty() && 
              !mySpeciesString.isEmpty() && 
              !myLongitudeString.isEmpty() && 
              !myLatitudeString.isEmpty())
      {
        mMessenger.emitMessage( myGenusString  + " " + mySpeciesString + ", " + myLatitudeString  + ", " + myLongitudeString);

        OmgLocality myLocality;
        QString myId = myInstitutionString + "_" + myCollectionCodeString + "_" + myAccessionCodeString;
        myId = myId.replace(" ","");
        myId = Omgui::xmlEncode(myId);
        myLocality.setId(myId);
        myLocality.setLabel(myGenusString + " " +mySpeciesString);
        myLocality.setLatitude(myLatitudeString.toFloat());
        myLocality.setLongitude(myLongitudeString.toFloat());
        if (!myLocality.isValid())
        {
          continue;
        }
        mLocalityVector.push_back(myLocality);
        myCountInt++;    // count the number of matches
      }
    }
  }
  qDebug() <<   myCountInt << " records found" ;


  //
  // Now build the shapefile
  //
  QString myTextFileName = createTextFile(mFileName);
  if (myTextFileName.isEmpty())
  {
    mMessenger.emitFileNotWritten(mTaxonName);
  }
  else
  {
    createShapefile(mFileName);
    mMessenger.emitFileWritten(mFileName, myTextFileName,mTaxonName,myCountInt);
  }
  //! @TODO clear the spots list check if this is a good idea!
  mLocalityVector.clear();
  qDebug("\n\n ------------------------ end of part 2 of gbif search ------------------- \n\n");
  return true;
}
Q_EXPORT_PLUGIN2(gbif_scraper_plugin, OmgScraperGbif );
