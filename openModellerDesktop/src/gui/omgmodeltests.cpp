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

//local includes
#include "omgmodeltests.h"

#include "omgalgorithm.h"
#include "omgmodellerpluginregistry.h"

#include <omgui.h> //provides anciliary helper functions like getting app paths
// OM
#include <omglocality.h>

//qt includes
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>


OmgModelTests::OmgModelTests(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent,fl),
  mpModellerPlugin(NULL)
{
  //required by Qt4 to initialise the ui
  setupUi(this);

  // Get last user values
  QSettings mySettings;

  //when the user clicks on the addItems tool in the omglistTaxa widget
  //they should be prompted to load a file of occurrence data
  connect(pbnRun, SIGNAL(clicked()),this, SLOT(accept()));
  connect(pbnClose, SIGNAL(clicked()),this, SLOT(reject()));
  connect(omglistTaxa, SIGNAL(addItemClicked()),this, SLOT(toolSelectLocalitiesFileClicked()));
  connect(omglistTaxa, SIGNAL(configureClicked()),this, SLOT(toolConfigureLocalitiesClicked()));

  omglistAlgorithms->hideOptionsTool();
  omglistAlgorithms->hideAddItemTool();
  progressBar->reset();
  updateProgress(0, 1);
}

OmgModelTests::~OmgModelTests()
{
  
}

