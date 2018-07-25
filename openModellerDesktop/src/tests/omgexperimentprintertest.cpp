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
#include <QPrinter>
#include <omgexperiment.h>
#include <omgexperimentprinter.h>

class OmgExerimentPrinterTest: public QObject
{
  Q_OBJECT;
      private slots:
      void run();
};

void OmgExerimentPrinterTest::run()
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
  myExperiment2.setWorkDir("/tmp");
  //
  // Note omgexperimentprinter can run in its
  // own thread (using start()) but in this test
  // I am only validating in a non threaded
  // way
  //
  QPrinter myPrinter;
  myPrinter.setOutputFormat(QPrinter::PdfFormat);
  myPrinter.setOutputFileName("/tmp/experimentprintertest.pdf");
  myPrinter.setDocName("openModellerDesktopExperimentReport");
  OmgExperimentPrinter myExperimentPrinter ;
  myExperimentPrinter.setExperiment(&myExperiment2);
  myExperimentPrinter.setPrinter(myPrinter);
  //mpExperimentPrinter.start(); //see note above
  myExperimentPrinter.run();
}

QTEST_MAIN(OmgExerimentPrinterTest) 
#include "moc_omgexperimentprintertest.cxx"
  
