/***************************************************************************
 *   Copyright (C) 2009 by Pedro Cavalcante                                *
 *   pedro.cavalcante@poli.usp.br                                          *
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

#include "omgpreanalysisalgorithmset.h"
#include "omgmodellerplugininterface.h"
#include "omgalgorithmparameter.h"
#include "omgui.h" //provides anciliary helper functions like getting app paths
#include "omgmodellerpluginregistry.h"
#include <QApplication>
#include <QDebug>
#include <QMapIterator> 
#include <QStringList>
#include <QDomDocument>
#include <QDomElement>
#include <QDir>
#include <cassert>

// this should be removed when the plugin system for Pre-Analysis Algorithms be implemented
#include <openmodeller/pre/PreAlgorithm.hh>
#include <openmodeller/pre/PreChiSquare.hh>
#include <openmodeller/pre/PreJackknife.hh>

OmgPreAnalysisAlgorithmSet::OmgPreAnalysisAlgorithmSet() : OmgSerialisable() 
{

}
OmgPreAnalysisAlgorithmSet::~OmgPreAnalysisAlgorithmSet()
{

}
bool OmgPreAnalysisAlgorithmSet::addAlgorithm(OmgPreAnalysisAlgorithm thePreAnalysisAlgorithm,bool theOverwriteFlag)
{
  //check if algorithm is already in the hash and if it is dont add it
  if (mAlgorithmsMap.contains(thePreAnalysisAlgorithm.name()) && theOverwriteFlag==false)
  {
    //qDebug("Algorithm not added as overwrite mode is false");
    return false;
  }
  //first remove the old one if applicable
  if (mAlgorithmsMap.contains(thePreAnalysisAlgorithm.name()))
  {
    mAlgorithmsMap.remove(thePreAnalysisAlgorithm.name());
  }
  //now we have ensured the alg name is unique or overwriteable, we can add it
  mAlgorithmsMap[thePreAnalysisAlgorithm.name()]=thePreAnalysisAlgorithm;
  return true;
}
bool OmgPreAnalysisAlgorithmSet::removeAlgorithm(OmgPreAnalysisAlgorithm thePreAnalysisAlgorithm)
{
  mAlgorithmsMap.remove(thePreAnalysisAlgorithm.name());
  return true;
}
bool OmgPreAnalysisAlgorithmSet::removeAlgorithm(QString thePreAnalysisAlgorithm)
{
  mAlgorithmsMap.remove(thePreAnalysisAlgorithm);
  return true;
}

//
// Accessors
//

QString OmgPreAnalysisAlgorithmSet::name() const
{
  return mName;
}
QString OmgPreAnalysisAlgorithmSet::description() const
{
  return mDescription;
}

//
// Mutators
//

void OmgPreAnalysisAlgorithmSet::setName(QString theName)
{
  mName=theName;
}
void OmgPreAnalysisAlgorithmSet::setDescription(QString theDescription)
{
  mDescription = theDescription;
}



//
// Ancilliary methods
//

void OmgPreAnalysisAlgorithmSet::clear()
{
  mAlgorithmsMap.clear();
}
int OmgPreAnalysisAlgorithmSet::count() const
{
  return mAlgorithmsMap.count();
}

QString OmgPreAnalysisAlgorithmSet::toString()
{
  //!TODO implement this
  return QString("");
}
QStringList OmgPreAnalysisAlgorithmSet::nameList()
{
  QStringList myList;
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgPreAnalysisAlgorithm myAlgorithm = myIterator.value();
    myList.append(myAlgorithm.name());
  }
  return myList;
}

OmgPreAnalysisAlgorithm OmgPreAnalysisAlgorithmSet::getAlgorithm (QString theGuid)
{
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgPreAnalysisAlgorithm myAlgorithm = myIterator.value();
    if (myAlgorithm.guid() == theGuid)
    {
      return myAlgorithm;
    }
  }
  //this is bad!
  return OmgPreAnalysisAlgorithm();
}

OmgPreAnalysisAlgorithm OmgPreAnalysisAlgorithmSet::getAlgorithmByName (QString name)
{
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgPreAnalysisAlgorithm myAlgorithm = myIterator.value();
    if (myAlgorithm.name() == name)
    {
      return myAlgorithm;
    }
  }
  //this is bad!
  return OmgPreAnalysisAlgorithm();
}


bool OmgPreAnalysisAlgorithmSet::contains(QString theName)
{
  return mAlgorithmsMap.contains(theName);
}
//
// Overloaded operators
//

OmgPreAnalysisAlgorithm OmgPreAnalysisAlgorithmSet::operator [] (int thePosition)
{

  QString myKey = mAlgorithmsMap.keys().at(thePosition);
  if (!myKey.isEmpty())
  {
    return mAlgorithmsMap.value(myKey);
  }

  qDebug("OmgPreAnalysisAlgorithmSet: Algorithm not found.");
  //!TODO we need better error handling here...
  OmgPreAnalysisAlgorithm myAlgorithm;
  return myAlgorithm;
}

//
// Serialisation stuff
//
QString OmgPreAnalysisAlgorithmSet::toXml() const
{
  // TODO; implement this
  QString myString = QString("[not implemented yet]");
  /* QString myString = QString("    <Algorithms count=\"" 
      + QString::number(count())+"\" Name=\"" + mName + "\" Description=\"" + mDescription + "\">\n");
  
  AlgorithmsMap::const_iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    myString += myAlgorithm.toXml() + "\n";
  }
  myString+=QString("    </Algorithms>\n"); */

  return myString;
}



