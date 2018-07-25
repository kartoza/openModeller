/***************************************************************************
  omglayerset.cpp  -  description
  -------------------
begin                : March 2006
copyright            : (C) 2006 by Tim Sutton
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

#include "omglayerset.h"
#include <QStringList>
#include <QMapIterator> 
#include <QDebug>
#include <QRegExp>
#include <QDomDocument>
#include <QDomElement>
#include <QDir>

#include <omgui.h>
OmgLayerSet::OmgLayerSet() : OmgSerialisable(), OmgGuid() 
{

}
OmgLayerSet::~OmgLayerSet()
{

}
bool OmgLayerSet::addLayer(OmgLayer theLayer)
{
  //@TODO add some quality control for the added layer?
  mLayersMap[theLayer.name()]=theLayer;
  return true;
}
bool OmgLayerSet::removeLayer(OmgLayer theLayer)
{
  mLayersMap.remove(theLayer.name());
  return true;
}
bool OmgLayerSet::removeLayer(QString theLayer)
{
  mLayersMap.remove(theLayer);
  return true;
}

//
// Accessors
//

QString OmgLayerSet::name() const
{
  return mName;
}
QString OmgLayerSet::description() const
{
  return mDescription;
}
OmgLayer  OmgLayerSet::mask() const
{
  return mMask;
}

//
// Mutators
//

void OmgLayerSet::setName(QString theName)
{
  mName=theName;
}
void OmgLayerSet::setDescription(QString theDescription)
{
  mDescription = theDescription;
}
void OmgLayerSet::setMask(OmgLayer theMask)
{
  mMask = theMask;
}



//
// Ancilliary methods
//

QString OmgLayerSet::maskName() const
{
  return mMask.name();
}

int OmgLayerSet::count() const
{
  return mLayersMap.count();
}

QString OmgLayerSet::toString() const
{
	QString myString ("Layer Set Name: " + mName + "\n");
	myString += "Layer Set Description: " + mDescription + "\n";
	myString += "Layer Set Unique Id: " + guid() + "\n";
  LayersMap::const_iterator myIterator;
  for (myIterator = mLayersMap.begin(); myIterator != mLayersMap.end(); ++myIterator)
  {
    OmgLayer myLayer = myIterator.value();
    myString += myLayer.name() + "\n";
  }
  return myString;
}
OmgLayerSet::LayersMap OmgLayerSet::layers() const
{
  return mLayersMap;
}
QStringList OmgLayerSet::nameList() const
{
  QStringList myList;
  LayersMap::const_iterator myIterator;
  for (myIterator = mLayersMap.begin(); myIterator != mLayersMap.end(); ++myIterator)
  {
    OmgLayer myLayer = myIterator.value();
    myList.append(myLayer.name());
  }
  return myList;

}

QString OmgLayerSet::toXml(bool includeExtraNodes) const
{
  QString myString = QString("    <Environment NumLayers=\"" 
      + QString::number(count())+"\"");
  // Even properly encoded, the content of name and description may cause problems 
  // to the openModeller library if they contain special characters, so sometimes 
  // these nodes should be avoided
  if (includeExtraNodes)
  {
    myString+=QString(" Name=\"" + Omgui::xmlEncode(mName) + "\" Description=\"" 
      + Omgui::xmlEncode(mDescription) 
      + "\" Guid=\"" + guid() + "\"");
  }

  myString+=QString(">\n");

  LayersMap::const_iterator myIterator;
  for (myIterator = mLayersMap.begin(); myIterator != mLayersMap.end(); ++myIterator)
  {
    OmgLayer myLayer = myIterator.value();
    myString += myLayer.toXml() + "\n";
  }
  //ignore categorical attribute for mask
  myString+=QString("      <Mask Id=\"" + mMask.name() + "\"/>\n");
  myString+=QString("    </Environment>\n");

  return myString;
}



QString OmgLayerSet::toHtml() const
{
  //Iterate through creation layers
  QString  myString = QString("<h3>" + mName + "</h3>\n");
  myString += QString("<p>" + mDescription + "</p>\n");
  //myString += QString("<p>GUID: " + guid() + "</p>\n");
  myString += QString("        <ul>\n");
  LayersMap::const_iterator myIterator;
  for (myIterator = mLayersMap.begin(); myIterator != mLayersMap.end(); ++myIterator)
  {
    OmgLayer myLayer = myIterator.value();
    myString+=QString("          <li>"+myLayer.name()+"</li>\n");
  }
  myString+=QString("        </ul>\n");
  myString+="<p>Mask Layer: " + mMask.name() + "</p>\n";
  return myString;
}

bool OmgLayerSet::fromXml(const QString theXml) 
{
  //qDebug("OmgLayerSet::fromXml called....");
  QDomDocument myDocument("mydocument");
  myDocument.setContent(theXml);
  QDomElement myTopElement = myDocument.firstChildElement("Environment");
  //get the name and description attributes from the top level tag
  mName=Omgui::xmlDecode(myTopElement.attribute("Name"));
  mDescription=Omgui::xmlDecode(myTopElement.attribute("Description"));
  setGuid(myTopElement.attribute("Guid"));
  //now get the layer names from the nested tags
  QDomNode myNode = myTopElement.firstChild();
  while(!myNode.isNull()) 
  {
    QDomElement myElement = myNode.toElement(); 
    if(!myElement.isNull()) 
    {
      OmgLayer myLayer;
      myLayer.setName(myElement.attribute("Id"));
      if (myElement.attribute("IsCategorical")=="0")
      {
        myLayer.setCategorical(false);
      }
      else
      {
        myLayer.setCategorical(true);
      }

      //see if its a layer or the mask
      if (myElement.tagName()=="Map")
      {
        addLayer(myLayer);
      }
      else if(myElement.tagName()=="Mask")
      {
        setMask(myLayer);
      }
      else
      {
        qDebug("Unrecognised tag!");
      }
    }
    myNode = myNode.nextSibling();
  }
  return true;
}

bool OmgLayerSet::save(QString theFileName) const
{
  if (theFileName.isEmpty())
  {
    theFileName = Omgui::userLayersetDirPath() + QDir::separator() + guid() + ".xml";
  }
  QFile myFile(theFileName);
  if (!myFile.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return false;
  }
  QString myXml = toXml();
  QTextStream myStream(&myFile);
  myStream << myXml;
  myFile.close();
  qDebug("Layerset saved to: " + theFileName.toLocal8Bit());
  return true;
}