void OmgModelTests::accept()
{
  qDebug("Starting to process external test request");

  // Save user values
  QSettings mySettings;

  // Insanity checks
  if( checks() )
  {
    pbnRun->setDisabled(true);

    // Test name
    myTestName = leTestName->text();

    // Get AlgorithmList
    myAlgorithmCount = omglistAlgorithms->checkedItemCount();
    myAlgorithmList = omglistAlgorithms->checkedDataItems();

    // Get Taxa list (é taxa ou taxon que escreve?)
    int myTaxaCount = omglistTaxa->checkedItemCount();
    QStringList myTaxaList = omglistTaxa->checkedItems();

    // Taxa loop
    for ( int myCounter = 0; myCounter < myTaxaCount; myCounter++ )
    {
      QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
      QString myTaxon, mySpeciesFileName;
      if (myLoaderType=="FILE") //read locs from a single file with many taxa
      {
        myTaxon = myTaxaList.at(myCounter);
        //emit logMessage ("Adding " + myTaxon.toLocal8Bit() + " to experiment");
        mySpeciesFileName = mSpeciesFileName;
      }
      else //list entries are file names from the dir
      {
        QString myFileName = myTaxaList.at(myCounter);
        //we need to find out the first
        QString myPrefix = 
          mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString();
        mySpeciesFileName = myPrefix + QDir::separator() + myFileName;
        QStringList mySpeciesList = getSpeciesList(myPrefix + QDir::separator() + myFileName);
        if (mySpeciesList.size()<1)
        {
          continue;
        }
        myTaxon=mySpeciesList.at(0);
        QString myMessage("Adding " + myFileName + " to experiment for taxon: " + myTaxon);
        //emit logMessage (myMessage);
        //qDebug(myMessage);
      }


      OmgLocalities myTestingLocalities;

      unsigned int myLocalitiesCount = loadLocalities(myTestingLocalities, mySpeciesFileName, myTaxon);

      // Algorithm loop inside Taxa loop
      for (int i=0; i < myAlgorithmCount; i++)
      {
        // Get existing model
        OmgModel * mypModel = mpExperiment->getModel(myTaxon, myAlgorithmList.at(i));

        if (mypModel == NULL)
        {
          QMessageBox::warning(this,tr("Model Exception"),
          tr("Could not find the corresponding model for ")+myTaxon+" / "+ myAlgorithmList.at(i));
          continue;
        }

        mpModellerPlugin = OmgModellerPluginRegistry::instance()->getPlugin();
        //mpModellerPlugin = mpExperiment->modellerPlugin();

        if (mpModellerPlugin == NULL)
        {
          QMessageBox::critical( this,tr("Plugin Exception"),
          tr("No modeller plugin could be found"));
          return;
        }

        mypModel->setModellerPlugin(mpModellerPlugin);

        // Add test
        OmgModelTest * myTest = new OmgModelTest(mypModel);
        myTest->setLabel(myTestName);
        myTest->setLocalities(myTestingLocalities);
        myTest->setInternal(false);

        if (myModelTestOptions.getUseLowestThreshold())
        {
          // Get existing tests
          QListIterator<OmgModelTest *> myTestIterator( mypModel->getModelTests() );

          double myThreshold = -1.0;
          int myNumBackgroundPoints = 1000;
          OmgLocalities myTrainingLocalities;

          // Find internal test
          while (myTestIterator.hasNext())
          {
            OmgModelTest * mypExistingTest = myTestIterator.next();

            // Note: there should always be an internal test! 
            if (mypExistingTest->isInternal())
            {
              // If there's a lowest threshold, get it
              if (mypExistingTest->useLowestThreshold())
              {
		  qDebug("Found lowest threshold on "+mypExistingTest->label());

                myThreshold = mypExistingTest->threshold();
              }
              else
              {
                // Get necessary data to run the internal test
                myNumBackgroundPoints = mypExistingTest->numBackgroundPoints();
                myTrainingLocalities = mypExistingTest->getLocalities();
	      }

              break;
            }
	  }

          if (myThreshold < 0.0)
          {
            qDebug("Running temporary test to find lowest threshold");

            // Calculate threshold using a copy of the internal test
            OmgModelTest * mypTemporaryTest = new OmgModelTest(mypModel);
            mypTemporaryTest->setLabel(QObject::tr("Temporary Test"));
            mypTemporaryTest->setLocalities(myTrainingLocalities);
            mypTemporaryTest->setInternal(true);
            mypTemporaryTest->setUseLowestThreshold(true);
            mypTemporaryTest->setNumBackgroundPoints(myNumBackgroundPoints);
            mpModellerPlugin->testModel(mypModel, mypTemporaryTest);

            if (mypTemporaryTest->hasError())
            {
              QMessageBox::warning( this,tr("Model Test Exception"),
              tr("Failed to complete model test for ")+myTaxon+" / "+ myAlgorithmList.at(i));
              delete mypTemporaryTest;
              continue;
            }

            myThreshold = mypTemporaryTest->threshold();

            qDebug("Lowest threshold calculated: " + QString::number(myThreshold).toLocal8Bit());

            delete mypTemporaryTest;
	  }

          myTest->setThreshold(myThreshold);
        }
        else
        {
          myTest->setThreshold(myModelTestOptions.getThreshold()/100);
        }

        myTest->setNumBackgroundPoints(myModelTestOptions.getNumBackgroundPoints());
        double myMaxOmission = myModelTestOptions.getMaxOmission();
        if (myMaxOmission < 100 )
        {
          myTest->setMaxOmission(myMaxOmission/100);
        }
        mypModel->addTest(myTest);
        mypModel->runTest(myTest);
        // Update HTML Report
        mypModel->toHtml(true);
        //mpExperiment->save();
        // Update Progress bar
        emit updateProgress(i+myCounter+2, myTaxaCount+myAlgorithmCount);
      }
    }
    QMessageBox::information( this,QString(tr("Message")),
      QString(tr("Finished processing tests")));

    clear();
    emit refreshExperiment();
  }
  pbnRun->setDisabled(false);

  qDebug("Finished processing external test request...");
}


void OmgModelTests::toolSelectLocalitiesFileClicked()
{
  QSettings mySettings;
  QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
  if (myLoaderType=="FILE") //read locs from a single file with many taxa
  {
    QString myFileTypeQString;
    QString myTextFileFilterString="Text File (*.txt)";
    QString myFileName = QFileDialog::getOpenFileName(
        this,
        "Select localities text file" , //caption
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(), //initial dir
        myTextFileFilterString,  //filters to select
        &myFileTypeQString //the pointer to store selected filter
        );
    //std::cout << "Selected filetype filter is : " << myFileTypeQString.toAscii() << std::endl;
    if (myFileName.isEmpty()) return;
    setSpeciesList(myFileName);

    //store directory where localities file is for next time
    QFileInfo myFileInfo(myFileName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myFileInfo.dir().path()+QString("/") );
  }
  else //read files from a dir
  {
    omglistTaxa->clear();
    QString myDirName = QFileDialog::getExistingDirectory(this, 
        tr("Select Directory"),
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(),
        QFileDialog::ShowDirsOnly);
    if (myDirName.isEmpty()) return;
    setSpeciesList(myDirName);
    mySettings.setValue("openModeller/experimentDesigner/localitiesFileDirectory", myDirName + QString("/") );
  }
} //end of on_pbnSelectLocalitiesFile_clicked


