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

#include <omgexperimentprogress.h>
#include <omgexperiment.h>
//qt includes
#include <QPalette>
#include <QMessageBox>
#include <QColor>
#include <QColorGroup>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QResizeEvent>
#include <QHeaderView>
#include <QIcon>
#include <QPixmap>
#include <QGroupBox>
#include <QPushButton>
#include <QSettings>
#include <QTextCursor>
#include <QFileInfo>
// QGIS Includes
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
OmgExperimentProgress::OmgExperimentProgress(QWidget* parent, Qt::WFlags fl)
: QDialog(parent,fl),
  mpTimer(new QTimer(this))
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  pbnCancel->hide();
  progressTotal->setMaximum(1);
  progressTotal->setValue(0); 
  mExperimentSetFlag = false;
  //hide the table headers (they should be obvious)
  tblQueue->horizontalHeader()->hide();
  tblQueue->verticalHeader()->hide();
  //make sure the second & third col of the table always 
  //expands to take all available space
  //see resize() below to see how cols are maintained during resize
  tblQueue->horizontalHeader()->setResizeMode(0,QHeaderView::Interactive);
  tblQueue->horizontalHeader()->resizeSection(0,100);
  tblQueue->horizontalHeader()->setResizeMode(1,QHeaderView::Interactive);
  tblQueue->horizontalHeader()->resizeSection(1,30);
  tblQueue->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
  pbnOk->setEnabled(false);
  QSettings mySettings;
	// whether to show log pane or not 
	bool myFlag=mySettings.value("experimentProgress/showProgressLog","0").toBool();
  if (myFlag)
  {
    grpLogMessages->setVisible(true);
  }
  else
  {
    grpLogMessages->setVisible(false);
  }
  //make sure to update the display so user sees widgets properly
  //@TODO implement experiment as a threaded class!!
  connect(mpTimer, SIGNAL(timeout()), this, SLOT(update()));
  mpTimer->start(1000); //every 1 sec
}
OmgExperimentProgress::~OmgExperimentProgress()
{
  mpTimer->stop(); //every 1 sec
  delete mpTimer;
  //note we do not delete the experiment pointer here as this class
  //does not own the experiment!
}

void OmgExperimentProgress::on_toolShowLog_clicked()
{
  QSettings mySettings;
  grpLogMessages->setVisible(true);
  mySettings.setValue("experimentProgress/showProgressLog",1);
}
void OmgExperimentProgress::on_toolHideLog_clicked()
{
  QSettings mySettings;
  grpLogMessages->setVisible(false);
  mySettings.setValue("experimentProgress/showProgressLog",0);
}

void OmgExperimentProgress::resizeEvent ( QResizeEvent * theEvent )
{
  tblQueue->horizontalHeader()->resizeSection(0,100);
  tblQueue->horizontalHeader()->resizeSection(1,30);
  tblQueue->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
}

