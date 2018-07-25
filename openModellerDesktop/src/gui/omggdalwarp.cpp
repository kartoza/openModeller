/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton and Peter Brewer   *
 *   tim@linfiniti.com and dovecote@pembshideaway.co.uk  *
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

#include <omggdalwarp.h>
#include <omggdal.h>

//qt includes
#include <QSettings>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QFileInfo>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>

OmgGdalWarp::OmgGdalWarp(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl) ,
    mpOmgGdal (new OmgGdal()),
    mFilterList(QString())
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  readSettings();
  connect(mpOmgGdal, SIGNAL(error(QString)), this, SLOT(error(QString)));
  connect(mpOmgGdal, SIGNAL(updateProgress(int, int )), this, SLOT(updateProgress(int, int )));
  connect(mpOmgGdal, SIGNAL(convertComplete()), this, SLOT(convertComplete()));
  mpOmgGdal->buildSupportedRasterFileFilter(mFilterList);
}

OmgGdalWarp::~OmgGdalWarp()
{}

void OmgGdalWarp::accept()
{
  writeSettings();
  done(1);
}

void OmgGdalWarp::on_pbnSelectFile_clicked()
{
  QSettings mySettings;
  QString myBaseDir = mySettings.value("gdalwarp/inputLayersDirectory","/tmp").toString(); //initial dir
  //! @TODO Auto get a list of filters from gdal (well omgdal actually)
  QStringList myFileNameQStringList = QFileDialog::getOpenFileNames(
                                        this,
                                        "Select one or more files",
                                        myBaseDir, //initial dir
                                        mFilterList  //filters to select
                                      );
  QFileInfo myFileInfo(myFileNameQStringList.at(0));
  myBaseDir = myFileInfo.dir().path();

    mySettings.setValue("gdalwarp/inputLayersDirectory",myBaseDir);
  QStringList::Iterator it = myFileNameQStringList.begin();
  while(it != myFileNameQStringList.end())
  {
    lstInputFiles->addItem(*it);
    ++it;
  }
}

void OmgGdalWarp::on_pbnSelectDir_clicked()
{
  QSettings mySettings;
  QString myBaseDir = mySettings.value("gdalwarp/outputDirectory","/tmp").toString(); //initial dir
  QString myDir = QFileDialog::getExistingDirectory(
                    this,
                    "Choose a directory",
                    myBaseDir);

  mySettings.setValue("gdalwarp/outputDirectory",myDir);

  leOutputDir->setText(myDir);
}

void OmgGdalWarp::on_pbnProcess_clicked()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Iterate through file list and process
  for ( int myFileInt = 0; myFileInt < lstInputFiles->count(); myFileInt++ )
  {
    //QListWidgetItem *myFileItem = lstInputFiles->item( myFileInt );
  //  mpOmgGdal->gdalResize(myFileItem->text(), leOutputDir->text(), leWidth->text().toInt(), leHeight->text().toInt());
  }
  progressBar->reset();
  QApplication::restoreOverrideCursor();
}


void OmgGdalWarp::readSettings()
{
  QSettings mySettings;
}

void OmgGdalWarp::writeSettings()
{
  QSettings mySettings;
}

void OmgGdalWarp::error(QString theError)
{
  QMessageBox::warning( this,QString(tr("File Format Converter Error")),theError);
}

void OmgGdalWarp::updateProgress (int theCurrentValue, int theMaximumValue)
{
  progressBar->setMaximum(theMaximumValue);
  progressBar->setValue(theCurrentValue);
  QApplication::processEvents();
}

QStringList OmgGdalWarp::getListWidgetEntries(const QListWidget * theListWidget)
{
  QStringList myList;
  for ( int myCounter = 0; myCounter < theListWidget->count(); myCounter++ )
  {
    myList.append(theListWidget->item(myCounter)->text());
  }
  return myList;
}
