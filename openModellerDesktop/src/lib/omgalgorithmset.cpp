/***************************************************************************
  omgalgorithmset.cpp  -  description
  -------------------
begin                : March 2006
copyright            : (C) 2003 by Tim Sutton
email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "omgalgorithmset.h"
#include "omgmodellerplugininterface.h"
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
OmgAlgorithmSet::OmgAlgorithmSet() : OmgSerialisable() 
{

}
OmgAlgorithmSet::~OmgAlgorithmSet()
{

}
bool OmgAlgorithmSet::addAlgorithm(OmgAlgorithm theAlgorithm,bool theOverwriteFlag)
{
  //check if algorithm is already in the hash and if it is dont add it
  if (mAlgorithmsMap.contains(theAlgorithm.name()) && theOverwriteFlag==false)
  {
    //qDebug("Algorithm not added as overwrite mode is false");
    return false;
  }
  //first remove the old one if applicable
  if (mAlgorithmsMap.contains(theAlgorithm.name()))
  {
    mAlgorithmsMap.remove(theAlgorithm.name());
  }
  //now we have ensured the alg name is unique or overwriteable, we can add it
  mAlgorithmsMap[theAlgorithm.name()]=theAlgorithm;
  return true;
}
bool OmgAlgorithmSet::removeAlgorithm(OmgAlgorithm theAlgorithm)
{
  mAlgorithmsMap.remove(theAlgorithm.name());
  return true;
}
bool OmgAlgorithmSet::removeAlgorithm(QString theAlgorithm)
{
  mAlgorithmsMap.remove(theAlgorithm);
  return true;
}

//
// Accessors
//

QString OmgAlgorithmSet::name() const
{
  return mName;
}
QString OmgAlgorithmSet::description() const
{
  return mDescription;
}

//
// Mutators
//

void OmgAlgorithmSet::setName(QString theName)
{
  mName=theName;
}
void OmgAlgorithmSet::setDescription(QString theDescription)
{
  mDescription = theDescription;
}



//
// Ancilliary methods
//

void OmgAlgorithmSet::clear()
{
  mAlgorithmsMap.clear();
}
int OmgAlgorithmSet::count() const
{
  return mAlgorithmsMap.count();
}

QString OmgAlgorithmSet::toString()
{
  //!TODO implement this
  return QString("");
}
QStringList OmgAlgorithmSet::nameList()
{
  QStringList myList;
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    myList.append(myAlgorithm.name());
  }
  return myList;
}

OmgAlgorithm OmgAlgorithmSet::getAlgorithm (QString theGuid)
{
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    if (myAlgorithm.guid() == theGuid)
    {
      return myAlgorithm;
    }
  }
  //this is bad!
  return OmgAlgorithm();
}

bool OmgAlgorithmSet::contains(QString theName)
{
  return mAlgorithmsMap.contains(theName);
}
//
// Overloaded operators
//

OmgAlgorithm OmgAlgorithmSet::operator [] (int thePosition)
{

 QString myKey = mAlgorithmsMap.keys().at(thePosition);
 if (!myKey.isEmpty())
 {
   return mAlgorithmsMap.value(myKey);
 }
 //!TODO we need better error handling here...
 OmgAlgorithm myAlgorithm;
 return myAlgorithm;
}

//
// Serialisation stuff
//
QString OmgAlgorithmSet::toXml() const
{
  QString myString = QString("    <Algorithms count=\"" 
      + QString::number(count())+"\" Name=\"" + mName + "\" Description=\"" + mDescription + "\">\n");
  
  AlgorithmsMap::const_iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    myString += myAlgorithm.toXml() + "\n";
  }
  myString+=QString("    </Algorithms>\n");

  return myString;
}



QString OmgAlgorithmSet::toHtml()
{
  //Iterate through creation algorithms
  QString myString;
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    myString+=myAlgorithm.toHtml();
    myString += "<hr/>\n";
  }
  return myString;
}

bool OmgAlgorithmSet::fromXml(const QString theXml)
{
  QDomDocument myDocument("mydocument");
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
  }
  return true;
}
    
OmgAlgorithmSet OmgAlgorithmSet::getFromActivePlugin()
{
  /* This is the Abstract plugin type. Depending on the users
   * options, it will be initialised to either the OmgModellerLocalPlugin
   * or OmgModellerWebServices plugin (or possibly other plugin types in 
   * the future) 
   */
  //qDebug("AlgorithmSet calling OmGui to get the modeller plugin");
  //note this pointer should *not* be deleted afterwards because its managed by the registry!!
  OmgModellerPluginInterface * mypModellerPlugin = OmgModellerPluginRegistry::instance()->getPlugin();
  //qDebug("AlgorithmSet designer testing if returned modeller plugin is ok");
  if(!mypModellerPlugin)
  {
    //this is bad! TODO notify user he has no useable adapters
    //TODO handle this more gracefully than asserting!
    //qDebug("AlgorithmSet Error no valid modelling adapters could be loaded");
    //qDebug("No modelling plugins could be found.\nPlease report this problem to you system administrator or the openModeller developers.");
    //assert ("Undefined adapter type in __FILE__  , line  __LINE__");
    OmgAlgorithmSet myAlgorithmSet;
    return myAlgorithmSet;
  }
  else
  {
    //qDebug("Plugin is good to go....");
  }
  //now we can go on to use our plugin...
  OmgAlgorithmSet myAlgorithmSet = mypModellerPlugin->getAlgorithmSet();
  return myAlgorithmSet;
}

