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

#include "omgscrapersplink.h"
#include "omgwebpagefetcher.h"

//QT Includes
#include <QTimer>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
//needed for Q_EXPORT_PLUGIN macro at the end of this file
#include <QtPlugin> 
OmgScraperSplink::OmgScraperSplink(QObject * parent) :
  QObject(parent),
  OmgScraperPluginInterface()
{
  qDebug("OmgScraperSplink constructor called...");
}


OmgScraperSplink::~OmgScraperSplink()
{
}

const QString  OmgScraperSplink::getName() 
{ 
  return QString("speciesLink Plugin"); 
}

const QString OmgScraperSplink::getLicense()
{
  QFile myQFile( ":/splink_terms.txt" );
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
    myString="Terms and conditions document for speciesLink could not be retrieved.";
  }
  return myString; 
}

bool OmgScraperSplink::search(QString theTaxonName, QString theFileName)
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
  QString mySearch=theTaxonName.simplified();
  mySearch=mySearch.replace(" ","&species=");
  //QString myUrl= "http://www.google.com.br";
  QString myUrl= "http://splink.cria.org.br/bdworld/service?";
  myUrl+= "genus=";
  myUrl+= mySearch;

  qDebug ("SPlink scraper plugin URL for Search string = " + myUrl.toLocal8Bit());
  OmgWebPageFetcher myWebPageFetcher;
  connect(&myWebPageFetcher, SIGNAL(statusChanged(QString)),
          this, SLOT(setStatus(QString)));
  QString myResult = myWebPageFetcher.getPage(myUrl);
  qDebug("Web request Result");
  qDebug() << myResult;
  int myCount=0; 
  QDomDocument myDocument("mydocument");
  myDocument.setContent(myResult);
  QDomElement myTopElement = myDocument.firstChildElement("ResultSet");
  if (myTopElement.isNull())
  {
    qDebug("Top (ResultSet) element could not be found!");
    return false;
  }
  QDomElement myElement = myTopElement.firstChildElement();
  while(!myElement.isNull()) 
  {
    if (myElement.tagName()!="Record")
    {
      myElement = myElement.nextSiblingElement();
      continue;
    }
    qDebug("SPLink Parser found a Record");
    OmgLocality myLocality;
    QString myId = myElement.firstChildElement("AccessionCode").text();
    myLocality.setId(myId);
    QString myGenus = myElement.firstChildElement("Genus").text();
    QString mySpecies = myElement.firstChildElement("Species").text();
    myLocality.setLabel(myGenus + " " + mySpecies);
    QString myLatitude = myElement.firstChildElement("Latitude").text();
    myLocality.setLatitude(myLatitude.toFloat());
    QString myLongitude = myElement.firstChildElement("Longitude").text();
    myLocality.setLongitude(myLongitude.toFloat());
    myElement = myElement.nextSiblingElement();
    if (!myLocality.isValid())
    {
      continue;
    }
    mLocalityVector.push_back(myLocality);
    myCount++;    // count the number of matches
  }
  qDebug() <<   myCount << " records found" ;
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
    mMessenger.emitFileWritten(mFileName, myTextFileName,mTaxonName,myCount);
  }
  //! @TODO clear the spots list check if this is a good idea!
  mLocalityVector.clear();
  return true;
}
Q_EXPORT_PLUGIN2(splink_scraper_plugin, OmgScraperSplink );
