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
#include "omgalgorithm.h"
#include "omgui.h"
#include <QVectorIterator>
#include <QDomDocument>
#include <QDomElement>
OmgAlgorithm::OmgAlgorithm() : OmgSerialisable(), OmgGuid(),
                               mId(QString ("[not set]")),
                               mName(QString ("[not set]")),
                               mAuthor(QString( "[not set]")),
                               mCodeAuthor(QString ("[not set]")),
                               mContact(QString ("[not set]")),
                               mVersion(QString ("[not set]")),
                               mCategorical(0),
                               mAbsence(0),
                               mOverview(QString ("[not set]")),
                               mDescription(QString ("[not set]")),
                               mBibliography(QString ("[not set]")),
                               mOrigin(UNDEFINED)

{
  mParameters.clear();
}
OmgAlgorithm::~OmgAlgorithm()
{}

//
// Mutators
//

void OmgAlgorithm::setId(QString theId)
{
  mId=theId;
}
void OmgAlgorithm::setName(QString theName)
{
  mName=theName;
}
void OmgAlgorithm::setAuthor(QString theAuthor)
{
  mAuthor=theAuthor;
}
void OmgAlgorithm::setCodeAuthor(QString theCodeAuthor)
{
  mCodeAuthor=theCodeAuthor;
}
void OmgAlgorithm::setContact(QString theContact)
{
  mContact=theContact;
}
void OmgAlgorithm::setVersion(QString theVersion)
{
  mVersion=theVersion;
}
void OmgAlgorithm::setCategorical(int theCategorical)
{
  mCategorical=theCategorical;
}
void OmgAlgorithm::setAbsence(int theAbsence)
{
  mAbsence=theAbsence;
}
void OmgAlgorithm::setOverview(QString theOverview)
{
  mOverview=theOverview;

}
void OmgAlgorithm::setDescription(QString theDescription)
{
  mDescription=theDescription;
}
void OmgAlgorithm::setBibliography(QString theBibliography)
{
  mBibliography=theBibliography;
}
void OmgAlgorithm::setParameters(QVector<OmgAlgorithmParameter> theParameters)
{
  mParameters=theParameters;
}
void OmgAlgorithm::setOrigin(Origin theOrigin)
{
  mOrigin=theOrigin;
}
//
// Accessors
//
QString OmgAlgorithm::id()
{
  return mId;
}
QString OmgAlgorithm::name() const
{
  return mName;
}
QString OmgAlgorithm::author()
{
  return mAuthor;
}
QString OmgAlgorithm::codeAuthor()
{
  return mCodeAuthor;
}
QString OmgAlgorithm::contact()
{
  return mContact;
}
QString OmgAlgorithm::version()
{
  return mVersion;
}
int OmgAlgorithm::categorical()
{
  return mCategorical;
}
int OmgAlgorithm::absence()
{
  return mAbsence;
}
QString OmgAlgorithm::overview()
{
  return mOverview;
}
QString OmgAlgorithm::description()
{
  return mDescription;
}
QString OmgAlgorithm::bibliography()
{
  return mBibliography;
}
QVector<OmgAlgorithmParameter> OmgAlgorithm::parameters()
{
  return  mParameters;
}
OmgAlgorithmParameter OmgAlgorithm::parameter(QString theId)
{
    //here we use the new Qt4 java style iterator!
    QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
    OmgAlgorithmParameter myParameter;
    myParameter.setId("invalid");
    while (myIterator.hasNext())
    {
      myParameter=myIterator.next();
      if (myParameter.id()==theId)
      {
        return myParameter;
      }
    }
    //!TODO implement better error handling
    return myParameter;
}

OmgAlgorithm::Origin OmgAlgorithm::origin()
{
  return mOrigin;
}
//
// end of accessors
//

void OmgAlgorithm::addParameter(OmgAlgorithmParameter theParameter)
{
  mParameters.push_back(theParameter);
}

