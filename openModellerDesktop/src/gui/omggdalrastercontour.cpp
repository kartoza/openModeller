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

#include "omggdalrastercontour.h"
#include "omglayerselector.h"

//qt includes
#include <QSettings>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>

OmgGdalRasterContour::OmgGdalRasterContour(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl) 
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  readSettings();
  connect(&mOmgGdal, SIGNAL(error(QString)), this, SLOT(error(QString)));
  connect(&mOmgGdal, SIGNAL(updateProgress(int, int )), this, SLOT(updateProgress(int, int )));
  //connect(&mOmgGdal, SIGNAL(contourComplete()), this, SLOT(contourComplete()));
}

OmgGdalRasterContour::~OmgGdalRasterContour()
{}

void OmgGdalRasterContour::accept()
{
  writeSettings();
  done(1);
}

void OmgGdalRasterContour::on_pbnSelectFile_clicked()
{
  QStringList myList;
  QSettings mySettings;
  QString myBaseDir = mySettings.value("gdalrastercontour/layersDirectory","c:\tmp").toString(); //initial dir
  OmgLayerSelector myOmgLayerSelector (myBaseDir, this);
  if(myOmgLayerSelector.exec())
  {
    myList=myOmgLayerSelector.getSelectedLayers();
    mySettings.setValue("gdalrastercontour/layersDirectory",myOmgLayerSelector.getBaseDir());
    lstInputFiles->addItems(myList);
  }
}

void OmgGdalRasterContour::on_pbnSelectDir_clicked()
{
  QSettings mySettings;
  QString myLastDir = mySettings.value("gdalrastercontour/outputDirectory","c:\tmp").toString(); //initial dir
  QString myDir = QFileDialog::getExistingDirectory(
                    this,
                    "Choose a directory",
                    myLastDir);

  mySettings.setValue("gdalrastercontour/outputDirectory",myDir);
  leOutputDir->setText(myDir);
}

void OmgGdalRasterContour::on_pbnProcess_clicked()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Iterate through file list and process
  for ( int myFileInt = 0; myFileInt < lstInputFiles->count(); myFileInt++ )
  {
    QListWidgetItem *myFileItem = lstInputFiles->item( myFileInt );
    mOmgGdal.contour(myFileItem->text());
    //mOmgGdal.contour(myFileItem->text(), leOutputDir->text());
    progressBar->reset();
  }
  QApplication::restoreOverrideCursor();
}

void OmgGdalRasterContour::readSettings()
{
}

void OmgGdalRasterContour::writeSettings()
{
}

void OmgGdalRasterContour::error(QString theError)
{
  QMessageBox::warning( this,QString(tr("File Format Converter Error")),theError);
}

void OmgGdalRasterContour::updateProgress (int theCurrentValue, int theMaximumValue)
{
  progressBar->setMaximum(theMaximumValue);
  progressBar->setValue(theCurrentValue);
  QApplication::processEvents();
}
