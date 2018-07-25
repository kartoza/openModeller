/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton*
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
#include <omgui.h>
#include "omgalgorithmmanager.h"
#include <omgalgorithmset.h>
#include <omgalgorithmparameter.h>
//qt includes
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
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
#include <QMessageBox>

OmgAlgorithmManager::OmgAlgorithmManager(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  connect(tblAlgorithms, SIGNAL(cellClicked( int,int)), this, SLOT(cellClicked( int,int)));


  //Set up dynamic parameters gui
  mpLayout2 = new QVBoxLayout();
  frameParameters->setLayout(mpLayout2);
  mpScrollArea3 = new QScrollArea();
  mpLayout2->addWidget(mpScrollArea3);
  mpQFrame4  = new QFrame();
  mpScrollArea3->setWidget(mpQFrame4);
  mpScrollArea3->setWidgetResizable( true );
  mpLayout5 = new QGridLayout(mpQFrame4);

  
  //qDebug(myAlgorithmSet.toXml().toLocal8Bit());
  refreshAlgorithmTable();
}

OmgAlgorithmManager::~OmgAlgorithmManager()
{
  //save only user profiles
  //mAlgorithmSet.saveAlgorithms(Omgui::userAlgorithmProfilesDirPath(),true);
}

void OmgAlgorithmManager::resizeEvent ( QResizeEvent * theEvent )
{
  tblAlgorithms->setColumnWidth(1,tblAlgorithms->width());
}