int OmgAlgorithm::parameterCount()
{
  return mParameters.count();

}
QString OmgAlgorithm::toXml() const
{
  QString myString("  <Algorithm  Id=\"" + mId +  "\" Version=\"" + mVersion + "\">\n");
  //alg metadata is not required when submitting models to om
  //but is used in other parts of the gui
  myString+=QString("  <Name>" + Omgui::xmlEncode(mName) + "</Name>\n " );
  myString+=QString("  <Overview>"+Omgui::xmlEncode(mOverview)+"</Overview>\n");
  myString+=QString("  <Description>"+Omgui::xmlEncode(mDescription)+"</Description>\n");
  //@todo make designers a collection in this class instead of just a string member
  myString+=QString("  <Designers>\n" );
  //@TODO Adda  designer contact field to this class and then update contact below
  myString+=QString("    <Designer Name=\"" + Omgui::xmlEncode(mAuthor) + "\" Contact=\"\"/>\n" );
  myString+=QString("  </Designers>\n" );
  myString+=QString("  <Bibliography>"+Omgui::xmlEncode(mBibliography)+"</Bibliography>\n");
  //@todo make developers a collection in this class instead of just a string member
  myString+=QString("  <Developers>\n");
  myString+=QString("    <Developer Name=\"" + Omgui::xmlEncode(mCodeAuthor) + "\" Contact=\"" + mContact + "\"/>\n" );
  myString+=QString("  </Developers>\n" );
  myString+=QString("  <Guid>" + guid() + "</Guid>\n" );
  myString+=QString("  <AcceptsCategoricalMaps>" + QString::number(mCategorical) + "</AcceptsCategoricalMaps>\n" );
  myString+=QString("  <RequiresAbsencePoints>" + QString::number(mAbsence) + "</RequiresAbsencePoints>\n" );
  myString+=QString("  <Parameters>\n");
  //here we use the new Qt4 java style iterator!
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString += myParameter.toXml();
  }
  myString +=QString( "    </Parameters>\n");
  //enum Origin {ADAPTERPROFILE=100,USERPROFILE=200,UNDEFINED=300};
  myString+=QString("  <Origin>");
  if(mOrigin==OmgAlgorithm::ADAPTERPROFILE)
  {
    myString+=QString("ADAPTERPROFILE");
  }
  else if(mOrigin==OmgAlgorithm::USERPROFILE)
  {
    myString+=QString("USERPROFILE");
  }
  else if(mOrigin==OmgAlgorithm::UNDEFINED)
  {
    myString+=QString("UNDEFINED");
  }
  myString+=QString("</Origin>\n " );

  //TODO work out how to serialise and deserialise the model itself e.g.
  //+"      <BioclimModel Mean="271.1538461538461319833004 271.1538461538461319833004 271.1538461538461319833004" StdDev="12.19477741305047935327366 12.19477741305047935327366 12.19477741305047935327366" Minimum="6 6 6" Maximum="1141 1141 1141"/>\n"
  myString+=QString("  </Algorithm>\n");
  return myString;
}

QString OmgAlgorithm::toModelCreationXml() const
{
  QString myString("  <Algorithm  Id=\"" + mId +  "\" Version=\"" + mVersion + "\">\n");
  myString +=QString( "    <Parameters>\n");
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString += myParameter.toModelCreationXml();
  }
  myString += QString( "    </Parameters>\n");
  myString += QString("  </Algorithm>");
  return myString;
}

QString OmgAlgorithm::toSerializedModelXml(const QString theNormalizationXml, const QString theModelDefinitionXml) const
{
  QString myString("  <Algorithm  Id=\"" + mId +  "\" Version=\"" + mVersion + "\">\n");
  myString +=QString( "    <Parameters>\n");
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString += myParameter.toModelCreationXml();
  }
  myString += QString( "    </Parameters>\n");
  if (!theNormalizationXml.isEmpty())
  {
    myString += theNormalizationXml;
  }
  myString += theModelDefinitionXml;
  myString += QString("  </Algorithm>");
  return myString;
}

QString OmgAlgorithm::toModelProjectionXml(QString theModel, QString theNormalization) const
{
  QString myString("  <Algorithm  Id=\"" + mId +  "\" Version=\"" + mVersion + "\">\n");
  myString +=QString( "    <Parameters>\n");
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString += myParameter.toModelCreationXml();
  }
  myString +=QString( "    </Parameters>\n");
  //include the model tag as returned by libopenModelller
  //algorithms when they serialise
  myString += theModel;
  if (!theNormalization.isEmpty())
  {
    myString += theNormalization;
  }
  myString+=QString("  </Algorithm>");
  return myString;
}

