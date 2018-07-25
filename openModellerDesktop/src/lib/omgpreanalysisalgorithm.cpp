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
#include "omgpreanalysisalgorithm.h"
#include "omgui.h"
#include <QVectorIterator>
#include <QDomDocument>
#include <QDomElement>
OmgPreAnalysisAlgorithm::OmgPreAnalysisAlgorithm() : OmgSerialisable(), OmgGuid(),
                               mId(QString ("[not set]")),
                               mName(QString ("[not set]")),
                               mOverview(QString ("[not set]"))
{
  // mParameters.clear();
}
OmgPreAnalysisAlgorithm::~OmgPreAnalysisAlgorithm()
{}

//
// Mutators
//

void OmgPreAnalysisAlgorithm::setId(QString theId)
{
  mId=theId;
}
void OmgPreAnalysisAlgorithm::setName(QString theName)
{
  mName=theName;
}
void OmgPreAnalysisAlgorithm::setFactoryName(QString theFactoryName)
{
  mFactoryName=theFactoryName;
}
void OmgPreAnalysisAlgorithm::setOverview(QString theOverview)
{
  mOverview=theOverview;

}
void OmgPreAnalysisAlgorithm::setAlgorithm(PreAlgorithm *theAlgorithm)
{
  mAlgorithm = theAlgorithm;
}

void OmgPreAnalysisAlgorithm::setParameters(QVector<OmgAlgorithmParameter> theParameters)
{
  mParameters=theParameters;
}

//
// Accessors
//
QString OmgPreAnalysisAlgorithm::id()
{
  return mId;
}
QString OmgPreAnalysisAlgorithm::name() const
{
  return mName;
}
QString OmgPreAnalysisAlgorithm::factoryName()
{
  return mFactoryName;
}
QString OmgPreAnalysisAlgorithm::overview()
{
  return mOverview;
}
PreAlgorithm *OmgPreAnalysisAlgorithm::algorithm()
{
  return mAlgorithm;
}

QVector<OmgAlgorithmParameter> OmgPreAnalysisAlgorithm::parameters()
{
  return  mParameters;
}
OmgAlgorithmParameter OmgPreAnalysisAlgorithm::parameter(QString theId)
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

//
// end of accessors
//

void OmgPreAnalysisAlgorithm::addParameter(OmgAlgorithmParameter theParameter)
{
  mParameters.push_back(theParameter);
}

int OmgPreAnalysisAlgorithm::parameterCount()
{
  return mParameters.count();

}

//
// Serialisation stuff
//
QString OmgPreAnalysisAlgorithm::toXml() const
{
  // TODO; implement this
  QString myString = QString("[not implemented yet]");

  return myString;
}

bool OmgPreAnalysisAlgorithm::fromXml(const QString theXml)
{
  // TODO; implement this

  return true;
}

QString OmgPreAnalysisAlgorithm::toString() const
{
  QString myString("Id : " + mId+"\n"); 
  myString+=QString("Name : "+mName+"\n"); 
  myString+=QString("Globally Unique Identifier:"+guid()+"\n");
  myString+=QString("Overview : "+ mOverview +"\n --- \n");

  //here we use the new Qt4 java style iterator!
  QVectorIterator<OmgAlgorithmParameter> myIterator(mParameters);
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter=myIterator.next();
    myString +=myParameter.id() + " : " + myParameter.value() + "\n";
  }
  return myString;
}

/* QString OmgPreAnalysisAlgorithm::toHtml() const
{
  QString myString("<table width=\"100%\" border=\"0\">\n"); 
  myString+=QString("<tr><th colspan=\"2\" class=\"glossyBlue\"><h3>" + QObject::tr("Algorithm Name: ") + mName + "</h3></th></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Id :") + "</th><td width=\"75%\"> "+mId+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Version :") + "</th><td> "+mVersion+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("Author :") + "</th><td>"+mAuthor+"</td></tr>\n");
  myString+=QString("<tr><th width=\"25%\"> " + QObject::tr("CodeAuthor :") + "</th><td> " + mCodeAuthor+"</td></tr>\n");
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
} */

