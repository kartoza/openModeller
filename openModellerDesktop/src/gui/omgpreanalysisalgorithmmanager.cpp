/***************************************************************************
 *   Copyright (C) 2009 by Pedro Cavalcante                                *
 *   pedro.cavalcante@poli.usp.br                                          *
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
#include "omgpreanalysisalgorithmmanager.h"
#include "omgui.h"
#include <omgmodellerpluginregistry.h>
#include <omgscraperpluginregistry.h>
#include <qgsproviderregistry.h>
#include <omgui.h>  // for version
#include <omglayerset.h>
//qt includes
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QListIterator>
#include <QListWidget>
#include <QMessageBox>
#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTextStream>
#include <QToolTip>
#include <QToolButton>
#include <QMessageBox>

#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/pre/PreAlgorithm.hh>


OmgPreAnalysisAlgorithmManager::OmgPreAnalysisAlgorithmManager(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  connect(tblAlgorithms, SIGNAL(cellClicked( int,int)), this, SLOT(cellClicked( int,int)));
  QList<int> mySizes;
  mySizes << 200 << this->width() - 200;
  splitter->setSizes( mySizes );
  //Set up dynamic parameters gui
  mpScrollArea3 = new QScrollArea( this );
  parametersLayout->addWidget( mpScrollArea3 );
  mpQFrame4  = new QFrame();
  mpQFrame4->setGeometry( frameParameters->geometry() );
  mpScrollArea3->setWidget(mpQFrame4);
  mpScrollArea3->setWidgetResizable( true );
  mpLayout5 = new QGridLayout(mpQFrame4);
  mOccurrencesFile = "";
  QIcon myIcon;
  myIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                                           QIcon::Normal, QIcon::Off);
  mpOccurrenceSelector.setIcon(myIcon);
  mpSpecies.setObjectName("cboSpecies");
  cboModelLayerSet.setObjectName("cboModelLayerSet");
  connect(&mpOccurrenceSelector, SIGNAL(clicked()), this, SLOT(occurrenceSelectorClicked()));
  // until we don't have a dynamic way to access the pre-analysis algorithms, having a separate method
  // to refresh the Algorithms list maybe seem unnecessary but will be useful on the future
  Omgui::LayerSetMap myLayerSetMap = Omgui::getAvailableLayerSets();
  QMapIterator<QString, OmgLayerSet> myIterator(myLayerSetMap);
  while (myIterator.hasNext()) 
  {
    myIterator.next();
    OmgLayerSet myLayerSet = myIterator.value();
    QString myGuid = myLayerSet.guid();
    QString myName = myLayerSet.name();
    //display an icon indicating if the layerset is local or remote (e.g. terralib)
    QIcon myIcon;
    myIcon.addFile(":/localdata.png");
    cboModelLayerSet.addItem(myIcon,myName,myGuid);

  }
  refreshAlgorithmTable();
}

OmgPreAnalysisAlgorithmManager::~OmgPreAnalysisAlgorithmManager()
{}

void OmgPreAnalysisAlgorithmManager::refreshAlgorithmTable(QString theGuid)
{
  mAlgorithmSet.clear();
  mAlgorithmSet = OmgPreAnalysisAlgorithmSet::getFromActivePlugin();
  mAlgorithmSet.loadAlgorithms(Omgui::userAlgorithmProfilesDirPath());
  //iterate through the algorithm sets adding a row for each to the table
  tblAlgorithms->clear();
  tblAlgorithms->setRowCount(0);
  tblAlgorithms->setColumnCount(2);
  QIcon myIcon;
  myIcon.addFile(":/algorithmTypeSystem.png");
  int mySelectedRow=0;
  if (mAlgorithmSet.count() < 1)
  {
    return;
  }

  for (int i=0; i < mAlgorithmSet.count(); i++)
  {
    tblAlgorithms->insertRow(i);
    OmgPreAnalysisAlgorithm myAlgorithm = mAlgorithmSet[i];
    if (theGuid.isEmpty())
    {
      theGuid=myAlgorithm.guid();
    }
    if (myAlgorithm.guid()==theGuid)
    {
      mySelectedRow=i;
    }

    // Add details to the new row
    QTableWidgetItem *mypIdItem = new QTableWidgetItem(myAlgorithm.guid());
    tblAlgorithms->setItem(tblAlgorithms->rowCount()-1, 0, mypIdItem);
    QTableWidgetItem *mypNameItem = new QTableWidgetItem(myAlgorithm.name());
    mypNameItem->setIcon(myIcon);
    tblAlgorithms->setItem(tblAlgorithms->rowCount()-1, 1, mypNameItem);
  }

  tblAlgorithms->setCurrentCell(mySelectedRow,1);
  QStringList headerLabels;
  headerLabels << "Name" <<  "Description" ;
  tblAlgorithms->setHorizontalHeaderLabels(headerLabels);
  tblAlgorithms->setColumnWidth(0,0);
  tblAlgorithms->setColumnWidth(1,tblAlgorithms->width());
  tblAlgorithms->horizontalHeader()->hide();
  tblAlgorithms->verticalHeader()->hide();
  tblAlgorithms->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  selectAlgorithm(theGuid);
}

void OmgPreAnalysisAlgorithmManager::selectAlgorithm(QString theGuid)
{
  OmgPreAnalysisAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm(theGuid);
  bool myEnabledFlag=true;
  bool myCloneEnabledFlag=false;
  bool myDefaultsButtonEnabledFlag=false;
  pbnRun->setEnabled(myEnabledFlag);
  lePreAnalysisName->setText(myAlgorithm.name());
  lePreAnalysisName->setEnabled(myEnabledFlag);
  teOverview->clear();
  teOverview->append(myAlgorithm.overview());
  setParametersGui(myAlgorithm);
}

void OmgPreAnalysisAlgorithmManager::setParametersGui(OmgPreAnalysisAlgorithm &theAlgorithm)
{
  bool myEnabledFlag = true;

  //delete current parameter map contents
  //the parameters map contains widgets so we must delete each widget first
  //then clear the map itself
  ParametersMap::Iterator myParametersIterator;
  for ( myParametersIterator = mParametersMap.begin(); myParametersIterator != mParametersMap.end(); ++myParametersIterator )
  {
    if(myParametersIterator.value() != &cboModelLayerSet && myParametersIterator.value() != &mpSpecies)
    	delete myParametersIterator.value();
    //qDebug("Deleted widget " + myParametersIterator.key().toLocal8Bit() + " from parameters map");
  }
  mParametersMap.clear();
  //the default parameters map simply contains strings so we can just clear it
  mDefaultParametersMap.clear();
  //the parameter labels map once again contains widgets so we need to delete
  //them first
  ParameterLabels::Iterator myLabelIterator;
  for ( myLabelIterator = mLabelsMap.begin(); myLabelIterator != mLabelsMap.end(); ++myLabelIterator )
  {
    delete myLabelIterator.value();
  }
  mLabelsMap.clear();
  //the description labels map once again contains widgets so we need to delete
  //them too
  DescriptionLabels::Iterator myDescriptionLabelIterator;
  for ( myDescriptionLabelIterator = mDescriptionLabelsMap.begin(); 
      myDescriptionLabelIterator != mDescriptionLabelsMap.end(); 
      ++myDescriptionLabelIterator )
  {
    delete myDescriptionLabelIterator.value();
  }
  mDescriptionLabelsMap.clear();

  // clear and reinitialise the paramters layout
  if (mpLayout5)
  {
    delete mpLayout5;
  }
  //GridLayout within the LayoutWidget
  mpLayout5 = new QGridLayout(mpQFrame4); //, myRowCountInt+1,3);
  // Set Parameter GUI
  QVector<OmgAlgorithmParameter> myAlgorithmParameters = theAlgorithm.parameters();
  QVectorIterator<OmgAlgorithmParameter> myIterator(myAlgorithmParameters);
  int myRow=0;
  while (myIterator.hasNext())
  {
    OmgAlgorithmParameter myParameter = myIterator.next();
    QLabel *myLabel;
    if(myParameter.minimum() != myParameter.maximum())
	myLabel = new QLabel(myParameter.name() + " [" +
		    myParameter.minimum() +
		    "," +
		    myParameter.maximum() + 
		    "]");
    else
	myLabel = new QLabel(myParameter.name());
    myLabel->setMaximumWidth(120);
    if(myParameter.type()=="Integer")
    {
      QString myControlName = QString("spin"+QString(myParameter.id()));
      QSpinBox * mySpinBox = new QSpinBox();
      //system profiles are non editable
      mySpinBox->setEnabled(myEnabledFlag);
      mySpinBox->setObjectName(myControlName);
      mySpinBox->setRange((myParameter.minimum().toInt()), (myParameter.maximum().toInt()));
      mySpinBox->setValue(myParameter.value().toInt());
      mySpinBox->setToolTip(myParameter.description());
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(mySpinBox,myRow,1);
      //
      // Add the widget to the map
      //
      mParametersMap[myParameter.id()] = mySpinBox;
      myRow++;
    }
    else if ((myParameter.type()=="Real") || (myParameter.type()=="Double"))
    {
      QString myControlName = QString("dblspin"+QString(myParameter.id()));
      QDoubleSpinBox * mySpinBox = new QDoubleSpinBox();
      //system profiles are non editable
      mySpinBox->setEnabled(myEnabledFlag);
      mySpinBox->setObjectName(myControlName);
      if(myParameter.minimum() != myParameter.maximum())
        mySpinBox->setRange((myParameter.minimum().toFloat()), (myParameter.maximum().toFloat()));
      mySpinBox->setDecimals(3);
      mySpinBox->setSingleStep(0.05);
      //doesnt apply to a line edit
      //myLineEdit->setRange((myParameter.minimum().toInt()), (myParameter.maximum().toInt()));
      mySpinBox->setValue(myParameter.value().toFloat());
      mySpinBox->setToolTip(myParameter.description());
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(mySpinBox,myRow,1);
      //
      // Add the widget to the map
      //
      mParametersMap[myParameter.id()] = mySpinBox;
      myRow++;
    }
    else if (myParameter.type()=="SamplerPtr")
    {
      QSettings mySettings;
      delete myLabel;
      myLabel = new QLabel("Occurrence data: ");
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(&mpSpecies,myRow,1);
      mpLayout5->addWidget(&mpOccurrenceSelector,myRow,2);
      mLabelsMap[myParameter.id() + "Od"] = myLabel;
      mParametersMap[myParameter.id() + "Od"] = &mpSpecies;
      myRow++;
      myLabel = new QLabel("Layerset: ");
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(&cboModelLayerSet,myRow,1);
      mParametersMap[myParameter.id()] = &cboModelLayerSet;
      myRow++;
    }
    else if (myParameter.type()=="AlgorithmPtr")
    {
      // qDebug("Parameter is a AlgorithmPtr");
      QString myControlName = QString("cbo"+QString(myParameter.id()));
      QComboBox * myComboBox = new QComboBox();
      myComboBox->setObjectName(myControlName);
      OmgAlgorithmSet algorithmSet = OmgAlgorithmSet::getFromActivePlugin();
      for(int i = 0; i < algorithmSet.count(); i++)
      {
        QString name = algorithmSet[i].name();
        QString id = algorithmSet[i].id();
        QVariant qVariant(id);
        qDebug("---> (FOUND) adding '%s':'%s' to AlgorithmCombo", name.toStdString().c_str(), id.toStdString().c_str());
        myComboBox->addItem(name, qVariant);
      }
      // myComboBox->addItems(algorithmSet.nameList());
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(myComboBox,myRow,1);
      //
      // Add the widget to the map
      //
      mParametersMap[myParameter.id()] = myComboBox;
      myRow++;
    }
    mDefaultParametersMap[myParameter.id()]=QString(myParameter.defaultValue());
    mLabelsMap[myParameter.id()] = myLabel;
    //
    // Now draw the description label
    //
    //The description label spans two cols...
    //void QGridLayout::addWidget ( QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )

    // it is not desired to show "Sampler parameters" if it's the only parameter
    if(!(myAlgorithmParameters.size() == 1 && myAlgorithmParameters[0].type() == "SamplerPtr"))
    {
      QLabel * myDescriptionLabel = new QLabel (
          myParameter.description());
      myDescriptionLabel->setWordWrap(true);
      //QSizePolicy myPolicy =  myDescriptionLabel->sizePolicy();
      //makes labels far too big but with no clipping
      //myPolicy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
      //myDescriptionLabel->setSizePolicy(myPolicy);
      QFontMetrics myMetrics(myDescriptionLabel->font());
      int myWidth = myMetrics.width(myDescriptionLabel->text());
      int myHeight = myMetrics.height();
      mpLayout5->addWidget(myDescriptionLabel,myRow,0,1,3); //1 row 2 cols spanned
      myHeight = ((myWidth / frameParameters->width())+1)*myHeight;
      myWidth = myDescriptionLabel->width() / 2;
      QSize mySize (myWidth,myHeight);
      myDescriptionLabel->setMinimumSize(mySize);

      mDescriptionLabelsMap[myParameter.id()] = myDescriptionLabel;
      ++myRow;
    }
  }
}


void OmgPreAnalysisAlgorithmManager::cellClicked(int theRow, int theColumn)
{
  QString myGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  selectAlgorithm(myGuid);
}

void OmgPreAnalysisAlgorithmManager::occurrenceSelectorClicked()
{
  QSettings mySettings;

  QString myFileTypeQString;

  mOccurrencesFile = QFileDialog::getOpenFileName(
        this,
        "Select localities text file" , //caption
        mySettings.value("openModeller/experimentDesigner/localitiesFileDirectory", "").toString(), //initial dir
        "Text File (*.txt)",  //filters to select
        &myFileTypeQString //the pointer to store selected filter
        );

  refreshSpecies();
}

void OmgPreAnalysisAlgorithmManager::refreshSpecies()
{
  mpSpecies.clear();
  if(mOccurrencesFile.isEmpty())
    return;
  mpSpecies.addItems(getSpeciesList());
}

QStringList OmgPreAnalysisAlgorithmManager::getSpeciesList()
{
  QStringList myTaxonList;
  QFile myQFile( mOccurrencesFile );
  if ( myQFile.open( QIODevice::ReadOnly ) )
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLine;
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
          if(!myTaxonList.contains(myTaxonName))
            myTaxonList.append(myTaxonName);
        }
      }
    }
    myQFile.close();
  }
  else
  {
    QMessageBox::warning( this,QString("openModeller OmgExperimentDesigner Error"),QString("The localities file is not readable. Check you have the neccessary file permissions and try again."));
  }
  return myTaxonList;
}

void OmgPreAnalysisAlgorithmManager::on_pbnRun_clicked()
{
  if(lePreAnalysisName->text().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"), tr("Please give this pre-analysis a valid name."));
    return;
  }

  if(mpSpecies.currentText().isEmpty())
  {
    QMessageBox::warning( this,tr("openModeller Desktop"), tr("Occurrence data must be specified."));
    return;
  }

  OmgPreAnalysis *mypPreAnalysis = getPreAnalysis();
  emit preAnalysisCreated(mypPreAnalysis);

  done(1);
}


OmgPreAnalysis * OmgPreAnalysisAlgorithmManager::getPreAnalysis()
{
  QSettings mySettings;
  emit logMessage("Creating pre-analysis");
  //set up the experiment
  OmgPreAnalysis * mypPreAnalysis = new OmgPreAnalysis();
  mypPreAnalysis->setName(lePreAnalysisName->text());
  QString theGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  OmgPreAnalysisAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm(theGuid);
  mypPreAnalysis->setPreAnalysisAlgorithm(myAlgorithm.algorithm());
  // qDebug("Algorithm name: " + myAlgorithm.factoryName());
  mypPreAnalysis->setAlgorithmName(myAlgorithm.factoryName());

  QVector<OmgAlgorithmParameter> myUpdatedParameters;

  ParametersMap::Iterator myIterator;
  /* Test if all the parameters are filled up */
  for (myIterator = mParametersMap.begin(); myIterator != mParametersMap.end(); ++myIterator)
  {
    QString myId = myIterator.key();
    // qDebug("Param name: " + myId);
    QString myValue= "";
    QString myWidgetName = myIterator.value()->objectName();
//    qDebug("---\nmyId: " + myId);
//    qDebug("myWidgetName: " + myWidgetName);
    if (myWidgetName.left(4)=="spin")
    {
//      qDebug("\tdetected: Integer");
      QSpinBox *spin = (QSpinBox*)(myIterator.value());
      double value = spin->value();
      mypPreAnalysis->setParameterValue(myId, value);
    }
    else if (myWidgetName.left(7)=="dblspin")
    {
//      qDebug("\tdetected: Real");
      QDoubleSpinBox *dblspin = (QDoubleSpinBox*)(myIterator.value());
      double value = dblspin->value();
      mypPreAnalysis->setParameterValue(myId, value);
    }
    else if (myWidgetName=="cboAlgorithm")
    {
//      qDebug("\tdetected: Algorithm");
      QComboBox *cbo = (QComboBox*)(myIterator.value());
      QString text = cbo->itemText(cbo->currentIndex());
      QString data = (cbo->itemData(cbo->currentIndex())).toString();
      AlgorithmPtr algPtr = AlgorithmFactory::newAlgorithm(data.toStdString());
      // qDebug("Filling with default values: ");
      // TODO: read profiles also saved in the GUI
      const AlgMetadata *algMetadata = algPtr->getMetadata();
      AlgParamMetadata *params = algMetadata->param;
      int n = algMetadata->nparam;
      AlgParameter *p = new AlgParameter[n];
      for(int i = 0; i < n; i++)
      {
        qDebug("=> (" + QString::number(i) + ") " + QString::fromStdString(params->name) + "["
               + QString::fromStdString(params->id) + "]" + ": "
               + QString::fromStdString(params->typical));
        p[i].setId(params->id);
        p[i].setValue(params->typical);
        params++;
      }
      // TODO: terminar isso aqui.
      algPtr->setParameters(n, p);
      mypPreAnalysis->setParameterValue(myId, algPtr);
    }
    else if (myWidgetName=="cboSpecies")
    {
//      qDebug("\tdetected: Species");
      QComboBox *cbo = (QComboBox*)(myIterator.value());
      QString value = cbo->currentText();
      mypPreAnalysis->setOccurrenceDataFile(mOccurrencesFile);
      mypPreAnalysis->setTaxonName(value);
    }
    else if (myWidgetName=="cboModelLayerSet")
    {
//      qDebug("\tdetected: Layer");
      QComboBox *cbo = (QComboBox*)(myIterator.value());
      QString value = cbo->currentText();
      mypPreAnalysis->setLayerSet(value);
    }
    else
    {
      qDebug("\tParameter not detected: " + myWidgetName);
    }
  }

  mypPreAnalysis->configureSamplePtr();


  mypPreAnalysis->setCompleted(false);
  return mypPreAnalysis;
}
