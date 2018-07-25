/***************************************************************************
                          omdataseries.cpp  -  description
                             -------------------
    begin                : April 2007
    copyright            : (C) 2007 by Tim Sutton
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

#include "omgdataseries.h"
#include "omgui.h"
#include <algorithm> //needed for sort
OmgDataSeries::OmgDataSeries()
{
  mLineColor = Omgui::randomColour();
  mFillColor = Omgui::randomColour();
}
OmgDataSeries::~OmgDataSeries()
{
}

int OmgDataSeries::size() const
{
  return mYList.size();
}
double OmgDataSeries::xAt(int thePosition)
{
  if (mXList.size() <= thePosition)
  {
    qDebug("Error requesting non existing position in OmgDataSeries::xAt(), returning -9999!");
    QString myString = QString("x list size: %1 position requestd %2 ( %3 line %4 )")
                           .arg(mXList.size()).arg(thePosition).arg(__FILE__).arg(__LINE__);
    qDebug(myString);
    return -9999;
  }
  return mXList.at(thePosition);
}
double OmgDataSeries::yAt(int thePosition)
{
  if (mYList.size() <= thePosition)
  {
    qDebug("Error requesting non existing position in OmgDataSeries::yAt(), returning -9999!");
    QString myString = QString("y list size: %1 position requestd %2 ( %3 line %4 )")
                           .arg(mYList.size()).arg(thePosition).arg(__FILE__).arg(__LINE__);
    qDebug(myString);
    return -9999;
  }
  return mYList.at(thePosition);
}
double OmgDataSeries::xMin() const
{
  bool myFirstFlag=true;
  double myMin=0;
  QListIterator< double > myIterator( mXList );
  while (myIterator.hasNext()) 
  {
    double myVal = myIterator.next();
    if ( myVal < myMin || myFirstFlag )
    {
      myMin=myVal;
    }
    myFirstFlag=false;
  }
  return myMin;
}

double OmgDataSeries::yMin() const
{
  bool myFirstFlag=true;
  double myMin=0;
  QListIterator< double > myIterator( mYList );
  while (myIterator.hasNext()) 
  {
    double myVal = myIterator.next();
    if ( myVal < myMin || myFirstFlag )
    {
      myMin=myVal;
    }
    myFirstFlag=false;
  }
  return myMin;
}

double OmgDataSeries::xMax() const
{
  bool myFirstFlag=true;
  double myMax=0;
  QListIterator< double > myIterator( mXList );
  while (myIterator.hasNext()) 
  {
    double myVal = myIterator.next();
    if ( myVal > myMax || myFirstFlag )
    {
      myMax=myVal;
    }
    myFirstFlag=false;
  }
  return myMax;
}

double OmgDataSeries::yMax() const
{
  bool myFirstFlag=true;
  double myMax=0;
  QListIterator< double > myIterator( mYList );
  while (myIterator.hasNext()) 
  {
    double myVal = myIterator.next();
    if ( myVal > myMax || myFirstFlag )
    {
      myMax=myVal;
    }
    myFirstFlag=false;
  }
  return myMax;
}

void OmgDataSeries::sortX(Qt::SortOrder theOrder)
{
  if (theOrder==Qt::DescendingOrder)
  {
    qSort(mXList.begin(),mXList.end(),qGreater<double>());
  }
  else //ascending sort
  {
    qSort(mXList.begin(),mXList.end());
  }
}

void OmgDataSeries::sortY(Qt::SortOrder theOrder)
{
  if (theOrder==Qt::DescendingOrder)
  {
    qSort(mYList.begin(),mYList.end(),qGreater<double>());
  }
  else //ascending sort
  {
    qSort(mYList.begin(),mYList.end());
  }
}

QList<double> OmgDataSeries::xValuesList() const
{
  return mXList;
}

void OmgDataSeries::setXValuesList(QList<double> theXValues)
{
  mXList=theXValues;
  //autofill the Y axis
  QList<double> myYList;
  for (int myCount=0; myCount < mXList.size(); ++myCount)
  {
    myYList << myCount;
  }
  mYList = myYList;
}

QList<double> OmgDataSeries::yValuesList() const
{
  return mYList;
}

void OmgDataSeries::setYValuesList(QList<double> theYValues)
{
  mYList=theYValues;
  //autofill the X axis
  QList<double> myXList;
  for (int myCount=0; myCount < mYList.size(); ++myCount)
  {
    myXList << myCount;
  }
  mXList = myXList;
}

void OmgDataSeries::setXYValueLists(QList<double> theXValues,QList<double> theYValues)
{
  mXList=theXValues;
  mYList=theYValues;
}

QString OmgDataSeries::xLabel()
{
  return mXLabel;
}

void OmgDataSeries::setXLabel(QString theLabel)
{
  mXLabel=theLabel;
}

QString OmgDataSeries::yLabel()
{
  return mYLabel;
}

void OmgDataSeries::setYLabel(QString theLabel)
{
  mYLabel=theLabel;
}
    
QString OmgDataSeries::label()
{
  return mLabel;
}

void OmgDataSeries::setLabel(QString theLabel)
{
  mLabel=theLabel;
}

QColor OmgDataSeries::lineColor()
{
  return mLineColor;
}

void OmgDataSeries::setLineColor(QColor theColor)
{
  mLineColor=theColor;
}

QColor OmgDataSeries::fillColor()
{
  return mFillColor;
}

void OmgDataSeries::setFillColor(QColor theColor)
{
  mFillColor=theColor;
}

