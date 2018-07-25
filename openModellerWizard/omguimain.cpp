//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "omguimain.h"
#include <qpixmap.h>
#include <qlabel.h>
#include <qstring.h>
#include "openmodellergui.h"
#include "omguiabout.h"
#include <qsettings.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qscrollview.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qtextstream.h>

#include <assert.h>

//om includes
#include <openmodeller/om.hh>
    OmGuiMain::OmGuiMain()
: OmGuiMainBase()
{
  //set up a scrollviewdesigner
  QGridLayout *myLayout = new QGridLayout(frameImage,1,1);
  QScrollView *myScrollView = new QScrollView(frameImage);

  myLayout->addWidget(myScrollView,0,0,0);
  QVBox * myVBox = new QVBox (myScrollView->viewport());
  myScrollView->addChild(myVBox);
  mPictureWidget = new QLabel(myVBox);
  myScrollView->setResizePolicy(QScrollView::AutoOneFit);

  mCurrentModel = 0;
  btnFirst->setEnabled(false);
  btnPrevious->setEnabled(false);
  btnLast->setEnabled(false);
  btnNext->setEnabled(false);
  //show();
  //
  // Note this function searches for om plugins - and should only
  // ever be run once in the life of the qgis session (the om
  // plugin registry is a singleton), so we do it here
  // rather than in the ctor of the plugin.
  //
  AlgorithmFactory::searchDefaultDirs();
  //runWizard();
}


OmGuiMain::~OmGuiMain()
{}

void OmGuiMain::fileExit()
{
  close();
}
void OmGuiMain::runWizard()
{
  OpenModellerGui * myOpenModellerGui = new OpenModellerGui(this,"openModeller Wizard",true,0);
  connect(myOpenModellerGui, SIGNAL(modelDone(QString, QString, QString)), this, SLOT(modelDone(QString, QString, QString)));
  connect(myOpenModellerGui, SIGNAL(jobComplete()), this, SLOT(btnFirst_clicked()));

  myOpenModellerGui->exec();
  show();
}

void OmGuiMain::helpAboutAction_activated()
{
  qDebug("Help About");
  OmGuiAbout *myAboutGui = new OmGuiAbout(this,"About");
  myAboutGui->exec();
}

/**

  Convenience function for readily creating file filters.

  Given a long name for a file filter and a regular expression, return
  a file filter string suitable for use in a QFileDialog::OpenFiles()
  call.  The regular express, glob, will have both all lower and upper
  case versions added.

*/
static QString createFileFilter_(QString const &longName, QString const &glob)
{
  return longName + " (" + glob.lower() + " " + glob.upper() + ");;";
}                               // createFileFilter_



void OmGuiMain::drawModelImage(QString theFileName)
{
  //set the image label on the calculating variables screen to show the last
  //variable calculated
  std::cout << "drawModelImage Called" << std::endl;
  QPixmap myPixmap(theFileName);
  mPictureWidget->setPixmap(myPixmap);
  mPictureWidget->show();
  //make sure the main gui windows shows (its off when app starts!)
  show();
}


