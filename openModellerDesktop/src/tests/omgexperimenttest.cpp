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
#include <omgui.h>
#include <QString>
#include <QDir>
class OmgExperimentTest: public QObject
{
  Q_OBJECT;
      private slots:
      void createExperiment();
      void loadAndGraph();
      void run();
      void isAborted();
      void saveAndLoadExperiment();
      void printMessage(QString); //used for connecting to logMessage
};

void OmgExperimentTest::isAborted()
{
  OmgExperiment myExperiment;
  QVERIFY(myExperiment.isAborted()==false);
  myExperiment.abort();
  QVERIFY(myExperiment.isAborted()==true);
}
void OmgExperimentTest::createExperiment()
{
  OmgExperiment myExperiment;
}
void OmgExperimentTest::saveAndLoadExperiment()
{
  OmgExperiment myExperiment;
  myExperiment.setName("Test 1234");
  QString myFileName;
  //@TODO make this OS agnostic...
  myFileName = "/tmp/" + myExperiment.guid() + ".xml";
  Omgui::createTextFile( myFileName , myExperiment.toXml());
  OmgExperiment myExperiment2;
  myExperiment2.fromXmlFile(myFileName);
  QVERIFY(myExperiment2.name()=="Test 1234");
}
void OmgExperimentTest::loadAndGraph()
{
  QString myFileName (TEST_DATA_DIR); //defined in top CmakeLists.txt
  myFileName += "/experiment.xml";
  OmgExperiment myExperiment;
  myExperiment.fromXmlFile(myFileName);
  QVERIFY(myExperiment.name()=="Test");
  myExperiment.makeAlgorithmSummaryGraphs("/tmp");
}
void OmgExperimentTest::run()
{
  //TEST_DATA_DIR is a compiler define set by cmake
  QString myFileName(QString(TEST_DATA_DIR) + "/experiment.xml");
  //we need to replace [PREFIX] in teh file paths in the 
  //model we just loaded so our test can actually find the model 
  //data. To do that I temporarily get the xml for the model
  //search and replace the xml and then reinstate the model
  //from the updated xml...
  OmgExperiment myExperiment;
  myExperiment.fromXmlFile(myFileName);
  QString myExperimentXml = myExperiment.toXml();
  myExperimentXml.replace("[PREFIX]",TEST_DATA_DIR);
  OmgExperiment myExperiment2;
  myExperiment2.fromXml(myExperimentXml);
  QVERIFY(myExperiment2.name()=="Test");
  connect(&myExperiment2, SIGNAL(logMessage(QString)),
      this, SLOT(printMessage(QString)));
  QStringList myList = OmgModellerPluginRegistry::instance()->names();
  qDebug(
    myList.join(" -- ")
      );
  QVERIFY(myList.count() > 0);
  OmgModellerPluginInterface * mypModellerPlugin = 
    OmgModellerPluginRegistry::instance()->getPlugin();
  QVERIFY(!mypModellerPlugin->getName().isEmpty());
  myExperiment2.setModellerPlugin(mypModellerPlugin);
  myExperiment2.reset();
  myExperiment2.setWorkDir("/tmp");
  myExperiment2.run();
  myExperiment2.makeAlgorithmSummaryGraphs("/tmp");

}
void OmgExperimentTest::printMessage(QString theMessage)
{
  qDebug(theMessage);
}

QTEST_MAIN(OmgExperimentTest) 
#include "moc_omgexperimenttest.cxx"
  