bool OmgAlgorithm::fromXml(const QString theXml) 
{
  QDomDocument myDocument("mydocument");
  myDocument.setContent(theXml);
  QDomElement myTopElement = myDocument.firstChildElement("Algorithm");
  if (myTopElement.isNull())
  {
    //TODO - just make this a warning
    qDebug("top element could not be found!");
  }
  mId=myTopElement.attribute("Id");
  mVersion=myTopElement.attribute("Version");
  mName=Omgui::xmlDecode(myTopElement.firstChildElement("Name").text());
  mOverview=myTopElement.firstChildElement("Overview").text();
  mDescription=myTopElement.firstChildElement("Description").text();
  mBibliography=myTopElement.firstChildElement("Bibliography").text();
  mCategorical=myTopElement.firstChildElement("AcceptsCategoricalMaps").text().toInt();
  mAbsence=myTopElement.firstChildElement("RequiresAbsencePoints").text().toInt();
  //iterate through the nested designer details
  //@TODO update the class member to be a collection rather than a single string
  QDomElement myDesignersElement = myTopElement.firstChildElement("Designers");
  QDomElement myDesignerElement= myDesignersElement.firstChildElement("Designer");
  mAuthor="";
  //@TODO add author contact class member
  while(!myDesignerElement.isNull()) 
  {
    if (myDesignerElement.tagName()!="Designer")
    {
      myDesignerElement = myDesignerElement.nextSiblingElement();
      continue;
    }
    mAuthor+=myDesignerElement.attribute("Name");
    mAuthor+=" (" + myDesignerElement.attribute("Contact") + ")";
    myDesignerElement = myDesignerElement.nextSiblingElement();
  }
  //iterate through the nested developer details
  //@TODO update the class member to be a collection rather than a single string
  QDomElement myDevelopersElement = myTopElement.firstChildElement("Developers");
  QDomElement myDeveloperElement= myDevelopersElement.firstChildElement("Developer");
  mCodeAuthor="";
  mContact="";
  while(!myDeveloperElement.isNull()) 
  {
    if (myDeveloperElement.tagName()!="Developer")
    {
      myDeveloperElement = myDeveloperElement.nextSiblingElement();
      continue;
    }
    mCodeAuthor+=myDeveloperElement.attribute("Name");
    mContact+=myDeveloperElement.attribute("Contact");
    myDeveloperElement = myDeveloperElement.nextSiblingElement();
  }
  QString myGuid = myTopElement.firstChildElement("Guid").text();
  //assign a Guid if none was provided
  if (myGuid.isEmpty())
  {
    setGuid();
    //also assume origin to be from plugin if none was provided
    setOrigin(ADAPTERPROFILE);
  }
  else
  {
    setGuid(myGuid);
  }
  //now get the algs'parameters from the nested tags
  QDomElement myParametersElement = myTopElement.firstChildElement("Parameters");
  QDomElement myElement = myParametersElement.firstChildElement();
  while(!myElement.isNull()) 
  {
    if (myElement.tagName()!="Parameter")
    {
      myElement = myElement.nextSiblingElement();
      continue;
    }
    OmgAlgorithmParameter myParameter;
    //get a textual xml representation of the param tag
    QDomDocument myParamDoc("parameter");
    //note we need to do a deep copy here because the
    //element is shared otherwise and when we
    //reparent it the loop stops after the first node
    //as no more siblings are found!
    QDomElement myCopy = myElement.cloneNode().toElement();
    myParamDoc.appendChild(myCopy);
    QString myXml = myParamDoc.toString();
    //now hand over the xml snippet to the alg parameter class to be deserialised
    myParameter.fromXml(myXml);
    addParameter(myParameter);
    myElement = myElement.nextSiblingElement();
  }
  QString mOriginString=Omgui::xmlDecode(myTopElement.firstChildElement("Origin").text());
  if(mOriginString=="ADAPTERPROFILE")
  {
    mOrigin=OmgAlgorithm::ADAPTERPROFILE;
  }
  else if(mOriginString=="USERPROFILE")
  {
    mOrigin=OmgAlgorithm::USERPROFILE;
  }
  else if(mOriginString=="UNDEFINED")
  {
    mOrigin=OmgAlgorithm::UNDEFINED;
  }
  return true;
}

