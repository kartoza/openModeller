/***************************************************************************
 *   Copyright (C) 2007 by Tim Sutton   *
 *   peter@pembshideaway.co.uk tim@linfiniti.com   *
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

#ifndef OMGSURVEYWIZARD_H
#define OMGSURVEYWIZARD_H

#include <QWizard>
#include <QLabel>
#include <QPixmap>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
class OmgSurveyIntroPage : public QWizardPage
{
  Q_OBJECT;
public:
  OmgSurveyIntroPage(QWidget *parent)
    : QWizardPage(parent)
    {
      setTitle(tr("Introduction"));
      //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

      QLabel * label = new QLabel(tr("Congratulations for installing openModellerDesktop! "
            "Would you mind participating in our user survey? It will just "
            "take a few minutes and is completely optional. \n\n"
            "The data we gather "
            "from this survey will help us understand the needs of our users. "
            "We will also use the results of the survey as to help secure and "
            "justify funding for the openModeller project.\n\n"
            "No personal information will be shared beyond the project "
            "and your contact details (if you choose to supply them) will only "
            "be used to contact you with low frequency openModeller related "
            "announcements."
            ));
      label->setWordWrap(true);

      QVBoxLayout *layout = new QVBoxLayout;
      layout->addWidget(label);
      setLayout(layout);
    };
};
class OmgWizardPersonalInfoPage : public QWizardPage
{
  public:
  OmgWizardPersonalInfoPage(QWidget *parent) : QWizardPage(parent)
    {
      setTitle(tr("About yourself..."));
      setSubTitle(tr("We are interested to know who you are and where you work"));
      //setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo1.png"));

      QLabel * mypPersonNameLabel = new QLabel(tr("&Your name:"));
      QLineEdit * mypPersonNameLineEdit = new QLineEdit;
      mypPersonNameLabel->setBuddy(mypPersonNameLineEdit);

      QLabel * mypPersonOrganisationLabel = new QLabel(tr("Your organisation:"));
      QLineEdit * mypPersonOrganisationLineEdit = new QLineEdit;
      mypPersonOrganisationLabel->setBuddy(mypPersonOrganisationLineEdit);

      QLabel * mypPersonEmailLabel = new QLabel(tr("Your email address (optional):"));
      QLineEdit * mypPersonEmailLineEdit = new QLineEdit;
      mypPersonEmailLabel->setBuddy(mypPersonEmailLineEdit);

      //QCheckBox * qobjectMacroCheckBox = new QCheckBox(tr(""));

      QGroupBox * groupBox = new QGroupBox(tr("Job Description"));

      QRadioButton * mypAcademicRadioButton = new QRadioButton(tr("Academic staff / researcher"));
      QRadioButton * mypPhdStudentRadioButton = new QRadioButton(tr("Student - phd"));
      QRadioButton * mypMastersStudentRadioButton = new QRadioButton(tr("Student - masters"));
      QRadioButton * mypUndergradStudentRadioButton = new QRadioButton(tr("Student - undergraduate"));
      QRadioButton * mypDeveloperRadioButton = new QRadioButton(tr("Developer"));
      QRadioButton * mypGISPractitionerRadioButton = new QRadioButton(tr("GIS practitioner"));
      QRadioButton * mypOtherProfessionRadioButton = new QRadioButton(tr("Other profession:"));
      QLineEdit * mypOtherProfessionLineEdit = new QLineEdit;
      //defaultCtorRadioButton->setChecked(true);

      connect(mypOtherProfessionRadioButton, SIGNAL(toggled(bool)),
          mypOtherProfessionLineEdit, SLOT(setEnabled(bool)));

      registerField("mypPersonName*", mypPersonNameLineEdit);
      registerField("mypPersonOrganisation", mypPersonOrganisationLineEdit);
      registerField("mypPersonEmail", mypPersonEmailLineEdit);
      registerField("mypAcademicRadioButton", mypAcademicRadioButton);
      registerField("mypPhdStudentRadioButton", mypPhdStudentRadioButton);
      registerField("mypMastersStudentRadioButton", mypMastersStudentRadioButton);
      registerField("mypUndergradStudentRadioButton", mypUndergradStudentRadioButton);
      registerField("mypDeveloperRadioButton", mypDeveloperRadioButton);
      registerField("mypGISPractitionerRadioButton", mypGISPractitionerRadioButton);
      registerField("mypOtherProfessionRadioButton", mypOtherProfessionRadioButton);
      registerField("mypOtherProfessionLineEdit",mypOtherProfessionLineEdit);

      QVBoxLayout *groupBoxLayout = new QVBoxLayout;
      groupBoxLayout->addWidget(mypAcademicRadioButton);
      groupBoxLayout->addWidget(mypPhdStudentRadioButton);
      groupBoxLayout->addWidget(mypMastersStudentRadioButton);
      groupBoxLayout->addWidget(mypUndergradStudentRadioButton);
      groupBoxLayout->addWidget(mypDeveloperRadioButton);
      groupBoxLayout->addWidget(mypGISPractitionerRadioButton);
      groupBoxLayout->addWidget(mypAcademicRadioButton);
      groupBoxLayout->addWidget(mypOtherProfessionRadioButton);
      groupBoxLayout->addWidget(mypOtherProfessionLineEdit);
      groupBox->setLayout(groupBoxLayout);

      QGridLayout *layout = new QGridLayout;
      layout->addWidget(mypPersonNameLabel, 0, 0);
      layout->addWidget(mypPersonNameLineEdit, 0, 1);
      layout->addWidget(mypPersonOrganisationLabel, 1, 0);
      layout->addWidget(mypPersonOrganisationLineEdit, 1, 1);
      layout->addWidget(mypPersonEmailLabel, 2, 0);
      layout->addWidget(mypPersonEmailLineEdit, 2, 1);
      layout->addWidget(groupBox, 3, 0, 1, 2);
      setLayout(layout);
    }
};
/** \ingroup gui
 * \brief A survey form to gather information from our users
 */
class OmgSurveyWizard : public QWizard
{
  Q_OBJECT;

  public:
  OmgSurveyWizard(QWidget* parent = 0, Qt::WFlags fl = 0 )
  {
    addPage(new OmgSurveyIntroPage(this));
    addPage(new OmgWizardPersonalInfoPage(this));
    setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
    setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));

    setWindowTitle(tr("openModeller Desktop User Survey"));
  }
  ~OmgSurveyWizard(){};
  /*$PUBLIC_FUNCTIONS$*/

};


#endif //omgsurveywizaard_h


