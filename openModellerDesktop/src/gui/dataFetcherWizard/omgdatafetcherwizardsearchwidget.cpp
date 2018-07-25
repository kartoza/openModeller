/***************************************************************************
 *   Copyright (C) 2007 by Tim Sutton tim@linfiniti.com   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02222-2307, USA.             *
 ***************************************************************************/

#include <omgui.h> //ancilliary helper functions
#include <omgdatafetcherwizardsearchwidget.h>
#include <omgscraperpluginregistry.h>
#include <QSettings>
#include <QStringList>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
OmgDataFetcherWizardSearchWidget::OmgDataFetcherWizardSearchWidget(QWidget* parent , Qt::WFlags fl )
{
  setupUi(this);
  QSettings mySettings;
  //set the gui defaults
  QString myPluginName = mySettings.value("locScraper/scraperPluginName", "GBIF REST Web Service Plugin").toString();
  QStringList myList = OmgScraperPluginRegistry::instance()->names();
  cboScraperPluginName->addItems(myList);
  if (myList.contains(myPluginName))
  {
    cboScraperPluginName->setCurrentIndex(cboScraperPluginName->findText(myPluginName));
  }

  leSearchString->setText(mySettings.value("locScraper/searchString", "").toString());
  leOutputPath->setText(mySettings.value("locScraper/outputPath", "").toString());
  spinMinimumRecords->setValue(mySettings.value("locScraper/minimumRecords", 20).toInt());
  connect(leOutputPath, SIGNAL(editingFinished()), 
      this, SLOT(outputPathChanged()));
}
OmgDataFetcherWizardSearchWidget::~OmgDataFetcherWizardSearchWidget()
{
  QSettings mySettings;
  mySettings.setValue("locScraper/searchString",leSearchString->text());
  mySettings.setValue("locScraper/outputPath",leOutputPath->text());
  mySettings.setValue("locScraper/scraperPluginName",cboScraperPluginName->currentText() );
  mySettings.setValue("locScraper/minimumRecords",spinMinimumRecords->value());
}
QListWidget * OmgDataFetcherWizardSearchWidget::getLstSearch()
{
  return lstSearch;
}
QLineEdit * OmgDataFetcherWizardSearchWidget::getLeOutputPath()
{
  return leOutputPath;
}
void OmgDataFetcherWizardSearchWidget::on_toolSelectSearchFileName_clicked()
{
  QSettings mySettings;
  QString myFileNameQString = QFileDialog::getOpenFileName(
      this,
      tr("Choose a file to load"),
      mySettings.value("locScraper/searchFilePath", ".").toString(),
      "Text files (*.txt)");
  setSearchList(myFileNameQString);
  mySettings.setValue("locScraper/searchFilePath",myFileNameQString);
  emit dataChanged();
}

void OmgDataFetcherWizardSearchWidget::setSearchList(QString theFileName)
{
  //
  // Add all search strings from text file
  //
  //first build a regex to match text at the beginning of the line
  QRegExp myQRegExp( "^[^#][ a-zA-Z\\-]*" ); //seconf caret means 'not'
  QStringList myTaxonQStringList;;
  QFile myQFile( theFileName );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the file, checking each line for its taxon string
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLineQString;
    while ( !myQTextStream.atEnd() )
    {
      myCurrentLineQString = myQTextStream.readLine(); // line of text excluding '\n'
      int  myPosInt = myQRegExp.indexIn( myCurrentLineQString,0 );
      if (myPosInt<0) continue;
      QStringList myMatchesQStringList = myQRegExp.capturedTexts();
      QStringList::Iterator myIterator = myMatchesQStringList.begin();
      QString myTaxonQString=*myIterator;
      myTaxonQString=myTaxonQString.simplified();
      if (myTaxonQString != "")
      {
        //make sure there are only single spaces separating words.
        myTaxonQString=myTaxonQString.replace( QRegExp(" {2,}"), " " );
        myTaxonQStringList.append(myTaxonQString);
      }
    }
    myQFile.close();
    //sort the taxon list alpabetically
    myTaxonQStringList.sort();
    //now find the uniqe entries in the qstringlist and
    //add each entry to the list
    QString myLastTaxon="";
    QStringList::Iterator myIterator= myTaxonQStringList.begin();
    while( myIterator!= myTaxonQStringList.end() )
    {
      QString myCurrentTaxon=*myIterator;
      if (myCurrentTaxon!=myLastTaxon)
      {
        lstSearch->addItem(myCurrentTaxon);
      }
      myLastTaxon=*myIterator;
      ++myIterator;
    }
  }
  else
  {
    QMessageBox::warning( this,QString(tr("Localities Search  Error")),QString(tr("The file is not readable. Check you have the neccessary file permissions and try again.")));
    return;
  }

}

void OmgDataFetcherWizardSearchWidget::on_toolAddSearchItem_clicked()
{
  lstSearch->addItem(leSearchString->text());
  emit dataChanged();
}

void OmgDataFetcherWizardSearchWidget::on_toolDeleteItem_clicked()
{

  unsigned int myItemsCount = static_cast<unsigned int>(lstSearch->count());
  for ( unsigned int myInt = 0; myInt < myItemsCount; myInt++ )
  {
    QListWidgetItem *myItem = lstSearch->item( myInt );
    // if the item is selected...
    if ( lstSearch->isItemSelected(myItem) )
    {
      //remove the item if it is selected
      //this method is described in the qt docs but gives a compile errror saying the error is non existant!
      //lstProjectionLayers->removeItem(myInt);
      //so we kludge it for now!
      lstSearch->takeItem(myInt);
      myInt--;
      myItemsCount--;
    }
  }
  emit dataChanged();
}

void OmgDataFetcherWizardSearchWidget::on_toolSelectOutputPath_clicked()
{
  QString myOutputPathQString = QFileDialog::getExistingDirectory(
      this,
      tr("Choose an output folder"),
      "ShowDirsOnly");
  leOutputPath->setText(myOutputPathQString);
  emit dataChanged();

}

void OmgDataFetcherWizardSearchWidget::outputPathChanged()
{
  emit dataChanged();
}

