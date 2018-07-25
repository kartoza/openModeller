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
#include "omgtermsandconditions.h"
//qt includes
#include <QSettings>
#include <QTextEdit>
#include <QTextStream>
#include <QFile>
#include <QPushButton>
#include <QString>
#include <QMessageBox>
OmgTermsAndConditions::OmgTermsAndConditions(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  //NOTE: make sure these lines stay after myApp init above
  QFile myQFile( ":/termsandconditions.html" );
  if ( myQFile.open( QIODevice::ReadOnly ) ) 
  {
    //now we parse the loc file, checking each line for its taxon
    QTextStream myStream( &myQFile );
    teConditions->setHtml(myStream.readAll());
    myQFile.close();
  }
  else
  {

  }
  QSettings mySettings;
}

OmgTermsAndConditions::~OmgTermsAndConditions()
{}


void OmgTermsAndConditions::reject()
{
  checkAgreement();
}


void OmgTermsAndConditions::accept()
{
  checkAgreement();
}

void OmgTermsAndConditions::checkAgreement()
{
  QSettings mySettings;
  if (chkAcceptTerms->checkState()==Qt::CheckState(2))
  {
    qDebug("Agreed to license agreement");
    mySettings.setValue("licensing/termsAgreedFlag",1);
    done(QDialog::Accepted); 
  }
  else if (chkAcceptTerms->checkState()==Qt::CheckState(0))
  {
    qDebug("Rejected license agreement");
    mySettings.setValue("licensing/termsAgreedFlag",0);
    QMessageBox::warning(this, tr("openModeller GUI Licensing"),
       tr("You must agree to the useage terms before using this application!\n"));
    done(QDialog::Rejected); 
  }
  else
  {
    qDebug ("Cockup with license agreement!");
  }
}