void OmgAlgorithmManager::refreshAlgorithmTable(QString theGuid)
{
  mAlgorithmSet.clear();
  mAlgorithmSet = OmgAlgorithmSet::getFromActivePlugin();
  mAlgorithmSet.loadAlgorithms(Omgui::userAlgorithmProfilesDirPath());
  //iterate through the algorithm sets adding a row for each to the table
  tblAlgorithms->clear();
  tblAlgorithms->setRowCount(0);
  tblAlgorithms->setColumnCount(2);
  int mySelectedRow=0;
  if (mAlgorithmSet.count() < 1)
  {
    return;
  }

  for (int i=0; i < mAlgorithmSet.count(); i++)
  {
    tblAlgorithms->insertRow(i);
    OmgAlgorithm myAlgorithm = mAlgorithmSet[i];
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
    //display the text in red if the origin of the alg is from the adapter
    //rather than the users xml alg profile store
    OmgAlgorithm::Origin myOrigin = myAlgorithm.origin();
    if (myOrigin==OmgAlgorithm::USERPROFILE)
    {
      QIcon myIcon;
      myIcon.addFile(":/algorithmTypeUser.png");
      mypNameItem->setIcon(myIcon);
    }
    else if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
    {
      QIcon myIcon;
      myIcon.addFile(":/algorithmTypeSystem.png");
      mypNameItem->setIcon(myIcon);
    }
    else if (myOrigin==OmgAlgorithm::UNDEFINED)
    {
      mypNameItem->setTextColor(Qt::yellow);
    }
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

void OmgAlgorithmManager::cellClicked(int theRow, int theColumn)
{
  //note we use the alg name not the id becuase user may have customised params
  QString myGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  selectAlgorithm(myGuid);
}
void OmgAlgorithmManager::selectAlgorithm(QString theGuid)
{
  OmgAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm(theGuid);
  OmgAlgorithm::Origin myOrigin = myAlgorithm.origin();
  bool myEnabledFlag;
  bool myCloneEnabledFlag=true;
  bool myDefaultsButtonEnabledFlag=true;
  //system profiles are not changeable
  if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
  {
    myEnabledFlag=false;
    myDefaultsButtonEnabledFlag=false;
    lblProfileNotes->setText(tr("Non-editable system profile."));
    lblProfileIcon->setPixmap(QPixmap(":/algorithmTypeSystem.png"));
  }
  else
  {
    myEnabledFlag=true;
    lblProfileNotes->setText(tr("Editable user profile."));
    lblProfileIcon->setPixmap(QPixmap(":/algorithmTypeUser.png"));
  }
  if (myAlgorithm.parameterCount()<1)
  {
    //dont allow cloning of algs with no parameters
    myCloneEnabledFlag=false;
    //dont show defaults button for algs with no params
    myDefaultsButtonEnabledFlag=false;
  }
  pbnApply->setEnabled(myEnabledFlag);
  pbnDelete->setEnabled(myEnabledFlag);
  pbnClone->setEnabled(myCloneEnabledFlag);
  pbnDefaults->setEnabled(myDefaultsButtonEnabledFlag);
  leProfileName->setText(myAlgorithm.name());
  leProfileName->setEnabled(myEnabledFlag);
  lblAlgorithm->setText(tr("Algorithm: ") + myAlgorithm.id());
  lblVersion->setText(myAlgorithm.version());
  lblAuthor->setText(myAlgorithm.author());
  lblCodeAuthor->setText(myAlgorithm.codeAuthor());

  teOverview->clear();
  teOverview->append(myAlgorithm.overview());
  teDescription->clear();
  teDescription->append(myAlgorithm.description());
  teBibliography->clear();
  teBibliography->append(myAlgorithm.bibliography());
  setParametersGui(myAlgorithm);
  lblParameterCount->setText(tr("Parameter Count: ") + QString::number(myAlgorithm.parameterCount()));
}
void OmgAlgorithmManager::on_pbnClone_clicked()
{
  if (tblAlgorithms->currentRow() < 0) 
  {
    return;
  }
  //to clone, we get the algorithm that is currently selected
  //copy it and serialise it off to disk
  QString myGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  if (myGuid.isEmpty())
  {
    return;
  }
  OmgAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm(myGuid);
  QString myProfileName = myAlgorithm.name();
  //get a unique userfriendly name for this alg
  int myCount = 1;
  while (mAlgorithmSet.contains(myProfileName))
  {
    myProfileName = tr("Copy ") + QString::number(myCount++) + " of " + myAlgorithm.name();
  }
  myAlgorithm.setName(myProfileName);
  //assign this alg its own guid
  myAlgorithm.setGuid();
  //indicate that this is a user alg definition
  myAlgorithm.setOrigin(OmgAlgorithm::USERPROFILE);
  saveAlgorithm(myAlgorithm);
  mAlgorithmSet.addAlgorithm(myAlgorithm);
  refreshAlgorithmTable(myAlgorithm.guid());
}
void OmgAlgorithmManager::on_pbnDelete_clicked()
{
  if (tblAlgorithms->currentRow() < 0) 
  {
    return;
  }
  
  if ( QMessageBox::warning(this, "openModeller Desktop",
                    tr("Deleting a user algorithm profile is non-reversable.\n"
                       "Do you really want to delete it?"),
                       QMessageBox::Yes | QMessageBox::No,
                       QMessageBox::No) == QMessageBox::No)
  {
    //user said no
    return;
  }

  
  QString myGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  if (!myGuid.isEmpty())
  {
    QFile myFile(Omgui::userAlgorithmProfilesDirPath() + QDir::separator() + myGuid + ".xml");
    if (!myFile.remove())
    {
      QMessageBox::warning(this, tr("openModeller Desktop"),
      tr("Unable to delete file \n") + myFile.fileName());
      //return focus to the undeleted alg
      refreshAlgorithmTable(myGuid);
    }
    else
    {
      //move focus to default alg
      refreshAlgorithmTable();
    }
  }
}
void OmgAlgorithmManager::on_pbnApply_clicked()
{
  if (tblAlgorithms->currentRow() < 0) 
  {
    return;
  }
  // Get the parameters for the selected alg
  QString myGuid = tblAlgorithms->item(tblAlgorithms->currentRow(),0)->text();
  OmgAlgorithm myAlgorithm = mAlgorithmSet.getAlgorithm(myGuid);
  //check the name is unique if its been changed
  if (0!=myAlgorithm.name().compare(leProfileName->text()))
  {
    if (mAlgorithmSet.contains(leProfileName->text()))
    {
      QMessageBox::warning( this,tr("openModeller Desktop"),tr("The name you choose for your algorithm profile must be unique.\n Update it and try again."));
      return;
    }
    //ok user has changed the name and ther is no other alg using this name
    //so we can go ahead and update it
    myAlgorithm.setName(leProfileName->text());
  }
  QVector<OmgAlgorithmParameter> myUpdatedParameters;
  //Get the algorithm parameters and store in QStringList
  ParametersMap::Iterator myIterator;
  for ( myIterator = mParametersMap.begin(); myIterator != mParametersMap.end(); ++myIterator )
  {
    QString myId = myIterator.key();
    QString myValue= "";
    QString myWidgetName = myIterator.value()->objectName();
    if (myWidgetName.left(2)=="le")
    {
      QLineEdit * myLineEdit = (QLineEdit*) myIterator.value();
      myValue = myLineEdit->text();
    }
    else if (myWidgetName.left(4)=="spin")
    {
      QSpinBox * mySpinBox = (QSpinBox*) myIterator.value();
      myValue = QString::number(mySpinBox->value());
    }
    else if (myWidgetName.left(7)=="dblspin")
    {
      QDoubleSpinBox * mySpinBox = (QDoubleSpinBox*) myIterator.value();
      myValue = QString::number(mySpinBox->value());
    }

    //now store our modified parameter
    //to do that we just get the parameter by its id from the alg
    //and then overwrite its value (users can only change the value
    //so everything else must be the same)
    //then we write the modified parameter back into the alg.
    OmgAlgorithmParameter myParameter = myAlgorithm.parameter(myId);
    if (myParameter.id()!="invalid")
    {
      myParameter.setValue(myValue);
      myUpdatedParameters.push_back(myParameter);
    }
  }
  myAlgorithm.setParameters(myUpdatedParameters);
  //qDebug(myAlgorithm.toXml().toLocal8Bit());
  //true = overwrite existing
  mAlgorithmSet.addAlgorithm(myAlgorithm,true);
  saveAlgorithm(myAlgorithm);
  refreshAlgorithmTable(myGuid);
}

void OmgAlgorithmManager::setParametersGui(OmgAlgorithm &theAlgorithm)
{
  OmgAlgorithm::Origin myOrigin = theAlgorithm.origin();
  bool myEnabledFlag;
  //system profiles are not changeable
  if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
  {
    myEnabledFlag=false;
  }
  else
  {
    myEnabledFlag=true;
  }
  //delete current parameter map contents
  //the parameters map contains widgets so we must delete each widget first
  //then clear the map itself
  ParametersMap::Iterator myParametersIterator;
  for ( myParametersIterator = mParametersMap.begin(); myParametersIterator != mParametersMap.end(); ++myParametersIterator )
  {
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
    QLabel *myLabel = new QLabel(myParameter.name() + " [" +
		    myParameter.minimum() +
		    "," +
		    myParameter.maximum() + 
		    "]");

    if(myParameter.type()=="Integer")
    {
      //qDebug("Parameter is an integer");
      QString myControlName = QString("spin"+QString(myParameter.id()));
      QSpinBox * mySpinBox = new QSpinBox();
      //system profiles are non editable
      mySpinBox->setEnabled(myEnabledFlag);
      mySpinBox->setObjectName(myControlName);
      mySpinBox->setRange((myParameter.minimum().toInt()), (myParameter.maximum().toInt()));
      if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
      {
        mySpinBox->setValue(myParameter.defaultValue().toInt());
      }
      else
      {
        mySpinBox->setValue(myParameter.value().toInt());
      }
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
      //qDebug("Parameter is a double");
      QString myControlName = QString("dblspin"+QString(myParameter.id()));
      QDoubleSpinBox * mySpinBox = new QDoubleSpinBox();
      //system profiles are non editable
      mySpinBox->setEnabled(myEnabledFlag);
      mySpinBox->setObjectName(myControlName);
      mySpinBox->setRange((myParameter.minimum().toFloat()), (myParameter.maximum().toFloat()));
      mySpinBox->setDecimals(3);
      mySpinBox->setSingleStep(0.5);
      //doesnt apply to a line edit
      //myLineEdit->setRange((myParameter.minimum().toInt()), (myParameter.maximum().toInt()));
      if (myOrigin==OmgAlgorithm::ADAPTERPROFILE)
      {
        mySpinBox->setValue(myParameter.defaultValue().toFloat());
      }
      else
      {
        mySpinBox->setValue(myParameter.value().toFloat());
      }
      mySpinBox->setToolTip(myParameter.description());
      mpLayout5->addWidget(myLabel,myRow,0);
      mpLayout5->addWidget(mySpinBox,myRow,1);
      //
      // Add the widget to the map
      //
      mParametersMap[myParameter.id()] = mySpinBox;
      myRow++;
    }
    mDefaultParametersMap[myParameter.id()]=QString(myParameter.defaultValue());
    mLabelsMap[myParameter.id()] = myLabel;
    //
    // Now draw the description label
    //
    //The description label spans two cols...
    //void QGridLayout::addWidget ( QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )
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
    mpLayout5->addWidget(myDescriptionLabel,myRow,0,1,2); //1 row 2 cols spanned
    myHeight = ((myWidth / frameParameters->width())+1)*myHeight;
    myWidth = myDescriptionLabel->width() / 2;
    QSize mySize (myWidth,myHeight);
    myDescriptionLabel->setMinimumSize(mySize);

    mDescriptionLabelsMap[myParameter.id()] = myDescriptionLabel;
    ++myRow;
  }
}

void OmgAlgorithmManager::on_pbnDefaults_clicked()
{
  ParametersMap::Iterator myIterator;
  for ( myIterator = mParametersMap.begin(); myIterator != mParametersMap.end(); ++myIterator )
  {
    QString myId = myIterator.key();
    QString myValue= "";
    QString myWidgetName = myIterator.value()->objectName();
    if (myWidgetName.left(2)=="le")
    {
      QLineEdit * myLineEdit = (QLineEdit*) myIterator.value();
      myLineEdit->setText(mDefaultParametersMap[myId]);
    }
    else if (myWidgetName.left(4)=="spin")
    {
      QSpinBox * mySpinBox = (QSpinBox*) myIterator.value();
      mySpinBox->setValue(mDefaultParametersMap[myId].toInt());
    }
    else if (myWidgetName.left(7)=="dblspin")
    {
      QDoubleSpinBox * myDoubleSpinBox = (QDoubleSpinBox*) myIterator.value();
      myDoubleSpinBox->setValue(mDefaultParametersMap[myId].toInt());
    }
  }
}

bool OmgAlgorithmManager::saveAlgorithm(OmgAlgorithm &theAlgorithm)
{
    QString myString = theAlgorithm.toXml();
    //use a globally unique identifier for the file name
    QFile myFile(Omgui::userAlgorithmProfilesDirPath() + QDir::separator() + theAlgorithm.guid() + ".xml");
    if (!myFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      return false;
    }
    QTextStream myStream(&myFile);
    myStream << myString;
    myFile.close();
    return true;
}
