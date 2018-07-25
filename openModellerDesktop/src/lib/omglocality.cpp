/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY{} without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program{} if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "omglocality.h"

//Xande
#include "omgrandom.h"
//#include <openmodeller/os_specific.hh>
#include <algorithm> // needed for random_shuffle

#include <QFile>
#include <QTextStream>
#include <QSettings>

OMG_LIB_EXPORT unsigned int loadLocalities(OmgLocalities& theLocalities, QString theSpeciesFile, QString theLabel)
{
  unsigned int myCounter=0;
  //
  // NOTE the code below duplicates from a bunch of code in omgmodeldesigner
  // the logic should rather be moved into a shared place - perhaps omgspeciesfile
  //

  //
  // Now that we have the localities text file, we need to parse it and find
  //
  //first creation a regex to match text at the beginning of the line
  QRegExp myQRegExp( "^[^#][ a-zA-Z]*" ); //second caret means 'not'
  QFile myQFile( theSpeciesFile );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    theLocalities.clear();
    //now we parse the loc file, checking each line for its taxon
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
    while ( !myQTextStream.atEnd() )
    {
      myCurrentLine = myQTextStream.readLine(); // line of text excluding '\n'
      //split on word boundaries ignoring empty parts
      QStringList myList = myCurrentLine.split(QRegExp("[\t]"));
      //qDebug("Read line : " + myList.join(" -- ").toLocal8Bit());
      if (myCurrentLine.startsWith("#"))
      {
        continue;
      }
      if (myList.size() < 4)
      {
        continue;
      }
      else //new 'proper' format file that has #id \t species name \t lon \t lat
      {
        QString myId=myList.at(0).simplified();
        QString myTaxonName=myList.at(1).simplified();
        //make sure there are only single spaces separating words.
        myTaxonName=myTaxonName.replace( QRegExp(" {2,}"), " " );
        //if this taxon name matches, add its locality to the localites collection
        if (myTaxonName==theLabel)
        {
          OmgLocality myLocality;
          myLocality.setId(myId);
          myLocality.setLabel(myTaxonName);
          myLocality.setLongitude(myList.at(2).simplified().toDouble());
          myLocality.setLatitude(myList.at(3).simplified().toDouble());
          if (myList.size()>4) //abundance
          {
            myLocality.setAbundance(myList.at(4).simplified().toDouble());
          }
          if (myLocality.isValid())
          {
            ++myCounter;
            theLocalities.push_back(myLocality);
          }
        }
      }
    }
    myQFile.close();
  }
  else
  {
    //qDebug("An error occurred in OmgModel while trying to load the localities");
    return myCounter;
  }
  return myCounter;
}

OMG_LIB_EXPORT void splitLocality(const OmgLocalities& theLocalities, OmgLocalities& theFirstSet, OmgLocalities& theSecondSet, double proportion)
{
  // add all samples to an array
  int i;
  int n = theLocalities.count();
  int k = (int) (n * proportion);
  std::vector<int> goToTrainSet(n);

  // first k are set to go to train set
  for ( i = 0; i < k; i++ )
  {
    goToTrainSet[i] = 1;
  }

  // all others are set to go to test set
  for ( ; i < n; i++ )
  {
    goToTrainSet[i] = 0;
  }

  // shuffle elements well
  initRandom();

  std::random_shuffle( goToTrainSet.begin(), goToTrainSet.end() );

  // traverse occurrences copying them to the right sampler
  OmgLocalities::const_iterator it = theLocalities.begin();
  OmgLocalities::const_iterator fin = theLocalities.end();

  i = 0;

  while( it != fin )
  {
    if ( goToTrainSet[i] )
    {
      theFirstSet.push_back( *it );
    }
    else
    {
      theSecondSet.push_back( *it );
    }
    ++i; ++it;
  }
}

OmgLocality::OmgLocality() :
  mId(""),
  mLabel(""),
  mLatitude(0),
  mLongitude(0),
  mAbundance(1) //presume presence
{

}
OmgLocality::~OmgLocality()
{

}
//
// accessors
//
QString OmgLocality::id() const
{
  return mId;
}
QString OmgLocality::label() const
{
  return mLabel;
}
float OmgLocality::latitude() const
{
  return mLatitude;
}
float OmgLocality::longitude() const
{
  return mLongitude;
}
float OmgLocality::abundance() const
{
  return mAbundance;
}
OmgSampleVector OmgLocality::samples() const
{
  return mSamples;
}

//
// Mutators
//
void OmgLocality::setId(QString theId)
{
  mId = theId;
}
void OmgLocality::setLabel(QString theLabel)
{
  mLabel = theLabel;
}
void OmgLocality::setLatitude(float theLatitude)
{
  mLatitude = theLatitude;
}
void OmgLocality::setLongitude(float theLongitude)
{
  mLongitude = theLongitude;
}
void OmgLocality::setAbundance(float theAbundance)
{
  mAbundance = theAbundance;
}
void OmgLocality::setSamples(OmgSampleVector theSamples)
{
  mSamples = theSamples;
}

//helper functions
QString OmgLocality::toString()
{
  QString myString;
  myString = mId + " " + mLabel +  " " + QString::number(mLongitude) + " " + QString::number(mLatitude);
  //loop through all the samples...
  for (int i=0;i<mSamples.count();i++)
  {
    myString+=" " + QString::number(mSamples.at(i));
  }
  return myString;
}
QString OmgLocality::toHtml()
{
  //NOTE we assume that you will be adding html to a table
  QString myString;
  myString="        "; //spacer to prettify html
  myString+="<tr><td>\"";
  myString+=mId;
  myString+="</td><td>";
  myString+=QString::number(mLongitude);
  myString+="</td><td>";
  myString+=QString::number(mLatitude);
  myString+="</td><td>";
  //loop through all the samples...
  for (int i=0;i<mSamples.count();i++)
  {
    myString+=QString::number(mSamples.at(i)) + " ";
  }
  myString+="</td></tr>";
  return myString;

}
QString OmgLocality::toXml()
{
  QString myString;
  myString="        "; //spacer to prettify xml
  myString+="<Point Id=\"" + mId + "\" X=\"";
  myString+=QString::number(mLongitude,'g',17);
  myString+="\" Y=\"";
  myString+=QString::number(mLatitude,'g',17);
  // this should be ignored by the omlib
  // but is used for de/serialising 
  // localities in the desktop so dont remove it
  myString+="\" Abundance=\"";
  myString+=QString::number(mAbundance);
  myString+="\" Sample=\"";
  //loop through all the samples...
  for (int i=0;i<mSamples.count();i++)
  {
    if (i>0) myString += " ";
    myString+=QString::number(mSamples.at(i)) ;
  }
  myString+="\"/>\n";
  return myString;
}

bool OmgLocality::isValid()
{
  if (mId.isEmpty())
  {
    return false;
  }
  if (mLabel.isEmpty())
  {
    return false;
  }
  if (mLatitude==0)
  {
    return false;
  }
  if (mLongitude==0)
  {
    return false;
  }
  return true;
}
    