void OmgModelTests::setSpeciesList(QString theFileName)
{
  QSettings mySettings;
  QString myLoaderType = mySettings.value("localities/loaderType","FILE").toString();
  if (myLoaderType=="FILE") //read locs from a single file with many taxa
  {
    mSpeciesFileName=theFileName;
    //clear the existing entries in the taxon combo first
    omglistTaxa->clear();
    //
    // Now that we have the localities text file, we need to parse it and find
    // all unique instances of taxon names and populate the taxon combo...
    //
    QStringList myTaxonList = getSpeciesList(theFileName);
    //sort the taxon list alpabetically descending order
    //Qt4 built in sort does in ascending order so I wrote my own sort
    myTaxonList = Omgui::sortList(myTaxonList);
    //now find the uniqe entries in the qstringlist and
    //add each entry to the taxon combo
    QString myLastTaxon="";
    QStringList::Iterator myIterator= myTaxonList.begin();
    while( myIterator!= myTaxonList.end() )
    {
      QString myCurrentTaxon=*myIterator;
      if (myCurrentTaxon!=myLastTaxon)
      {
        if ( experimentSpeciesList.lastIndexOf(myCurrentTaxon) != -1 )
        {
          omglistTaxa->addItem(myCurrentTaxon,true);
        }
        myLastTaxon=*myIterator;
      }
      ++myIterator;
    }
  }
  else //read files from a dir
  {
    omglistTaxa->clear();
    QDir myDir(theFileName);
    myDir.setFilter( QDir::Files | QDir::Readable );
    myDir.setSorting( QDir::Name );
    QStringList myFilters;
    myFilters << "*.txt";
    myDir.setNameFilters( myFilters );

    QFileInfoList myList = myDir.entryInfoList();
    if (myList.size()==0)
    {
      QMessageBox::warning( this,tr("openModeller OmgModelTests Warning"),
          tr("No valid localities files were found. "
            "Make sure that your localities files have a .txt extension "
            "and are in the format:\n"
            "ID <tab> Taxon name <tab> longitude <tab> latitude"));
      mFirstNonIdRecord=false;
    }
    for (int myCount = 0; myCount < myList.size(); ++myCount) 
    {
      QFileInfo myFileInfo = myList.at(myCount);
      if (!myFileInfo.fileName().endsWith(".txt"))
      {
        continue;
      }
     
      bool addFileFlag = false;

      QStringList myTaxonList = getSpeciesList(myFileInfo.absoluteFilePath());
      //sort the taxon list alpabetically descending order
      //Qt4 built in sort does in ascending order so I wrote my own sort
      myTaxonList = Omgui::sortList(myTaxonList);
      //now find the uniqe entries in the qstringlist and
      //add each entry to the taxon combo
      QString myLastTaxon="";
      QStringList::Iterator myIterator= myTaxonList.begin();
      while( myIterator!= myTaxonList.end() )
      {
        QString myCurrentTaxon=*myIterator;
        if (myCurrentTaxon!=myLastTaxon)
        {
          if ( experimentSpeciesList.lastIndexOf(myCurrentTaxon) == -1 )
          {
             addFileFlag = true;
          }
          myLastTaxon=*myIterator;
        }
        ++myIterator;
      }

      if( addFileFlag )
      {
         omglistTaxa->addItem(myFileInfo.fileName(),true);
      }
    }
    //show how many files you have
    //lblFileCount->setText("("+QString::number(lstLocalitiesFiles->count())+")");
  }
} //end of setSpeciesList

