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

#include "omgscrapergbifrest.h"
#include "omgwebpagefetcher.h"
#include "omgui.h"

//QT Includes
#include <QTimer>
#include <QDomDocument>
#include <QDomElement>
#include <QRegExp>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QSettings>
//needed for Q_EXPORT_PLUGIN macro at the end of this file
#include <QtPlugin> 
OmgScraperGbifRest::OmgScraperGbifRest(QObject * parent) : 
  QObject(parent),
  OmgScraperPluginInterface()
{
  qDebug("OmgScraperGbifRest constructor called...");
}


OmgScraperGbifRest::~OmgScraperGbifRest()
{
}

const QString  OmgScraperGbifRest::getName() 
{ 
  return QString("GBIF REST Web Service Plugin"); 
}

const QString OmgScraperGbifRest::getLicense()
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

bool OmgScraperGbifRest::search(QString theTaxonName, QString theFileName)
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
  QString mySearchName=theTaxonName.simplified();
  mySearchName=mySearchName.replace(" ","+");
  QSettings mySettings;
  QString myGbifUrl=mySettings.value("openModeller/gbifPlugin/url", "data.gbif.org").toString();


  //
  // Now we do a list query that allows us
  // to get the actual recs page by page
  //
  //
  
  //maximum allowed by GBIF
  const int myPageSize = 1000; 
  // the number of pages in the dataset
  // we will update this when we get our first query back
  int myPageCount = 0; 
  //useable recs only
  int myRecordCount=0; 
  // a flag to indicate whether we should try to get 
  // the next page of data
  bool myMoreDataFlag=true;
  while (myMoreDataFlag==true)
  {
    QString myStartString = QString::number(myPageCount * myPageSize);
    // An example search url:
    // http://data.gbif.org/ws/rest/occurrence/list?coordinateissues=false
    // &maxresults=1000&startindex=1000&mode=raw&coordinatestatus=true
    // &scientificname=Trifolium+repens&format=brief
    // Where:
    // &coordinatestatus=true  should limit responses to those with coordinates.  
    // &coordinateissues=false to exclude records where the coordinates 
    //                         do not match the supplied country name.
    // 
    // &maxresults=1000        page size to return
    // &startindex=1000        where in the recordset the page should start
    QString myUrl="http://" + myGbifUrl  + "/ws/rest/occurrence/list?scientificName=" 
      + mySearchName + "&format=brief&coordinatestatus=true&coordinateissues=false"
      + "&maxresults=1000&startindex=" + myStartString;
    qDebug ("GBIFRest scraper plugin URL for Search string = " + myUrl.toLocal8Bit());
    OmgWebPageFetcher myWebPageFetcher;
    connect(&myWebPageFetcher, SIGNAL(statusChanged(QString)),
        this, SLOT(setStatus(QString)));
    QString myResult = myWebPageFetcher.getPage(myUrl);
    QDomDocument myDocument("mydocument");
    myDocument.setContent(myResult);
    //
    // Check for the presence of gbif:nextRequestUrl element
    // which indicates there is another page to be fetched
    // after this one...
    //
    QDomNodeList myNextPageList= myDocument.elementsByTagName("gbif:nextRequestUrl");
    if (myNextPageList.length()<1)
    {
      //there are no more pages after this 
      myMoreDataFlag=false;
    }
    
    /*
       <gbif:occurrenceRecords>
        <to:TaxonOccurrence gbifKey="3985067" rdf:about="http://data.gbif.org/ws/rest/occurrence/get/3985067">
         <to:catalogNumber>72336</to:catalogNumber>
         <to:country>Chile</to:country>
         <to:decimalLatitude>-31.6666667</to:decimalLatitude>
         <to:decimalLongitude>-71.2166667</to:decimalLongitude>
         <to:earliestDateCollected>1989-02-04</to:earliestDateCollected>
         <to:identifiedTo>
          <to:Identification>
           <to:taxon>
             <tc:TaxonConcept gbifKey="5369461" rdf:about="http://data.gbif.org/ws/rest/taxon/get/5369461">
              <tc:hasName>
                <tn:TaxonName>
                 <tn:nameComplete>Acacia saligna (Labill.) H. H. Wendl. or Wendl. f.</tn:nameComplete>
                 <tn:genusPart>Acacia</tn:genusPart>
                 <tn:specificEpithet>saligna</tn:specificEpithet>
                 <tn:authorship>(Labill.) H. H. Wendl. or Wendl. f.</tn:authorship>
                 <tn:scientific>true</tn:scientific>
                </tn:TaxonName>
               </tc:hasName>
              </tc:TaxonConcept>
             </to:taxon>
            <to:taxonName>Acacia saligna (Labill.) H. H. Wendl. or Wendl. f.</to:taxonName>
           </to:Identification>
          </to:identifiedTo>
         <to:latestDateCollected>1989-02-04</to:latestDateCollected>
        </to:TaxonOccurrence>
       <gbif:occurrenceRecords>
       */
    //now loop through the occurrence records underneath that

    QDomNodeList myOccurrenceList = myDocument.elementsByTagName("to:TaxonOccurrence");
    for (int myCounter=0; myCounter < myOccurrenceList.size(); myCounter++) 
    {
      QDomNode myRecNode = myOccurrenceList.item(myCounter);
      QDomElement myRecElement = myRecNode.toElement();
      OmgLocality myLocality;
      QString myId = myRecElement.attribute("gbifKey");
      myLocality.setId(myId);
      QString myTaxonName = myRecElement.
        firstChildElement("to:identifiedTo").
        firstChildElement("to:Identification").
        firstChildElement("to:taxon").
        firstChildElement("tc:TaxonConcept").
        firstChildElement("tc:hasName").
        firstChildElement("tn:TaxonName").
        firstChildElement("tn:nameComplete").
        text();
      QString myReturnedName = QString(myTaxonName).simplified();
      if (myReturnedName.isEmpty())
      {
        //fall back to the search string the user used
        myLocality.setLabel(theTaxonName.simplified());
      }
      else
      {
         myLocality.setLabel(myReturnedName);
      }
      QString myLatitude = myRecElement.firstChildElement("to:decimalLatitude").text();
      myLocality.setLatitude(myLatitude.toFloat());
      QString myLongitude = myRecElement.firstChildElement("to:decimalLongitude").text();
      myLocality.setLongitude(myLongitude.toFloat());
      if (!myLocality.isValid())
      {
        continue;
      }
      mLocalityVector.push_back(myLocality);
      ++myRecordCount;
    }
    ++myPageCount;
  }
  qDebug() <<   myRecordCount << " useable records found" ;
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
    mMessenger.emitFileWritten(mFileName, myTextFileName,mTaxonName,myRecordCount);
  }
  //! @TODO clear the spots list check if this is a good idea!
  mLocalityVector.clear();
  return true;
}


void OmgScraperGbifRest::setStatus(QString theStatus)
{
  mMessenger.emitMessage(theStatus); 
}

Q_EXPORT_PLUGIN2(gbifrest_scraper_plugin, OmgScraperGbifRest );