void OmGuiMain::saveMapAsImage()
{
  //create a map to hold the QImageIO names and the filter names
  //the QImageIO name must be passed to the pixmap saveas image function
  typedef QMap<QString, QString> FilterMap;
  FilterMap myFilterMap;

  //find out the last used filter
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastUsedFilter = myQSettings.readEntry("/openmodeller/saveAsImageFilter");
  QString myLastUsedDir = myQSettings.readEntry("/openmodeller/lastSaveAsImageDir",".");


  // get a list of supported output image types
  int myCounterInt=0;
  QString myFilters;
  for ( ; myCounterInt < QImageIO::outputFormats().count(); myCounterInt++ )
  {
    QString myFormat=QString(QImageIO::outputFormats().at( myCounterInt ));
    QString myFilter = createFileFilter_(myFormat + " format", "*."+myFormat);
    myFilters += myFilter;
    myFilterMap[myFilter] = myFormat;
  }

  std::cout << "Available Filters Map: " << std::endl;
  FilterMap::Iterator myIterator;
  for ( myIterator = myFilterMap.begin(); myIterator != myFilterMap.end(); ++myIterator )
  {
    std::cout << myIterator.key() << "  :  " << myIterator.data() << std::endl;
  }

  //create a file dialog using the filter list generated above
  std::auto_ptr < QFileDialog > myQFileDialog(
          new QFileDialog(
              myLastUsedDir,
              myFilters,
              0,
              QFileDialog::tr("Save file dialog"),
              tr("Choose a filename to save the map image as")
              )
          );


  // allow for selection of more than one file
  myQFileDialog->setMode(QFileDialog::AnyFile);

  if (myLastUsedFilter!=QString::null)       // set the filter to the last one used
  {
    myQFileDialog->setSelectedFilter(myLastUsedFilter);
  }


  //prompt the user for a filename
  QString myOutputFileNameQString; // = myQFileDialog->getSaveFileName(); //delete this
  if (myQFileDialog->exec() == QDialog::Accepted)
  {
    myOutputFileNameQString = myQFileDialog->selectedFile();
  }

  QString myFilterString = myQFileDialog->selectedFilter()+";;";

  std::cout << "Selected filter: " << myFilterString << std::endl;
  std::cout << "Image type to be passed to pixmap saver: " << myFilterMap[myFilterString] << std::endl;

  myQSettings.writeEntry("/openmodeller/lastSaveAsImageFilter" , myFilterString);
  myQSettings.writeEntry("/openmodeller/lastSaveAsImageDir", myQFileDialog->dirPath());

  if (!myOutputFileNameQString.isEmpty())
  {
    mPictureWidget->pixmap()->save( myOutputFileNameQString, myFilterMap[myFilterString], -1);
  }

}

void OmGuiMain::modelDone(QString theImageFileName, QString theLogFileName, QString theTaxonName)
{
  // clean up the loggin stuff
  g_log.setCallback( 0 );
  // remove the GUI

  ModelResult myModelResult;

  myModelResult.imageFileName=theImageFileName;
  myModelResult.logFileName=theLogFileName;
  myModelResult.taxonName=theTaxonName;

  mModelVector.push_back(myModelResult);
  qDebug("Model done " + theTaxonName + " : " + QString::number(mModelVector.count()));
}

void OmGuiMain::btnFirst_clicked ()
{
  mCurrentModel = 1;

  if (mModelVector.count()==1)
  {
    btnFirst->setEnabled(false);
    btnPrevious->setEnabled(false);
    btnLast->setEnabled(false);
    btnNext->setEnabled(false);
  }
  else if (mModelVector.count()>1)
  {
    btnFirst->setEnabled(false);
    btnPrevious->setEnabled(false);    
    btnLast->setEnabled(true);
    btnNext->setEnabled(true);
  }

  showModel();
}


void OmGuiMain::btnLast_clicked ()
{
  mCurrentModel = mModelVector.count();
  btnLast->setEnabled(false);
  btnNext->setEnabled(false);

  if (mModelVector.count()>1)
  {
    btnFirst->setEnabled(true);
    btnPrevious->setEnabled(true);
  }
  showModel();
}



void OmGuiMain::btnNext_clicked ()
{
  qDebug ("The current value is " + QString::number(mCurrentModel));

  mCurrentModel++;
  if (mCurrentModel==mModelVector.count())
  {
    btnLast->setEnabled(false);
    btnNext->setEnabled(false);
  }

  if (mCurrentModel>1)
  {
    btnFirst->setEnabled(true);
    btnPrevious->setEnabled(true);
  }
  showModel();
}



