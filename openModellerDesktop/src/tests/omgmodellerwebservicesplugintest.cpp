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
#include <omgmodellerwebservicesplugin.h>
#include <soapH.h>

class OmgModellerWebServicesPluginTest: public QObject
{
  Q_OBJECT;
  private slots:
    void getAlgorithms();
    void getLayers();
    void getName();
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    //void init();// will be called before each testfunction is executed.
    //void cleanup();// will be called after every testfunction.
  public slots:
    void showMessage(QString,QString);
    void showMessage(QString);
  private:
    OmgModellerWebServicesPlugin * mpPlugin;
    const OmgPluginMessenger * mpMessenger;
};

void OmgModellerWebServicesPluginTest::initTestCase()
{
  mpPlugin = new OmgModellerWebServicesPlugin(); 
  mpMessenger = mpPlugin->getMessenger();
  connect(mpMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(showMessage(QString,QString)));
  connect(mpMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(showMessage(QString,QString)));
  connect(mpMessenger, SIGNAL(error(QString )),
      this, SLOT(showMessage(QString)));
  QCoreApplication::setOrganizationName("openModeller");
  QCoreApplication::setOrganizationDomain("openmodeller.sf.net");
  QCoreApplication::setApplicationName("OpenModellerGui");
}

void OmgModellerWebServicesPluginTest::cleanupTestCase()
{
  delete mpPlugin;
}

void OmgModellerWebServicesPluginTest::showMessage(QString theModelId,QString theMessage)
{
  qDebug(theMessage);
}

void OmgModellerWebServicesPluginTest::showMessage(QString theMessage)
{
  qDebug(theMessage);
}

void OmgModellerWebServicesPluginTest::getLayers()
{
  const QString myLayers = mpPlugin->getLayers("");
  //@TODO parse and really verify the xml properly
  QVERIFY(!myLayers.isEmpty());
}
void OmgModellerWebServicesPluginTest::getName()
{
  const  QString myName = mpPlugin->getName();
  QVERIFY(myName == "Web Services Modeller Plugin");
}

void OmgModellerWebServicesPluginTest::getAlgorithms()
{
  QStringList myList = mpPlugin->getAlgorithmList();
  //qDebug(myList.join(",").toLocal8Bit());
  QVERIFY(myList.size()>0);
}

QTEST_MAIN(OmgModellerWebServicesPluginTest) 
#include "moc_omgmodellerwebservicesplugintest.cxx"