QStringList OmgModelTests::getSpeciesList(QString theFileName)
{
  QStringList myTaxonList;
  QFile myQFile( theFileName );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
    unsigned int myCounter=0;
    while ( !myQTextStream.atEnd() )
    {
      myCurrentLine = myQTextStream.readLine(); // line of text excluding '\n'
      if (myCurrentLine.startsWith("#"))
      {
        continue;
      }
      //split on word boundaries ignoring empty parts
      QStringList myList = myCurrentLine.split(QRegExp("[\t]"));
      //qDebug("Read line : " + myList.join(" -- ").toLocal8Bit());
      if (myList.size() < 4)
      {
        continue;
      }
      else //new file format with at least 4 columns
      {
        QString myId=myList.at(0).simplified();
        QString myTaxonName=myList.at(1).simplified();
        if (myTaxonName!= "")
        {
          //make sure there are only single spaces separating words.
          myTaxonName=myTaxonName.replace( QRegExp(" {2,}"), " " );
          myTaxonList.append(myTaxonName);
          ++myCounter;
        }
      }
    }
    myQFile.close();
  }
  else
  {
    QMessageBox::warning( this,QString("openModeller OmgModelTests Error"),QString("The localities file is not readable. Check you have the neccessary file permissions and try again."));
  }
  return myTaxonList;
}

void OmgModelTests::toolConfigureLocalitiesClicked()
{
  myOptions.hideIndependentTest();
  myOptions.exec();
}

void OmgModelTests::setExperiment(OmgExperiment * const thepExperiment)
{
  progressBar->reset();
  updateProgress(0, 1);

  mpExperiment=thepExperiment;

  setAlgorithmList();

  setExperimentSpeciesList();
  //populateTree();
}

void OmgModelTests::setAlgorithmList()
{
  if (!mpExperiment)
  {
    return ;
  }

  int myCount = mpExperiment->count();

  QStringList algs;
  algs.clear();

  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      continue;
    }

    QString myName = mypModel->algorithm().name();

  if ( algs.lastIndexOf(myName) != -1 )
  {
    continue;
  }

  algs.push_back(myName);

    // Add details to the new row
    QListWidgetItem *mypNameItem = new QListWidgetItem(myName);
    //mypNameItem->setData(Qt::UserRole,mypModel->algorithm().guid());
    mypNameItem->setData(Qt::UserRole,myName);

    OmgAlgorithm::Origin myOrigin = mypModel->algorithm().origin();
    QIcon myIcon;
    if (myOrigin==OmgAlgorithm::USERPROFILE)
    {
      myIcon.addFile(":/algorithmTypeUser.png");
    }
    else if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
    {
      myIcon.addFile(":/algorithmTypeSystem.png");
    }
    else if (myOrigin==OmgAlgorithm::UNDEFINED)
    {
      mypNameItem->setTextColor(Qt::black);
    }
    mypNameItem->setIcon(myIcon);
    omglistAlgorithms->addItem(mypNameItem, true);
  }
}

void OmgModelTests::setExperimentSpeciesList()
{
  experimentSpeciesList.clear();

  if (!mpExperiment)
  {
    return ;
  }

  int myCount = mpExperiment->count();

  for (int i=0; i<myCount; i++)
  {
    OmgModel * mypModel=mpExperiment->getModel(i);
    if (mypModel==NULL)
    {
      continue;
    }

    QString myName = mypModel->taxonName();
    if ( experimentSpeciesList.lastIndexOf(myName) != -1 )
    {
      continue;
    }
    experimentSpeciesList.push_back(myName);

    // Add details to the new row
    /*QListWidgetItem *mypNameItem = new QListWidgetItem(myName);
    omglistTaxa->addItem(mypNameItem, true);*/
  }
}

void OmgModelTests::on_toolModelSettings_clicked()
{
  myModelTestOptions.exec();
}

QString OmgModelTests::getTestName()
{
  return myTestName;
}

void OmgModelTests::updateProgress (int theValue, int theMaximumValue)
{
  progressBar->setMaximum(theMaximumValue);
  progressBar->setValue(theValue);
  QApplication::processEvents();
}

bool OmgModelTests::checks()
{
  // No Label
  if( leTestName->text().isEmpty() )
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please give this test a valid name!"));
    return false;
  }

  if( omglistTaxa->checkedItemCount() < 1 )
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select at least one species!"));
    return false;
  }

  if( omglistAlgorithms->checkedItemCount() < 1 )
  {
    QMessageBox::warning( this,tr("openModeller Desktop"),
        tr("Please select at least one algorithm!"));
    return false;
  }

  return true;
}

void OmgModelTests::clear()
{
  leTestName->clear();
}