QString OmgAlgorithm::toString() const
{
  QString myString("Id : " + mId+"\n"); 
  myString+=QString("Name : "+mName+"\n"); 
  myString+=QString("Version : "+mVersion+"\n");
  myString+=QString("Author :"+mAuthor+"\n"); 
  myString+=QString("CodeAuthor : " + mCodeAuthor+"\n"); 
  myString+=QString("Contact :"+mContact+"\n");
  myString+=QString("Globally Unique Identifier:"+guid()+"\n");
  myString+=QString("Categorical data supported: "+ QString::number(mCategorical)+"\n --- \n");
  myString+=QString("Absence data supported : "+ QString::number(mAbsence)+"\n --- \n");
  myString+=QString("Overview : "+ mOverview +"\n --- \n");
  myString+=QString("Description : "+ mDescription +"\n --- \n");

  //here we use the new Qt4 java style iterator!
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString +=myParameter.id() + " : " + myParameter.value() + "\n";
  }
  return myString;
}
QString OmgAlgorithm::toHtml() const
{
  QString myString("<table width=\"100%\" border=\"0\">\n"); 
  myString+=QString("<tr><th colspan=\"2\" class=\"glossyBlue\"><h3>" + QObject::tr("Algorithm Name: ") + mName + "</h3></th></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Id :") + "</th><td width=\"75%\"> "+mId+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Version :") + "</th><td> "+mVersion+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Author :") + "</th><td>"+mAuthor+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Code developer :") + "</th><td> " + mCodeAuthor+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Contact :") + "</th><td>"+mContact+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Guid :") + "</th><td>"+guid()+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Categorical data supported:") + "</th><td> "+ QString::number(mCategorical)+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Absence data supported :") + "</th><td> "+ QString::number(mAbsence)+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Overview :") + "</th><td> "+ mOverview +"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Description :") + "</th><td> "+ mDescription +"</td></tr>\n");
  if (mParameters.size())
  {
    //make a mini table for the parameters
    myString+=QString("<tr><td colspan=\"2\">\n");
    myString+=QString("<table width=\"100%\" cellspacing=\"0\" cellpadding=\"2\">\n"); 
    myString+=QString("<tr><th class=\"glossyBlue\"> " + QObject::tr("Id") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Name") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Value") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Default") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Min") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Max") + "</th>\n");
    myString+=QString("<th class=\"glossyBlue\"> " + QObject::tr("Type") + "</th></tr>\n");

    //here we use the new Qt4 java style iterator!
    QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
    while (myIterator.hasNext())
    {
      OmgAlgorithmParameter myParameter=myIterator.next();
      myString+=QString("<tr><td class=\"parameterHeader\"><b>" + myParameter.id() + "</b></td>\n");
      myString+=QString("<td> "+ myParameter.name() +"</td>\n");
      myString+=QString("<td class=\"parameterHeader\"> "+ myParameter.value() +"</td>\n");
      myString+=QString("<td> "+ myParameter.defaultValue() +"</td>\n");
      myString+=QString("<td class=\"parameterHeader\"> "+ myParameter.minimum() +"</td>\n");
      myString+=QString("<td> "+ myParameter.maximum() +"</td>\n");
      myString+=QString("<td class=\"parameterHeader\"> "+ myParameter.type() +"</td></tr>\n");
      myString+=QString("<tr>\n");
      myString+=QString("<td class=\"parameterHeader\">" + QObject::tr("Description: ") + "</td><td colspan=\"6\"><b> " + "</b> "+ myParameter.description() +"</td>\n");
      myString+=QString("</tr>\n");
    }
    //end of alg parameters mini table
    myString +=QString("</td></tr></table>\n"); 
  }
  //end of alg table
  myString +=QString("</table>\n"); 
  return myString;
}
