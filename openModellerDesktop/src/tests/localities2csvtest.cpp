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
#include <QFile>
#include <QStringList>

#include <omgui.h>
#include <omgmodel.h>
#include <omglocality.h>
#include <omglayer.h>
#include <omglayerset.h>

class Localities2CsvTest: public QObject
{
  Q_OBJECT;
      private slots:
      void writeLocalities2Csv();
};

void Localities2CsvTest::writeLocalities2Csv()
{
  OmgModel myModel;
  //QStringList myList;
  //myList << "Layer1" << "Layer2";
  //myModel.setCreationLayerNames(myList); 
  OmgLayer myLayer1;
  myLayer1.setName("Layer1");
  OmgLayer myLayer2;
  myLayer2.setName("Layer2");
  OmgLayerSet myLayerSet;
  myLayerSet.addLayer(myLayer1);
  myLayerSet.addLayer(myLayer2);
  myModel.setCreationLayerSet(myLayerSet);
  OmgLocalities myLocalities;
  OmgLocality myLocality;
  myLocality.setLabel("Test species");
  myLocality.setLongitude(10.00);
  myLocality.setLatitude(20.00);
  myLocalities.push_back(myLocality);
  myModel.setLocalities(myLocalities);
  QString myString = myModel.localitiesToCsv();
  Omgui::createTextFile("/tmp/test.csv", myString );
  QVERIFY(!myString.isEmpty());
  qDebug("Csv file:");
  qDebug(myString.toLocal8Bit());
}
QTEST_MAIN(Localities2CsvTest) 
#include "moc_localities2csvtest.cxx"
  
