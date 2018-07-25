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
#include <omggraph.h>
#include <omgdataseries.h>
#include <QImage>
#include <QList>

class OmgGraphTest: public QObject
{
  Q_OBJECT;
  private slots:
  void testRemoveSeries();
  void testRenderGraph();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup(){};// will be called after every testfunction.
  private:
  //some dummy data
  OmgDataSeries mSeries1;
  OmgDataSeries mSeries2;
  OmgDataSeries mSeries3;
  QImage mImage;
  QPainter * mpPainter;
};

void OmgGraphTest::initTestCase()
{
  //prepare some dummy data for charting
  QList <double> myList1;
  myList1 << -0.5 << 0.8 << 1.0 << 0.8 << 1.0 << 0.6;
  mSeries1.setLabel( "Test Series 1" );
  mSeries1.setYValuesList( myList1 );
  QVERIFY(mSeries1.xAt(0)==0);
  mSeries1.setFillColor(Qt::red);
  mSeries1.setLineColor(Qt::darkRed);
  //
  mSeries2 << -0.3 << 0.1 << 0.5 << 0.3 << 0.1 << 0.9;
  mSeries2.setLabel ( "Test Series 2" );
  mSeries2.setFillColor(Qt::green);
  mSeries2.setLineColor(Qt::darkGreen);
  //
  mSeries3 << -0.7 << 0.5 << 0.3 << 0.4 << 0.3 << 0.6;
  mSeries3.setLabel( "Test Series 3" );
  mSeries3.setFillColor(Qt::blue);
  mSeries3.setLineColor(Qt::darkBlue);
  //
  //make an image to draw the graph onto
  mImage = QImage( 800,800,QImage::Format_ARGB32 );
  mImage.fill(Qt::white);
  mpPainter = new QPainter ( &mImage );
}
void OmgGraphTest::cleanupTestCase()
{
  delete mpPainter;
}
void OmgGraphTest::testRemoveSeries()
{
  //qDebug(mSeries1.first);
  //qDebug(QString::number(mSeries1.second.count()));
  OmgGraph myGraph(mpPainter);
  myGraph.addSeries(mSeries1);
  myGraph.addSeries(mSeries2);
  myGraph.addSeries(mSeries3);
  //qDebug("Series count:");
  //qDebug(QString::number(myGraph.seriesCount()));
  QVERIFY(myGraph.seriesCount()==3);
  //try to remove a non existing series
  QVERIFY(!myGraph.removeSeriesAt(10));
  //try to remove all series
  QVERIFY(myGraph.removeSeriesAt(1));
  //two is now one
  QVERIFY(myGraph.removeSeriesAt(1));
  QVERIFY(myGraph.removeSeriesAt(0));
  //qDebug("Series count:");
  //qDebug(QString::number(myGraph.seriesCount()));
  QVERIFY(myGraph.seriesCount()==0);
}
void OmgGraphTest::testRenderGraph()
{
  OmgGraph myGraph(mpPainter);
  myGraph.clear();
  myGraph.addSeries(mSeries1);
  myGraph.addSeries(mSeries2);
  myGraph.addSeries(mSeries3);
  //qDebug("Series count:");
  //qDebug(QString::number(myGraph.seriesCount()));
  myGraph.render();
  QVERIFY(mImage.save("/tmp/graphtest1.png")!=false);
  myGraph.clear();
  //turn on all the bells & whistles - except splining
  myGraph.setVerticesEnabled(true);
  myGraph.setVertexLabelsEnabled(true);
  myGraph.setAreaFillEnabled(true);
  myGraph.setSpliningEnabled(false);
  myGraph.setGridLinesEnabled(true);
  myGraph.render();
  QVERIFY(mImage.save("/tmp/graphtest2.png")!=false);
  myGraph.clear();
  myGraph.setVerticesEnabled(true);
  myGraph.setVertexLabelsEnabled(true);
  myGraph.setAreaFillEnabled(true);
  myGraph.setSpliningEnabled(true);
  myGraph.setGridLinesEnabled(false);
  myGraph.removeSeriesAt(0);
  myGraph.removeSeriesAt(0);
  myGraph.render();
  QVERIFY(mImage.save("/tmp/graphtest3.png")!=false);
}

QTEST_MAIN(OmgGraphTest) 
#include "moc_omggraphtest.cxx"