void OmgExperimentProgress::setExperiment(OmgExperiment * theExperiment)
{
  mpExperiment = theExperiment;
  tblQueue->clear();
  tblQueue->setRowCount(mpExperiment->count());
  tblQueue->setColumnCount(3);

  //poplate the hash with progrss bars
  int myCount = mpExperiment->count();
  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel = mpExperiment->getModel(i);
    //first col gets a progress bar
    QProgressBar * mypProgress = new QProgressBar(this);
    mypProgress->setEnabled(false);
    mProgressBarsHash.insert(mypModel->guid(),mypProgress);
    tblQueue->setCellWidget ( i, 0, mypProgress);
    //second col gets a little icon showing job state
    QTableWidgetItem *mypIconItem = new QTableWidgetItem();
    QIcon myIcon;
    if (mypModel->isCompleted())
    {
      if (mypModel->hasError())
      {
        myIcon.addFile(":/status_aborted.png");
      }
      else
      {
        myIcon.addFile(":/status_complete.png");
        mypProgress->setValue(100);
      }
    }
    else
    {
        myIcon.addFile(":/status_queued.png");
    }
    mypIconItem->setIcon(myIcon);
    mIconWidgetsHash.insert(mypModel->guid(),mypIconItem);
    tblQueue->setItem(i, 1, mypIconItem);
    //third col gets some text info
    //Items are created ouside the table (with no parent widget) and inserted into the table with setItem():
    QString myAlgName = mypModel->algorithm().name();
    QString myTaxon = mypModel->taxonName();
    QTableWidgetItem *mypTextItem = new QTableWidgetItem(myTaxon + " - " + myAlgName);
    tblQueue->setItem(i, 2, mypTextItem);
    //set custom colours for the progress bar
    QPalette myPalette = mypProgress->palette();
    // Set the bar red
    myPalette.setColor(QPalette::Highlight, QColor("red"));
    // Set the percentage text yellow
    myPalette.setColor(QPalette::WindowText, QColor("yellow"));
    // Set the background green
    //myPalette.setColor(QPalette::Window, QColor("green"));
    mypProgress->setPalette(myPalette);
  }
  // Get the messenger instance that arbitrates comms 
  // between the plugin and our gui
  //
  const OmgPluginMessenger * mypMessenger = 
    mpExperiment->modellerPlugin()->getMessenger();
  //
  // Connect the signals from the modeller adapter
  // used for monitoring the progress of individual models
  //
  connect(mypMessenger, SIGNAL(modelCreationProgress(QString,int )),
      this, SLOT(setModelCreationProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelProjectionProgress(QString,int )),
      this, SLOT(setModelProjectionProgress(QString,int)));
  connect(mypMessenger, SIGNAL(modelMessage(QString,QString )),
      this, SLOT(logMessage(QString,QString)));
  connect(mypMessenger, SIGNAL(modelError(QString,QString )),
      this, SLOT(logError(QString,QString)));
  connect(mpExperiment, SIGNAL(experimentProgress(int )),
      this, SLOT(setExperimentProgress(int)));
  setExperimentMaximum( mpExperiment->count() );
  
  //loop through all the models creating a table
  //row and a progress bar for each so that we can see their progress
  mExperimentSetFlag = true;
  show();
  update();
  mpExperiment->run();
  //qDebug("Experiment set for ExperimentProgress dialog");
}

void OmgExperimentProgress::on_pbnCancel_clicked()
{
  mpExperiment->abort();
  //tell the master progress bar this is the last mode now
  progressTotal->setMaximum(progressTotal->value()+1);
  pbnCancel->setText(tr("Cancelling..."));
  pbnCancel->setEnabled(false);
}
void OmgExperimentProgress::setModelCreationProgress (QString theModelGuid, int theProgress)
{
  //qDebug("Model creation progress fired");
  //next bit for debugging
  if ( ! mProgressBarsHash.contains( theModelGuid ) )
  {
    QString myModelsList;
    QHashIterator<QString, QProgressBar *> i( mProgressBarsHash );
    while (i.hasNext()) 
    {
      i.next();
      myModelsList += i.key();
    }
    //QMessageBox::warning( this,tr("openModeller Desktop"),tr("Creation Not found: %1\n%2").arg(theModelGuid).arg(myModelsList) );
    return;
  }
  //debugging end
  QProgressBar * mypProgress = mProgressBarsHash[theModelGuid];
  mypProgress->setValue(theProgress);
  //a sneaky way to make sure we dont try to draw the icon with each
  //progress bar update...
  if (!mypProgress->isEnabled())
  {
    mypProgress->setEnabled(true);
    QTableWidgetItem * mypIconItem = mIconWidgetsHash[theModelGuid];
    QIcon myIcon;
    myIcon.addFile(":/status_running.png");
    mypIconItem->setIcon(myIcon);
  }
  update(); //make sure the screen is refreshed
  QApplication::processEvents();
}

