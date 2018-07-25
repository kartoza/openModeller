/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "omgalgorithmparameter.h"
#include "omgui.h"

#include <QDomDocument>

OmgAlgorithmParameter::OmgAlgorithmParameter() : OmgSerialisable()
{}
OmgAlgorithmParameter::~OmgAlgorithmParameter()
{}
OmgAlgorithmParameter::OmgAlgorithmParameter(
    QString theId,
    QString theName,
    QString theDefault,
    QString theOverview,
    QString theDescription,
    QString theMinimum,
    QString theMaximum,
    QString theValue) :
  OmgSerialisable(),
  mId(theId),
  mName(theName),
  mDefault(theDefault),
  mOverview(theOverview),
  mDescription(theDescription),
  mMinimum(theMinimum),
  mMaximum(theMaximum),
  mValue(theValue)
{
}
void OmgAlgorithmParameter::setId(QString theId)
{
  mId=theId;
}
void OmgAlgorithmParameter::setName(QString theName)
{
  mName=theName;
}
void OmgAlgorithmParameter::setType(QString theType)
{
  mType=theType;
}
void OmgAlgorithmParameter::setDefault(QString theDefault)
{
  mDefault=theDefault;
}
void OmgAlgorithmParameter::setOverview(QString theOverview)
{
  mOverview=theOverview;
}
void OmgAlgorithmParameter::setDescription(QString theDescription)
{
  mDescription=theDescription;
}
void OmgAlgorithmParameter::setMinimum(QString theMinimum)
{
  mMinimum=theMinimum;
}
void OmgAlgorithmParameter::setMaximum(QString theMaximum)
{
  mMaximum=theMaximum;
}
void OmgAlgorithmParameter::setValue(QString theValue)
{
  mValue=theValue;
}
QString OmgAlgorithmParameter::id() const
{
  return mId;
}
QString OmgAlgorithmParameter::name() const
{
  return mName;
}
QString OmgAlgorithmParameter::type() const
{
  return mType;
}
QString OmgAlgorithmParameter::defaultValue() const
{
  return mDefault;
}
QString OmgAlgorithmParameter::overview() const
{
  return mOverview;
}
QString OmgAlgorithmParameter::description() const
{
  return mDescription;
}
QString OmgAlgorithmParameter::minimum() const
{
  return mMinimum;
}
QString OmgAlgorithmParameter::maximum() const
{
  return mMaximum;
}
QString OmgAlgorithmParameter::value() const
{
  return mValue;
}
QString OmgAlgorithmParameter::toString() const
{
  QString myString ("ID : " + mId + "\n");
  myString += QString("Name : " + mName + "\n");
  myString += QString("Default : " + mDefault + "\n");
  myString += QString("Description : " + mDescription + "\n");
  myString += QString("Type : " + mType + "\n");
  myString += QString("Minimum : " + mMinimum + "\n");
  myString += QString("Maximum : " + mMaximum + "\n");
  myString += QString("User Value : " + mValue + "\n");
  return myString;
}

QString OmgAlgorithmParameter::toXml() const
{
  QString myString  ("      <Parameter Id=\"" + id() + "\">\n");
  myString +=QString("        <Name>" + Omgui::xmlEncode(name()) + "</Name>\n");
  myString +=QString("        <Type>" + Omgui::xmlEncode(type()) + "</Type>\n");
  myString +=QString("        <Overview>" + Omgui::xmlEncode(overview()) + "</Overview>\n");
  myString +=QString("        <Description>" + Omgui::xmlEncode(description()) + "</Description>\n");
  myString +=QString("        <AcceptedRange Min=\"" + minimum() + "\" Max=\"" + maximum() + "\"/>\n");
  myString +=QString("        <Default>" + defaultValue() + "</Default>\n");
  //@note values is not part of the xsd specification
  //It is needed by omgui for persisting the state of user
  //algorithm profiles and preparing models
  if (value().isNull())
  {
    myString +=QString("        <Value>" + defaultValue() + "</Value>\n");
  }
  else //use the user defined value
  {
    myString +=QString("        <Value>" + value() + "</Value>\n");
  }
  myString +=QString("      </Parameter>\n");
  return myString;

}
QString OmgAlgorithmParameter::toModelCreationXml() const
{
  QString myString  ("      <Parameter Id=\"" + id() + "\" Value=\"");
  if (value().isNull())
  {
    myString += defaultValue() + "\"/>\n";
  }
  else //use the user defined value
  {
    myString += value() + "\"/>\n";
  }
  return myString;

}
bool OmgAlgorithmParameter::fromXml(const QString theXml) 
{
  QDomDocument myDocument("mydocument");
  myDocument.setContent(theXml);
  QDomElement myElement = myDocument.firstChildElement("Parameter");
  if (myElement.isNull())
  {
    //TODO - just make this a warning
    qDebug("AlgorithmParameter - Element could not be found!");
    return false;
  }
  setId(myElement.attribute("Id"));
  setName(myElement.firstChildElement("Name").text());
  setOverview(myElement.firstChildElement("Overview").text());
  setDescription(myElement.firstChildElement("Description").text());
  setType(myElement.firstChildElement("Type").text());
  QDomElement myRangeElement = myElement.firstChildElement("AcceptedRange");
  setMinimum(myRangeElement.attribute("Min"));
  setMaximum(myRangeElement.attribute("Max"));
  setDefault(myElement.firstChildElement("Default").text());
  setValue(myElement.firstChildElement("Value").text());
  return true;
}
