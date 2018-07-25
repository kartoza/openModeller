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
#include <QCoreApplication>
#include <omgexperiment.h>
#include <omgmodellerpluginregistry.h>
#include <omgmodellerplugininterface.h>
#include <omgpluginmessenger.h>
#include <omgui.h>
#include <omglocalitiesmodel.h>
#include <QString>
#include <QDir>
class OmgLocalitiesModelTest: public QObject
{
  Q_OBJECT;
      private slots:
      void rowsAndColumns();
};

void OmgLocalitiesModelTest::rowsAndColumns()
{
  //TEST_DATA_DIR is a compiler define set by cmake
  QString myFileName(QString(TEST_DATA_DIR) + "/model.xml");
  qDebug(myFileName);
  OmgModel myModel;
  myModel.fromXmlFile(myFileName);
  //we need to replace [PREFIX] in teh file paths in the 
  //model we just loaded so our test can actually find the model 
  //data. To do that I temporarily get the xml for the model
  //search and replace the xml and then reinstate the model
  //from the updated xml...
  QString myModelXml = myModel.toXml();
  myModelXml.replace("[PREFIX]",TEST_DATA_DIR);
  myModel.fromXml(myModelXml);
  QString myFileName2 = "/tmp/replacedmodel.xml";
  Omgui::createTextFile( myFileName2 , myModel.toModelCreationXml());


  QStringList myList = OmgModellerPluginRegistry::instance()->names();
  QVERIFY(myList.count() > 0);
  myModel.setWorkDir("/tmp");

  OmgModellerPluginInterface * mypModellerPlugin = 
    OmgModellerPluginRegistry::instance()->getPlugin();
  QVERIFY(!mypModellerPlugin->getName().isEmpty());
  QString myResult = mypModellerPlugin->createModel(&myModel);
  QVERIFY(!myResult.isEmpty());
  OmgLocalitiesModel myLocalitiesModel;
  myLocalitiesModel.setModel(&myModel);
  QVERIFY(myLocalitiesModel.rowCount() > 0);
  QVERIFY(myLocalitiesModel.columnCount() > 0);
}

QTEST_MAIN(OmgLocalitiesModelTest) 
#include "moc_omglocalitiesmodeltest.cxx"