void OmgAlgorithmSet::loadAlgorithms(QString theSearchDir)
{
  QDir myDirectory(theSearchDir);
  myDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks );
  QFileInfoList myList = myDirectory.entryInfoList();
  for (unsigned int i = 0; i < static_cast<unsigned int>(myList.size()); ++i)
  {
    QFileInfo myFileInfo = myList.at(i);
    //Ignore directories
    if(myFileInfo.fileName() == "." ||myFileInfo.fileName() == ".." )
    {
      continue;
    }
    //if the filename ends in .xml try to load it into our layerSets listing
    if(myFileInfo.completeSuffix()=="xml")
    {
      OmgAlgorithm myAlgorithm;
      myAlgorithm.fromXmlFile(myFileInfo.filePath() );
      if (myAlgorithm.name().isEmpty())
      {
        continue;
      }
      //define the origin of the algorithm - this is used by e.g. the algmanager
      //in the gui to differentiate between user profiles and profiles 
      //obtained from the plugin
      //I have created some default alg proiles which should be 
      //treated as system algs. These are for algs like svm that
      //have several 'sub algorithms'.
      if (myFileInfo.fileName().startsWith("algorithm"))
      {
        myAlgorithm.setOrigin(OmgAlgorithm::ADAPTERPROFILE);
      }
      else
      {
        myAlgorithm.setOrigin(OmgAlgorithm::USERPROFILE);
      }
      addAlgorithm(myAlgorithm);
    }
  }
}

void OmgAlgorithmSet::saveAlgorithms(QString theSaveDir, bool theUserProfilesFlag)
{
  AlgorithmsMap::iterator myIterator;
  for (myIterator = mAlgorithmsMap.begin(); myIterator != mAlgorithmsMap.end(); ++myIterator)
  {
    OmgAlgorithm myAlgorithm = myIterator.value();
    if ((myAlgorithm.origin()!=OmgAlgorithm::USERPROFILE) && theUserProfilesFlag)
    {
      //flag says only write alg profiles to disk that are tagged as origin user profile
      //so skipe this one...
      continue;
    }
    QString myString = myAlgorithm.toXml();
    QFile myFile(theSaveDir + myAlgorithm.guid() + ".xml");
    if (!myFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      continue;
    }
    QTextStream myStream(&myFile);
    myStream << myString;
    myFile.close();
  }
}

