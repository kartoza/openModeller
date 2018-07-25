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
#include <omggradientfactory.h>
#include <QImage>
#include <QPainter>

class OmgGradientFactoryTest: public QObject
{
  Q_OBJECT;
  private slots:
  void testRedGradient();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup();// will be called after every testfunction.
  private:
  OmgGradientFactory mFactory;
  QImage mImage;
  QPainter * mpPainter;
  QString mTestFile;
  QLinearGradient mGradient;
};

void OmgGradientFactoryTest::initTestCase()
{
  mImage = QImage( 800,800,QImage::Format_ARGB32 );
  mImage.fill(Qt::white);
  mpPainter = new QPainter ( &mImage );
}
void OmgGradientFactoryTest::cleanup()
{
  //paint out to the qimage here then save to disk
}
void OmgGradientFactoryTest::cleanupTestCase()
{
  delete mpPainter;
}
void OmgGradientFactoryTest::testRedGradient()
{
  mTestFile="/tmp/redGradient.png";
  //mGradient = mFactory.getRedGradient();
}

QTEST_MAIN(OmgGradientFactoryTest) 
#include "moc_omggradientfactorytest.cxx"