QString OmgPreAnalysisAlgorithmSet::toHtml()
{
  // TODO; implement this
  QString myString = QString("[not implemented yet]");
  /*
  //Iterate through creation algorithms
  QString myString;
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    myString+=myAlgorithm.toHtml();
    myString += "<hr/>\n";
  } */
  return myString;
}

bool OmgPreAnalysisAlgorithmSet::fromXml(const QString theXml)
{
  // TODO; implement this
  /* QDomDocument myDocument("mydocument");
  QString myError;
  int myErrorLine;
  int myErrorColumn;
  if (!myDocument.setContent(theXml, true, &myError, &myErrorLine, &myErrorColumn)) 
  {
    qDebug(QString("Parse error at line %1, column %2:\n%3")
    .arg(myErrorLine)
    .arg(myErrorColumn)
    .arg(myError)
    .toLocal8Bit());
    return false;
  }
  QDomElement myTopElement = myDocument.firstChildElement("Algorithms");
  //QDomElement myTopElement = myDocument.firstChild().toElement();
  //qDebug("Top Element" + myTopElement.tagName().toLocal8Bit());
  //get the name and descriptionattributes from the top level tag
  mName=myTopElement.attribute("Name");
  mDescription=myTopElement.attribute("Description");
  //now get the algorithm names from the nested tags
  QDomElement myElement = myTopElement.firstChildElement("Algorithm");
  while(!myElement.isNull()) 
  {
    if (myElement.tagName()!="Algorithm")
    {
      myElement = myElement.nextSiblingElement();
      continue;
    }
    //get a textual xml representation of the param tag
    QDomDocument myAlgorithmDoc("Algorithm");
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myCopy = myElement.cloneNode().toElement();
    myAlgorithmDoc.appendChild(myCopy);
    QString myXml = myAlgorithmDoc.toString();
    //qDebug("------- Adding alg to algset -----------");
    //now hand over the xml snippet to the alg parameter class to be deserialised
    OmgAlgorithm myAlgorithm;
    myAlgorithm.fromXml(myXml);
    addAlgorithm(myAlgorithm);
    myElement = myElement.nextSiblingElement();
  } */
  return true;
}
    
OmgPreAnalysisAlgorithmSet OmgPreAnalysisAlgorithmSet::getFromActivePlugin()
{
  // TODO: implement this properly
  return OmgPreAnalysisAlgorithmSet();
}

void OmgPreAnalysisAlgorithmSet::loadAlgorithms(QString theSearchDir)
{
  // for now, it ignores theSearchDir parameter

  /* load Chi-Square */
  OmgPreAnalysisAlgorithm chiSquare;
  chiSquare.setName("Chi-Square");
  chiSquare.setId("ChiSquare Pre-Analysis Algorithm");
  chiSquare.setFactoryName("PreChiSquare");

  PreAlgorithm *preAlgorithm;
  preAlgorithm =  new PreChiSquare();
  chiSquare.setAlgorithm(preAlgorithm);
  chiSquare.setOverview(QString::fromStdString(preAlgorithm->getDescription()));
  /* retrieve the parameters 'programatically' */
  /* PreChiSquare::stringMap info;
  preAlgorithm->getAcceptedParameters(info);
  PreChiSquare::stringMap::iterator iterator;
  for(iterator = info.begin(); iterator != info.end(); iterator++)
  {
    qDebug("%s: %s", (iterator->first).c_str(), (iterator->second).c_str());
  } */

  OmgAlgorithmParameter *mParameter;
  mParameter = new OmgAlgorithmParameter();
  mParameter->setId("Sampler");
  mParameter->setName("Sampler");
  mParameter->setType("SamplerPtr");
  mParameter->setDescription("Sampler parameters.");
  chiSquare.addParameter(*mParameter);

  addAlgorithm(chiSquare, false);

  /* load Jackknife */
  OmgPreAnalysisAlgorithm jackknife;
  jackknife.setName("Jackknife");
  jackknife.setId("Jackknife Pre-Analysis Algorithm");
  jackknife.setFactoryName("PreJackknife");

  preAlgorithm =  new PreJackknife();
  jackknife.setOverview(QString::fromStdString(preAlgorithm->getDescription()));
  jackknife.setAlgorithm(preAlgorithm);
  /* retrieve the parameters 'programatically' */
  /* PreJackknife::stringMap infoJk;
  preAlgorithm->getAcceptedParameters(infoJk);
  PreJackknife::stringMap::iterator iteratorJk;
  for(iteratorJk = infoJk.begin(); iteratorJk != infoJk.end(); iteratorJk++)
  {
    qDebug("%s: %s", (iteratorJk->first).c_str(), (iteratorJk->second).c_str());
  } */

  mParameter = new OmgAlgorithmParameter();
  mParameter->setId("Sampler");
  mParameter->setName("Sampler");
  mParameter->setType("SamplerPtr");
  mParameter->setDescription("Sampler parameters.");
  jackknife.addParameter(*mParameter);

  mParameter = new OmgAlgorithmParameter();
  mParameter->setId("Algorithm");
  mParameter->setName("Algorithm");
  mParameter->setType("AlgorithmPtr");
  mParameter->setDescription("Algorithm to be used on the sampler tested.");
  jackknife.addParameter(*mParameter);

  mParameter = new OmgAlgorithmParameter();
  mParameter->setId("PropTrain");
  mParameter->setName("PropTrain");
  mParameter->setType("Real");
  mParameter->setDefault("0.9");
  mParameter->setValue("0.9");
  mParameter->setMinimum("0.0");
  mParameter->setMaximum("1.0");
  mParameter->setDescription("Percentage of points to go to train sampler.");
  jackknife.addParameter(*mParameter);

  addAlgorithm(jackknife, false);
}

