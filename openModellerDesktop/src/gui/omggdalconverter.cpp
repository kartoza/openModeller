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

#include "omggdalconverter.h"
#include "omggdal.h"
#include "omglayerselector.h"

//qt includes
#include <QSettings>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>

OmgGdalConverter::OmgGdalConverter(QWidget* parent, Qt::WFlags fl)
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

OmgGdalConverter::~OmgGdalConverter()
{
  delete mpOmgGdal;
}

void OmgGdalConverter::accept()
{
  writeSettings();
  done(1);
}

void OmgGdalConverter::on_pbnSelectFile_clicked()
{
  QStringList myList;
  QSettings mySettings;
  QString myBaseDir = mySettings.value("gdalconverter/layersDirectory","c:\tmp").toString(); //initial dir
  OmgLayerSelector myOmgLayerSelector (myBaseDir, this);
  if(myOmgLayerSelector.exec())
  {
    myList=myOmgLayerSelector.getSelectedLayers();
    mySettings.setValue("gdalconverter/layersDirectory",myOmgLayerSelector.getBaseDir());
    lstInputFiles->addItems(myList);
  }
}

void OmgGdalConverter::on_pbnSelectDir_clicked()
{
  QString myDir = QFileDialog::getExistingDirectory(
                    this,
                    "Choose a directory",
                    mLastDir);

  mLastDir = myDir;
  leOutputDir->setText(myDir);
}

void OmgGdalConverter::on_pbnProcess_clicked()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Iterate through file list and process
  for ( int myFileInt = 0; myFileInt < lstInputFiles->count(); myFileInt++ )
  {
    QListWidgetItem *myFileItem = lstInputFiles->item( myFileInt );
    if (cboOutputFileType->currentText()=="GeoTiff")
    {
      qDebug("Converting " + myFileItem->text().toLocal8Bit()
          + " to tiff");
      mpOmgGdal->convert(myFileItem->text(), leOutputDir->text(), OmgGdal::GeoTiff);
    }
    else
    {
      mpOmgGdal->convert(myFileItem->text(), leOutputDir->text(), OmgGdal::ArcInfoAscii);
      qDebug("Converting " + myFileItem->text().toLocal8Bit()
          + " to ascii");
      
    }
    progressBar->reset();
  }
  QApplication::restoreOverrideCursor();
  QMessageBox::information( this,QString(tr("Conversion result")),QString(tr("File format conversion completed successfully.")));
  accept();
}

void OmgGdalConverter::readSettings()
{
  QSettings mySettings;
  //leFileName->setText(mySettings.value("gdalconverter/filename", "").toString());
  mLastDir=(mySettings.value("gdalconverter/lastdir",".").toString());
}

void OmgGdalConverter::writeSettings()
{
  QSettings mySettings;
  //mySettings.setValue("gdalconverter/filename", leFileName->text());
  mySettings.setValue("gdalconverter/lastdir",mLastDir);
}

void OmgGdalConverter::error(QString theError)
{
  QMessageBox::warning( this,QString(tr("File Format Converter Error")),theError);
}

void OmgGdalConverter::updateProgress (int theCurrentValue, int theMaximumValue)
{
  progressBar->setMaximum(theMaximumValue);
  progressBar->setValue(theCurrentValue);
  QApplication::processEvents();
}
