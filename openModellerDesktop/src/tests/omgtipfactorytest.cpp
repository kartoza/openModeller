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
#include <omgtip.h>
#include <omgtipfactory.h>

class OmgTipFactoryTest: public QObject
{
  Q_OBJECT;
  private slots:
  void getGuiTip();
  void getGenericTip();
  void getTip();
  void getTipAtPosition();
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup(){};// will be called after every testfunction.
  private:
  OmgTipFactory mFactory;
};

void OmgTipFactoryTest::initTestCase()
{
  QCoreApplication::setOrganizationName("openModeller");
  QCoreApplication::setOrganizationDomain("openmodeller.sf.net");
  QCoreApplication::setApplicationName("OpenModellerGui");
}
void OmgTipFactoryTest::cleanupTestCase()
{
}
void OmgTipFactoryTest::getGuiTip()
{
  QVERIFY(!mFactory.getGuiTip().title().isEmpty());
  QVERIFY(!mFactory.getGuiTip().content().isEmpty());

}
void OmgTipFactoryTest::getGenericTip()
{
  QVERIFY(!mFactory.getGenericTip().title().isEmpty());
  QVERIFY(!mFactory.getGenericTip().content().isEmpty());
}
void OmgTipFactoryTest::getTip()
{
  QVERIFY(!mFactory.getTip().title().isEmpty());
  QVERIFY(!mFactory.getTip().content().isEmpty());
}
void OmgTipFactoryTest::getTipAtPosition()
{
  QVERIFY(!mFactory.getTip(0).title().isEmpty());
  QVERIFY(!mFactory.getTip(0).content().isEmpty());
}

QTEST_MAIN(OmgTipFactoryTest) 
#include "moc_omgtipfactorytest.cxx"



