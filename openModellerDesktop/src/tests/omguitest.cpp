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
#include <omgui.h>
#include <omgexperiment.h>
#include <QImage>
#include <QList>

class OmguiTest: public QObject
{
  Q_OBJECT;
  private slots:
  void regression1728367(); //regression test for bug 1728367
  void regression1711409(); //regression test for not finding ai grid layers
  void initTestCase();// will be called before the first testfunction is executed.
  void cleanupTestCase();// will be called after the last testfunction was executed.
  void init(){};// will be called before each testfunction is executed.
  void cleanup(){};// will be called after every testfunction.
  private:
  QString mOriginalWorkDir;
};

void OmguiTest::initTestCase()
{
  QCoreApplication::setOrganizationName("openModeller");
  QCoreApplication::setOrganizationDomain("openmodeller.sf.net");
  QCoreApplication::setApplicationName("OpenModellerGui");
  QSettings mySettings;
  mOriginalWorkDir = mySettings.value("dataDirs/dataDir","/tmp").toString();
  mySettings.setValue("dataDirs/dataDir","/tmp");
  //qDebug("Data dir setting stored and set to /tmp");
}
void OmguiTest::cleanupTestCase()
{
  QSettings mySettings;
  mySettings.setValue("dataDirs/dataDir",mOriginalWorkDir);
  //qDebug("Data dir setting restored to \n" + mySettings.value("dataDirs/dataDir").toString().toLocal8Bit());
}
void OmguiTest::regression1728367()
{
  QString myFileName;
  //@TODO make sure this OS agnostic...
  QDir myDir;
  if (!myDir.exists("/tmp"))
  {
    QVERIFY(myDir.mkdir("/tmp"));
  }
  if (!myDir.exists("/tmp/modelOutputs"))
  {
    QVERIFY(myDir.mkdir("/tmp/modelOutputs"));
  }
  if (!myDir.exists("/tmp/modelOutputs/Test 1"))
  {
    QVERIFY(myDir.mkdir("/tmp/modelOutputs/Test 1"));
  }
  //create a temporary little experiment
  OmgExperiment myExperiment;
  myExperiment.setName("Test 1");
  myFileName = "/tmp/modelOutputs/Test 1/" + myExperiment.guid() + ".xml";
  Omgui::createTextFile( myFileName , myExperiment.toXml());
  QFileInfo myInfo(myFileName);
  QVERIFY(myInfo.exists());
  //now see if omgui can find this experiment...
  QStringList myList = Omgui::getExperimentsList();
  QVERIFY(myList.count() > 0);
  /*
  QStringListIterator myIterator(myList);
  while (myIterator.hasNext())
  {
    QString myFileName = myIterator.next();
    qDebug("Found");
    qDebug(myFileName);
  }
  */
}
void OmguiTest::regression1711409()
{
  //there should be a test aig file in there
  QString myDirName (TEST_DATA_DIR); //defined in top CmakeLists.txt
  QDir myDir;
  if (!myDir.exists("/tmp"))
  {
    QVERIFY(myDir.mkdir("/tmp"));
  }
  QString myLayers = Omgui::getLayers(myDirName);
  Omgui::createTextFile("/tmp/layerSelectorCache.xml", myLayers);
  //check at least one AIG layer was found
  QVERIFY(myLayers.contains("Type=\"AIG\""));
  
}

QTEST_MAIN(OmguiTest) 
#include "moc_omguitest.cxx"
  


