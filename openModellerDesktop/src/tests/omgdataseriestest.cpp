/***************************************************************************
 *   Copyright (C) 2007 by Tim Sutton   *
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
#include <QtTest/QtTest>
#include <omgdataseries.h>
#include <QColor>
#include <QString>

class OmgDataSeriesTest: public QObject
{
  Q_OBJECT;
  private slots:
  void testBasicFunctions();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup();// will be called after every testfunction.
  
  private:
  OmgDataSeries mDataSeries;
};

void OmgDataSeriesTest::initTestCase()
{
}
void OmgDataSeriesTest::cleanup()
{
}
void OmgDataSeriesTest::cleanupTestCase()
{
}

void OmgDataSeriesTest::testBasicFunctions()
{
  mDataSeries << 2.0 ;
  mDataSeries << 3.0 ;
  // chaining should work too...
  mDataSeries << 5.0 << 8.0;
  //so should adding via qpair
  QPair <double,double> myPair;
  myPair.first = 5.0;
  myPair.second = 7.0;
  mDataSeries << myPair;
  QVERIFY(mDataSeries.size()==5);
  QVERIFY(mDataSeries.yMin()==2.0);
  QVERIFY(mDataSeries.yMax()==8.0);
  QVERIFY(mDataSeries.xMin()==0.0);
  QVERIFY(mDataSeries.xMax()==5.0);
  QVERIFY(mDataSeries.xAt(0)==0);
  qDebug("yAt(0) :");
  qDebug(QString::number(mDataSeries.yAt(0)));
  QVERIFY(mDataSeries.yAt(0)==2.0);
  QColor myColor = Qt::red;
  mDataSeries.setLineColor(myColor);
  QVERIFY(mDataSeries.lineColor()==Qt::red);
  mDataSeries.setFillColor(myColor);
  QVERIFY(mDataSeries.fillColor()==Qt::red);
  mDataSeries.setLabel("Test");
  QVERIFY(mDataSeries.label()=="Test");
  mDataSeries.sortY();//descending sort
  QVERIFY(mDataSeries.yValuesList().first()==8.0);
  mDataSeries.sortY(Qt::AscendingOrder);
  QVERIFY(mDataSeries.yValuesList().last()==8.0);
}
QTEST_MAIN(OmgDataSeriesTest) 
#include "moc_omgdataseriestest.cxx"


