 /***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   tim@linfiniti.com   *
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY{} without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program{} if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <omgexperimentprinter.h>
#include <omgexperiment.h>
#include <omgmodel.h>
#include <omgui.h>
#include <QDebug>
#include <QCoreApplication>
#include <QPrinter>
#include <QTextDocument>
#include <QVariant>
#include <QUrl>
#include <QMap>
OmgExperimentPrinter::OmgExperimentPrinter() : QThread()
{

}
OmgExperimentPrinter::~OmgExperimentPrinter()
{

}

void OmgExperimentPrinter::run()
{
  mMutex.lock();
  //create the text document with the report content
  //@TODO see if we cant stream directly out to the
  //print device
  QTextDocument myTextDocument;
  myTextDocument.setDefaultStyleSheet(Omgui::defaultStyleSheet());
  QString myReport;
  myReport += Omgui::getHtmlHeader();
  myReport += mpExperiment->toPrintHtml();
  
  // 
  // First get a list of all layersets, taxa and algs used
  // 
  QMap<QString,OmgAlgorithm> myAlgorithmMap;
  QMap<QString,OmgLayerSet> myLayerSetMap;
  QStringList myTaxonNameList;
  int myCount = mpExperiment->count();
  emit maximum(myCount);
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      continue;
    }
    
    //
    // Projection Layersets
    //
    
    //QString myLayerSetName = mypModel->projectionLayerSet().name();
    //if (!myLayerSetMap.contains(myLayerSetName))
    //{
    //  myLayerSetMap.insert(myLayerSetName, mypModel->projectionLayerSet());
    //}

    //
    // Algorithms
    //
    
    QString myAlgorithmName = mypModel->algorithm().name();
    if (!myAlgorithmMap.contains(myAlgorithmName))
    {
      myAlgorithmMap.insert(myAlgorithmName, mypModel->algorithm());
    }
    
    //
    // Taxa
    //

    QString myTaxonName = mypModel->taxonName();
    if (!myTaxonNameList.contains(myTaxonName))
    {
      myTaxonNameList << myTaxonName;
    }
  }
  qSort(myTaxonNameList.begin(), myTaxonNameList.end());
  //
  // Print thumbnail summary view by taxon
  //
  myReport += "<center><h2 style=\"page-break-before:always;\">" + tr("Overview of Model Projections by Taxon") + "</h2></center>";
  bool myPageBreakNeededFlag = false;
  QStringListIterator myTaxonNameIterator(myTaxonNameList);
  while (myTaxonNameIterator.hasNext())
  {
    QString myTaxonName = myTaxonNameIterator.next();
    //qDebug() << "Generating thumbnail list for: " << myTaxonName;
    if (myPageBreakNeededFlag)
    {
      myReport += "<center><h2 class=\"glossyBlue\" style=\"page-break-before:always;\">" + myTaxonName + "</h2></center>";
    }
    else
    {
      //first page break  is not needed
      myReport += "<center><h2 class=\"glossyBlue\">" + myTaxonName + "</h2></center>";
      myPageBreakNeededFlag = true;
    }
    //myReport += mpExperiment->toTaxonSummaryHtml(myTaxonName);
  }
  //
  // Print thumbnail summary view by algorithm
  //
  /*
  QStringList myAlgorithmNameList = myAlgorithmMap.keys();
  qSort(myAlgorithmNameList.begin(), myAlgorithmNameList.end());
  QStringListIterator myIterator(myNameList);
  while (myIterator.hasNext())
  {
    QString myAlgorithmName = myIterator.next();
    myReport += mpExperiment->toAlgorithmSummaryHtml(myAlgorithmName);
  }
  */
  
  //
  // Print each layerst profile used in the experiment
  //
  //qSort(myLayerSetMap.begin(), myLayerSetMap.end());
  myReport += "<center><h2 style=\"page-break-before:always;\">" + tr("Summary of LayerSet Profiles") + "</h2>";
  myPageBreakNeededFlag = false;
  QMapIterator<QString,OmgLayerSet> myLayerSetIterator(myLayerSetMap);
  while (myLayerSetIterator.hasNext())
  {
    myLayerSetIterator.next();
    OmgLayerSet myLayerSet = myLayerSetIterator.value();
    if (myPageBreakNeededFlag)
    {
      myReport += "<h2 style=\"page-break-before:always;\"> </h2>";
    }
    else
    {
      //first page break  is not needed
      myPageBreakNeededFlag = true;
    }
    myReport += myLayerSet.toHtml();
  }
  
  //
  // Print each algorithm profile used in the experiment
  //
  //qSort(myAlgorithmMap.begin(), myAlgorithmMap.end());
  myReport += "<center><h2 style=\"page-break-before:always;\">" + tr("Summary of Algorithm Profiles") + "</h2></center>";
  myPageBreakNeededFlag = false;
  QMapIterator<QString,OmgAlgorithm> myAlgorithmtIterator(myAlgorithmMap);
  while (myAlgorithmtIterator.hasNext())
  {
    myAlgorithmtIterator.next();
    OmgAlgorithm myAlgorithm = myAlgorithmtIterator.value();
    if (myPageBreakNeededFlag)
    {
      myReport += "<h2 style=\"page-break-before:always;\"> </h2>";
    }
    else
    {
      //first page break  is not needed
      myPageBreakNeededFlag = true;
    }
    myReport += myAlgorithm.toHtml();
  }
  //
  // Print the detailed report for each model
  //
  myReport += "<center><h2 style=\"page-break-before:always;\">" + 
    tr("Detailed Model Reports") + "</h2></center>";
  myPageBreakNeededFlag = false;
  for (int i=0; i<myCount; i++)
  {
    OmgModel *  mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      //emit logMessage("Model is null is NULL, skipping");
      continue;
    }
    if (myPageBreakNeededFlag)
    {
      myReport += "<h2 style=\"page-break-before:always;\"> </h2>";
    }
    else
    {
      //first page break  is not needed
      myPageBreakNeededFlag = true;
    }
    myReport += mypModel->toPrintHtml();  
    emit progress(i);
    QCoreApplication::processEvents();
  }
  myReport += Omgui::getHtmlFooter();
  myTextDocument.setHtml(myReport);
  myTextDocument.print(&mPrinter);
  emit progress(myCount);
  emit done();
  mMutex.unlock();
}
bool OmgExperimentPrinter::isRunning()
{
 return mRunningFlag; 
}
/** Stop running anymore models */
void OmgExperimentPrinter::stop()
{

}

void OmgExperimentPrinter::setPrinter(QPrinter &thePrinter)
{
  mPrinter.setOutputFormat(thePrinter.outputFormat()) ;
  mPrinter.setOutputFileName(thePrinter.outputFileName()) ;
  mPrinter.setDocName("openModellerDesktopExperimentReport");
}
void OmgExperimentPrinter::setExperiment(OmgExperiment * thepExperiment)
{
  mpExperiment = thepExperiment;
}
