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
#include <QtTest/QtTest>
#include <QDomDocument>
#include <QFile>
#include <omgmodellerlocalplugin.h>

class OmgModellerLocalPluginTest: public QObject
{
  Q_OBJECT;
      private slots:
      void getAlgorithms();
      void getLayers();
};

void OmgModellerLocalPluginTest::getAlgorithms()
{
  OmgModellerLocalPlugin mypPlugin;
  QStringList myList = mypPlugin.getAlgorithmList();
  qDebug("Algorithms Found:");
  qDebug(myList.join("\n").toLocal8Bit());
  QVERIFY(myList.size()>0);
}
void OmgModellerLocalPluginTest::getLayers()
{
  OmgModellerLocalPlugin mypPlugin;
  QString myString = mypPlugin.getLayers("/home/timlinux/gisdata/Africa/");
  //NOTE debuggin the doc to std out seems to truncate it
  //this to really see if it worked we write it to a file as shown below
  //qDebug(myString.toLocal8Bit());
  //try to load the returned doc back to xml again...
  QDomDocument myDocument("TestDoc");
  QVERIFY(myDocument.setContent(myString));
  //write the doc fo a file
  QFile file("/tmp/layerset.xml");
  if (!file.open(QIODevice::Append))
  {
    QFAIL("Could not write temporary layerset xml to disk");
  }
  file.write(myString.toAscii());
  file.close();
}

QTEST_MAIN(OmgModellerLocalPluginTest) 
#include "moc_omgmodellerlocalplugintest.cxx"