void OmGuiMain::btnPrevious_clicked ()
{
  mCurrentModel--;
  if (mCurrentModel==1)
  {
    btnFirst->setEnabled(false);
    btnPrevious->setEnabled(false);
  }
  if (mCurrentModel<mModelVector.count())
  {
    btnLast->setEnabled(true);
    btnNext->setEnabled(true);
  }
  showModel();
}


void OmGuiMain::setTaxonName()
{
}


void OmGuiMain::showModel()
{

  if (mModelVector.count()==0)
  {
    return;
  }//Set log file text
  txtbLog->clear(); 
  QString myText;
  assert(mCurrentModel<=mModelVector.count());
  QString myLogFileName = mModelVector.at(mCurrentModel-1).logFileName;
  qDebug ("Loading log file: " + myLogFileName);
  QFile myFile( mModelVector.at(mCurrentModel-1).logFileName );
  if ( myFile.open( IO_ReadOnly ) )
  {
    QTextStream myStream( &myFile );
    while ( !myStream.atEnd() )
    {
      myText += myStream.readLine(); // line of text excluding '\n'
    }
    myFile.close();
  }
  txtbLog->setText(myText);

  //Set taxon name
  QString myTaxonName = mModelVector.at(mCurrentModel-1).taxonName;
  qDebug("Fetching taxon name : "+ myTaxonName);
  lblCurrentTaxon->setText(mModelVector.at(mCurrentModel-1).taxonName);

  //Set image
  QString myImageFileName = mModelVector.at(mCurrentModel-1).imageFileName;
  qDebug("Fetching image file name : " + myImageFileName);
  drawModelImage(mModelVector.at(mCurrentModel-1).imageFileName);

  //Set gui label
  lblPosition->setText(QString::number(mCurrentModel) + " of " + QString::number(mModelVector.count()));
}

void OmGuiMain::saveProject()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastFileName = myQSettings.readEntry("/openmodeller/lastSavedProject");
  QString myFileName = QFileDialog::getSaveFileName(
          myLastFileName,
          "OmGui Project (*.omg)",
          this,
          "Save project as...",
          "Choose a filename to save under" );
  myQSettings.writeEntry("/openmodeller/lastSavedProject",myFileName);
  QFile myFile(myFileName );
  if ( myFile.open( IO_WriteOnly ) ) 
  {
    QTextStream myStream( &myFile );
    for (int i=0;i<mModelVector.count();i++)
    {
      myStream << "---\n";
      myStream << mModelVector.at(i).imageFileName << "\n";
      myStream << mModelVector.at(i).logFileName << "\n";
      myStream << mModelVector.at(i).taxonName << "\n";
    }
    myFile.close();
  }
}
void OmGuiMain::loadProject()
{
  QSettings myQSettings;  // where we keep last used filter in persistant state
  QString myLastFileName = myQSettings.readEntry("/openmodeller/lastSavedProject");
  QString myFileName = QFileDialog::getOpenFileName(
          myLastFileName,
          "OmGui Project (*.omg)",
          this,
          "Load Project",
          "Choose a previously saved omgui project" );
  myQSettings.writeEntry("/openmodeller/lastSavedProject",myFileName);

  QFile myFile( myFileName );
  if ( myFile.open( IO_ReadOnly ) )
  {
    mModelVector.clear();
    QTextStream myStream( &myFile );
    while ( !myStream.atEnd() )
    {
      QString myText = myStream.readLine(); // line of text excluding '\n'
      //see if we are at the start of the record
      if (myText.compare(QString("---"))==0)
      {
        QString myImageFileName = myStream.readLine();
        QString myLogFileName = myStream.readLine();
        QString myTaxonName = myStream.readLine();
        ModelResult myModelResult;
        myModelResult.imageFileName=myImageFileName;
        myModelResult.logFileName=myLogFileName;
        myModelResult.taxonName=myTaxonName;
        mModelVector.push_back(myModelResult);
        
      }
    }
    myFile.close();
  }
  btnFirst_clicked();  
}
