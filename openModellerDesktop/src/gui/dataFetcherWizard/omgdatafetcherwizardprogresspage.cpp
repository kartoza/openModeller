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

#include <omgdatafetcherwizardprogresspage.h>
#include <omgdatafetcherwizardprogresswidget.h>
#include <omgpluginmessenger.h>
#include <omgscraperpluginregistry.h>
#include <omgui.h> //ancilliary helper functions
//qt includes
#include <QSettings>

//
// QT includes
//
#include <QWizard>
#include <QVBoxLayout>
#include <QFile>

  OmgDataFetcherWizardProgressPage::OmgDataFetcherWizardProgressPage(QWidget *parent)
: QWizardPage(parent),
  mpProgressWidget(new OmgDataFetcherWizardProgressWidget())
{
  setTitle(tr("Search Progress"));
  QVBoxLayout *mypLayout = new QVBoxLayout;
  mypLayout->addWidget(mpProgressWidget);
  setLayout(mypLayout);
}
void OmgDataFetcherWizardProgressPage::setTaxonListWidget (QListWidget * thepWidget)
{
  mpTaxonListWidget = thepWidget;
}
void OmgDataFetcherWizardProgressPage::search()
{
  qDebug("Search called");
  mpProgressWidget->getTeResults()->clear();
  QStringList myTaxonList;
  for (int myListPosition=0; myListPosition < mpTaxonListWidget->count(); myListPosition++)
  {
    QListWidgetItem *item = mpTaxonListWidget->item( myListPosition );
    myTaxonList << item->text();
  }
  if (myTaxonList.size()==0)
  {
    setStatus(QString(tr("Search list is empty!")));
    return;
  }

  QDir myOutputDir(field("leOutputPath").toString());
  if (!myOutputDir.isReadable())
  {
    setStatus(QString(tr("Output folder is invalid")));
    return;
  }



  mRunningCount = 0;
  mNamesWithDataCount = 0;
  int myListSize = myTaxonList.size();


  // set total progress bar
  mpProgressWidget->getProgressBar()->setValue(0);
  mpProgressWidget->getProgressBar()->setMaximum(myListSize);

  //
  //// Load scraper plugin
  //

  QSettings mySettings;
  
  QString myPluginName = 
    mySettings.value("locScraper/scraperPluginName", "GBIF REST Web Service Plugin").toString();
  OmgScraperPluginInterface * mypScraperPlugin = 
    OmgScraperPluginRegistry::instance()->getPlugin(myPluginName);
  if (!mypScraperPlugin)
  {
    setStatus(tr("Plugin matching this name could not be found!"));
    return;
  }
  if (!mypScraperPlugin)
  {
    setStatus(tr("Plugin loading failed!"));
    return;
  }
  qDebug (mypScraperPlugin->getName().toLocal8Bit());
  const OmgPluginMessenger * mypMessenger = mypScraperPlugin->getMessenger();
  connect(mypMessenger, SIGNAL(fileWritten(QString, QString, QString,int)),
      this, SLOT(fileWritten(QString, QString,QString, int)));
  connect(mypMessenger, SIGNAL(fileNotWritten(QString)),
      this, SLOT(fileNotWritten(QString)));
  connect(mypMessenger, SIGNAL(error(QString)),
      this, SLOT(setError(QString)));
  connect(mypMessenger, SIGNAL(message(QString)),
      this, SLOT(setStatus(QString)));

  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  qDebug(QString::number(myListSize).toLocal8Bit() + " items to be searched for...");
  int myMinimumRecords = mySettings.value("locScraper/minimumRecords",1).toInt();
  mypScraperPlugin->setMinimumRecords(myMinimumRecords);
  mpProgressWidget->getProgressBar()->reset();
  mpProgressWidget->getProgressBar()->show();
  mpProgressWidget->getProgressBar()->setValue(0);
  for (int myListPosition=0; myListPosition < myListSize; ++myListPosition)
  {
    QString mySearchString = myTaxonList.at(myListPosition); 
    QString myFileName = field("leOutputPath").toString()
      + QDir::separator() +mySearchString + ".shp";
    qDebug(tr("Creating : ") + myFileName.toLocal8Bit());
    qDebug(tr("Calling search on scraper"));
    mypScraperPlugin->search(mySearchString, myFileName);
    qDebug(tr("Search completed on scraper"));
    mpProgressWidget->getProgressBar()->setValue(myListPosition+1);
    emit shapefileCreated( myFileName );
  }
  QApplication::restoreOverrideCursor();
  mpProgressWidget->getProgressBar()->reset();
  mpProgressWidget->getProgressBar()->hide();
  //show some summary info at the end of the search
  mpProgressWidget->getTeResults()->append("==================");
  mpProgressWidget->getTeResults()->append(tr("Summary:"));
  mpProgressWidget->getTeResults()->append(tr("Names searched   : ") + QString::number(myListSize));
  mpProgressWidget->getTeResults()->append(tr("Number of taxa with more than ") + 
      QString::number(myMinimumRecords) +
      tr(" record(s) : ") + QString::number(mNamesWithDataCount));
  mpProgressWidget->getTeResults()->append(tr("Total usable localities : ") + QString::number(mRunningCount));
  mpProgressWidget->getTeResults()->append("==================");
  disconnect(mypMessenger, SIGNAL(fileWritten(QString, QString, QString,int)),
      this, SLOT(fileWritten(QString, QString,QString, int)));
  disconnect(mypMessenger, SIGNAL(fileNotWritten(QString)),
      this, SLOT(fileNotWritten(QString)));
  disconnect(mypMessenger, SIGNAL(statusChanged(QString)),
      this, SLOT(setStatus(QString)));
  disconnect(mypMessenger, SIGNAL(error(QString)),
      this, SLOT(setError(QString)));
  setStatus("Search complete!");

}