void OmgExperimentProgress::setModelProjectionProgress (QString theProjectionGuid, int theProgress)
{
  OmgProjection const * mypProjection = mpExperiment->getProjection( theProjectionGuid );
  if (NULL == mypProjection) 
  {
    emit logMessage( "Projection is NULL for " + theProjectionGuid );
    return ;
  }
  QString myModelGuid = mypProjection->modelGuid();
  //debugging end
  QProgressBar * mypProgress = mProgressBarsHash[myModelGuid];
  //TODO make this colour change happen only when projection starts
  QPalette myPalette = mypProgress->palette();
  // Set the bar red
  myPalette.setColor(QPalette::Highlight, QColor("green"));
  // Set the percentage text yellow
  myPalette.setColor(QPalette::WindowText, QColor("yellow"));
  // Set the background green
  //myPalette.setColor(QPalette::Window, QColor("green"));
  mypProgress->setPalette(myPalette);
  mypProgress->setValue(theProgress);
  if (theProgress >= 100)
  {
    mypProgress->setEnabled(true);
    QTableWidgetItem * mypIconItem = mIconWidgetsHash[ myModelGuid ];
    QIcon myIcon;
    myIcon.addFile(":/status_complete.png");
    mypIconItem->setIcon(myIcon);
  }
  if ( theProgress == 100 )
  { 
    QFileInfo myRasterFileInfo( mypProjection->workDir() + mypProjection->rawImageFileName() );
    if (!myRasterFileInfo.exists())
    {
      qDebug("Projection projection does not exist, no raster later will be loaded");
      return;
    }
    QgsRasterLayer *  mypRasterLayer = new QgsRasterLayer ( myRasterFileInfo.filePath(),
        myRasterFileInfo.completeBaseName() );
    mypRasterLayer->setDrawingStyle( QgsRasterLayer::SingleBandPseudoColor );
    mypRasterLayer->setColorShadingAlgorithm(QgsRasterLayer::PseudoColorShader);  
    //mypRasterLayer->setContrastEnhancementAlgorithm(
    //    QgsContrastEnhancement::StretchToMinimumMaximum, false);
    //mypRasterLayer->setMinimumValue( mypRasterLayer->grayBandName(),0.0, false );
    //mypRasterLayer->setMaximumValue( mypRasterLayer->grayBandName(),10.0 );
    // register this layer with the central layers registry
    QgsMapLayerRegistry::instance()->addMapLayer( mypRasterLayer );
  }
  update(); //make sure the screen is refreshed
  QApplication::processEvents();
}

void OmgExperimentProgress::setExperimentProgress (int theProgress)
{
  lblExperimentProgress->setText(tr("Experiment Progress (")
      + QString::number(theProgress) 
      + "/"
      + QString::number(progressTotal->maximum()) +")");
  progressTotal->setValue(theProgress);
  if (theProgress==progressTotal->maximum() )
  {
    pbnOk->setEnabled(true);
  }
  else if(pbnCancel->text() !=tr("Cancelling..."))
  {
    pbnOk->setEnabled(false);
  }
}
void OmgExperimentProgress::setExperimentMaximum(int theMaximum)
{
  lblExperimentProgress->setText(tr("Experiment Progress (") 
      +"0/" 
      + QString::number(theMaximum) +")");
  progressTotal->setMaximum(theMaximum);
  progressTotal->setValue(0);
}
void OmgExperimentProgress::logMessage(QString theMessage)
{
  teLog->append(theMessage);
  teLog->ensureCursorVisible();
}
void OmgExperimentProgress::logMessage(QString theModelGuid, QString theMessage)
{
  teLog->append(theMessage);
  teLog->ensureCursorVisible();
}
void OmgExperimentProgress::logError(QString theModelGuid, QString theMessage)
{
  //since we want to insert html and there is no append option
  //for html, we need to manually make sure we are at the end of the file
  teLog->textCursor().movePosition(QTextCursor::End);
  teLog->insertHtml("<br/><font color=\"red\">" + theMessage + "</font><font color=\"black\"> * </font><br/>");
  teLog->ensureCursorVisible();
}
