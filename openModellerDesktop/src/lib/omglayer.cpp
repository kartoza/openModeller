/***************************************************************************
                          omgserialisable.cpp  -  description
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

#include "omglayer.h"

OmgLayer::OmgLayer() : OmgSerialisable()
{

}
OmgLayer::~OmgLayer()
{

}

OmgLayer::OmgLayer(const OmgLayer& theLayer)
{
  mName=theLayer.name();
  mCategoricalFlag=theLayer.isCategorical();
  mType = theLayer.type();
}

OmgLayer& OmgLayer::operator=(const OmgLayer& theLayer)
{
  if (this == &theLayer) return *this;   // Gracefully handle self assignment

  mName=theLayer.name();
  mCategoricalFlag=theLayer.isCategorical();
  mType=theLayer.type();  
  // Put the normal assignment duties here...
  return *this;
}

QString OmgLayer::name() const
{
  return mName;
}

void OmgLayer::setName(QString theName)
{
  mName=theName;
}

bool OmgLayer::isCategorical() const
{
  return mCategoricalFlag;
}

void OmgLayer::setCategorical(bool theFlag)
{
  mCategoricalFlag = theFlag;
}

OmgLayer::LayerType OmgLayer::type() const
{
  return mType;
}

void OmgLayer::setType(OmgLayer::LayerType theType)
{
  mType = theType;
}

QString OmgLayer::toXml() const 
{
  QString myString = QString("      <Map Id=\"" 
      + mName.toLocal8Bit() 
      + "\" IsCategorical=\"" 
      + (mCategoricalFlag ? "1" : "0")  
      + "\"/>");
  return myString;
}