void OmgDataFetcherWizardProgressPage::fileWritten(QString theShapeFile, QString theTextFile,QString theTaxonName,int theCount)
{
  setStatus(theShapeFile + tr(" written"));
  //show the retrieved points in the gui - disabled for now as its too verbose
  //showPoints(theTextFile); 
  setStatus(theTextFile + tr(" written"));
  mpProgressWidget->getTeResults()->append(QString(theTaxonName + " (" + QString::number(theCount) + ")"));
  QSettings mySettings;
  int myMinimumRecords = mySettings.value("locScraper/minimumRecords",1).toInt();
  if (theCount >= myMinimumRecords)
  {
    ++mNamesWithDataCount;
    mRunningCount += theCount;
  }
}

void OmgDataFetcherWizardProgressPage::fileNotWritten(QString theTaxonName)
{
  setStatus(tr("Not enough data, file not written"));
  QString myMessage(theTaxonName + " - not enough data");
  mpProgressWidget->getTeResults()->textCursor().movePosition(QTextCursor::End);
  mpProgressWidget->getTeResults()->insertHtml("<br/><font color=\"red\">" + myMessage + "</font><font color=\"black\"> * </font><br/>");
  mpProgressWidget->getTeResults()->ensureCursorVisible();
}


void OmgDataFetcherWizardProgressPage::showPoints(QString theFileName)
{
  QFile myFile( theFileName );
  if ( myFile.open( QIODevice::ReadOnly ) )
  {
    mpProgressWidget->getTeResults()->append(myFile.readAll());
    myFile.close();
  }
  else
  {
    setError(tr("Could not open the generated Points file") + " (" + theFileName.toLocal8Bit() + ")");
  }
  return ;
}

void OmgDataFetcherWizardProgressPage::setStatus(QString theStatus)
{
  mpProgressWidget->getLblStatus()->setText( theStatus );
  //for debugging only
  //mpProgressWidget->getTeResults()->append( theStatus );
  //mpProgressWidget->getTeResults()->ensureCursorVisible ();
}

void OmgDataFetcherWizardProgressPage::setError(QString theError)
{
  mpProgressWidget->getTeResults()->append( theError );
  //QMessageBox::warning(this, "Localities Search", theError);
}

