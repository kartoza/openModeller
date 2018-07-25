/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
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
#include <QString>
#include <QDir>
class OmgModelUnitTest: public QObject
{
  Q_OBJECT;
      private slots:
      void run();
      void saveAndLoadModel();
      void testLoadLocalities();
      public slots:
      void showMessage(QString,QString);
};

void OmgModelUnitTest::showMessage(QString theModelId,QString theMessage)
{
  qDebug(theMessage);
}

void OmgModelUnitTest::saveAndLoadModel()
{
  OmgModel myModel;
  QString myFileName = "/tmp/" + myModel.guid() + ".xml";
  qDebug("Writing Model 1 to: " + myFileName.toLocal8Bit());
  Omgui::createTextFile( myFileName , myModel.toXml());
  OmgModel myModel2;
  QVERIFY(myModel2.fromXmlFile(myFileName));
  qDebug("Model 1 GUID: " + myModel.guid().toLocal8Bit());
  qDebug("Model 2 GUID: " + myModel2.guid().toLocal8Bit());
  QVERIFY(myModel2.guid()==myModel.guid());
}
void OmgModelUnitTest::run()
{
  qDebug("Current dir:");
  qDebug(QDir::current().absolutePath().toLocal8Bit());
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
  
  qDebug("\nCreation Layers:");
  qDebug("\n" + myModel.creationLayerNames().join("\n").toLocal8Bit());
  
  qDebug("\nProjection Layers:");
  QList<OmgProjection *> myProjectionList = myModel.projectionList();
  QListIterator<OmgProjection *> myIterator(myProjectionList);
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();

    qDebug("\nProjection: " + mypProjection->layerSet().name().toLocal8Bit());

    qDebug("\n" + mypProjection->projectionLayerNames().join("\n").toLocal8Bit());
  }
  
  QString myPluginsPath=Omgui::pluginDirPath() + QDir::separator() + "modeller";
  qDebug("Searching for modeller plugins at:");
  qDebug(myPluginsPath.toLocal8Bit());
  QStringList myList = OmgModellerPluginRegistry::instance()->names();
  qDebug("\nAvailable modeller plugins:");
  qDebug( "\n" + myList.join("\n").toLocal8Bit() );
  QVERIFY(myList.count() > 0);
  
  myModel.setWorkDir("/tmp");
  
  OmgModellerPluginInterface * mypModellerPlugin = 
  OmgModellerPluginRegistry::instance()->getPlugin();
  QVERIFY(!mypModellerPlugin->getName().isEmpty());
  const OmgPluginMessenger * mypMessenger = mypModellerPlugin->getMessenger();
  connect(mypMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(showMessage(QString,QString)));
  connect(mypMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(showMessage(QString,QString)));
  QString myResult = mypModellerPlugin->createModel(&myModel);
  //qDebug(myResult);
  QVERIFY(!myResult.isEmpty());
  myModel.parseModelResult(myResult);

  myIterator.toFront();
  while (myIterator.hasNext())
  {
    OmgProjection * mypProjection = myIterator.next();

    mypModellerPlugin->projectModel(mypProjection);
  }
}

void OmgModelUnitTest::testLoadLocalities()
{
  //TEST_DATA_DIR is a compiler define set by cmake
  QString myFileName(QString(TEST_DATA_DIR) + "/Trifolium_repens.txt");
  OmgLocalities myLocalities;
  unsigned int myCount = loadLocalities(myLocalities, myFileName, "Trifolium repens");
  qDebug(QString::number(myCount).toLocal8Bit() + " records loaded");
  QVERIFY(myCount==24527);
  QVERIFY(myLocalities.size()==24527);
}

QTEST_MAIN(OmgModelUnitTest) 
#include "moc_omgmodeltest.cxx"
  

