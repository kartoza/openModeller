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

#include <omgoptions.h>
#include <omgmodellerplugininterface.h>
#include <omgui.h> //provides helper functions for getting app paths etc
#include <omgmodellerpluginregistry.h>
//qt includes
#include <QDir>
#include <QColorDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QTabWidget>
#include <QTextCodec>
#include <QCheckBox>
#include <QHash>

#define DEFAULT_WS_URL "http://modeller.cria.org.br/cgi-bin/om_soap_server.cgi"

OmgOptions::OmgOptions(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  QSettings mySettings;

  // User data path
  leDataDirectory->setText(mySettings.value("dataDirs/dataDir",QDir::homePath() + QString("/.omgui/")).toString());

  //populate teh output image formats combo
  QHash<QString,QString> myHash = Omgui::getOutputFormats();
  QHashIterator<QString, QString> myIterator(myHash);
  QString myFormat = mySettings.value("outputFormat","GreyTiff100").toString();
  int myDefaultIndex=0;
  int myCurrentIndex=0;
  while (myIterator.hasNext()) 
  {
    myIterator.next();
    //We will add the hash item key to the Qt::UserRole 
    //for the combo list entry
    cboOutputFormat->addItem(myIterator.value(), myIterator.key());
    if (myIterator.key()==myFormat)
    {
      myDefaultIndex=myCurrentIndex;
    }
    ++myCurrentIndex;
  }
  cboOutputFormat->setCurrentIndex(myDefaultIndex);


  //Web proxy settings
  grpProxy->setChecked(mySettings.value("proxy/proxyEnabled", "0").toBool());
  leProxyHost->setText(mySettings.value("proxy/proxyHost", "").toString());
  leProxyPort->setText(mySettings.value("proxy/proxyPort", "").toString());
  leProxyUser->setText(mySettings.value("proxy/proxyUser", "").toString());
  leProxyPassword->setText(mySettings.value("proxy/proxyPassword", "").toString());

  //gbif url settings
  leGbifUrl->setText(mySettings.value("openModeller/gbifPlugin/url", "data.gbif.org").toString());
  //Plugin Type 
  lblPluginSearchPath->setText(Omgui::pluginDirPath());
  QString myWebServiceUrl = mySettings.value("openModeller/webServicesPlugin/url", DEFAULT_WS_URL).toString();
  //url could still be blank if key exists but user has blanked it, in this case provide a default url again
  if (myWebServiceUrl.isEmpty())
  {
    myWebServiceUrl = DEFAULT_WS_URL;
  }
  leWebServiceAddress->setText(myWebServiceUrl);
  leWebServiceAddress->setEnabled(true);
  QString myPluginName = mySettings.value("openModeller/modellerPluginType", "").toString();
  QStringList myList = OmgModellerPluginRegistry::instance()->names();
  cboModellerPluginName->addItems(myList);
  if (myList.contains(myPluginName))
  {
    cboModellerPluginName->setCurrentIndex(myList.indexOf(QRegExp(myPluginName)));
  }
  bool myWSLoggingFlag=mySettings.value("openModeller/webServicesPlugin/doLoggingFlag", false).toBool();
  cbxModellerLoggingFlag->setChecked(myWSLoggingFlag);
  bool myModellerVerboseFlag=mySettings.value("openModeller/modellerVerboseLogFlag", false).toBool();
  cbxModellerVerboseFlag->setChecked(myModellerVerboseFlag);

  //Database settings
  grpDatabase->setChecked(mySettings.value("database/databaseEnabled", "0").toBool());
  leDbHost->setText(mySettings.value("database/databaseHost", "").toString());
  leDbUser->setText(mySettings.value("database/databaseUser", "").toString());
  leDbPassword->setText(mySettings.value("database/databasePassword", "").toString());
  leDbName->setText(mySettings.value("database/databaseName", "").toString());
  leDbTable->setText(mySettings.value("database/databaseTable", "").toString());
  leDbTaxonField->setText(mySettings.value("database/taxonField", "").toString());
  leDbLatitudeField->setText(mySettings.value("database/latitudeField", "").toString());
  leDbLongitudeField->setText(mySettings.value("database/longitudeField", "").toString());

  // Tooltips and gui options
  bool myTipsFlag = mySettings.value("tipOfTheDay/tipOfTheDay", true).toBool();
  cbxShowTooltips->setChecked(myTipsFlag);
  bool myThreadingFlag = mySettings.value("openModeller/runExperimentInThread", true).toBool();
  cbxExperimentInThread->setChecked(myThreadingFlag);

  // Systray options
  bool myUseSystemTrayFlag = mySettings.value("openModeller/useSystemTray", false).toBool();
  grpSystrayIcon->setChecked(myUseSystemTrayFlag);
  bool myMinimiseToSystemTrayFlag = mySettings.value("openModeller/closeToSystemTray", false).toBool();
  cbxMinimizeToSystray->setChecked(myMinimiseToSystemTrayFlag);
  bool myShowSystemTrayMessagesFlag = mySettings.value("openModeller/showSystemTrayMessages", false).toBool();
  cbxShowSystrayMessages->setChecked(myShowSystemTrayMessagesFlag);
  
  
  bool myResetExtentsFlag= mySettings.value("openModeller/resetExtents", false).toBool();
  cbxResetExtents->setChecked(myResetExtentsFlag);
#ifdef OMG_NO_EXPERIMENTAL
  //hide the database tab if we are prepareing a release
  //as it doesnt do anything yet
  tabWidget->removeTab(5);
#endif

  //
  // Locale settings 
  //
  QString mySystemLocale = QTextCodec::locale();
  lblSystemLocale->setText(tr("Detected active locale on your system: ") + mySystemLocale);
  QString myUserLocale = mySettings.value("locale/userLocale", "").toString();
  QStringList myI18nList = Omgui::i18nList();
  cboLocale->addItems(myI18nList);
  if (myI18nList.contains(myUserLocale))
  {
    cboLocale->setCurrentText(myUserLocale);
  }
  bool myLocaleOverrideFlag = mySettings.value("locale/overrideFlag",false).toBool();
  grpLocale->setChecked(myLocaleOverrideFlag);


  //
  // Logging options
  //
  bool myLogToFileFlag = mySettings.value("openModeller/logToFile", false).toBool();
  QString myLogFileName = mySettings.value("openModeller/logFile","/tmp/omglog.txt").toString();
  grpLogToFile->setChecked(myLogToFileFlag);
  leLogFile->setText(myLogFileName);

  //
  // Map symbol options
  //
  spinLocalitySymbolSize->setValue(mySettings.value("mapping/localitySymbolSize",3.0).toDouble());
  QColor myPresenceColor(mySettings.value("mapping/presenceColour",QColor(Qt::green).name()).toString());
  QColor myAbsenceColor(mySettings.value("mapping/absenceColour",QColor(Qt::red).name()).toString());
  lblPresenceColourPreview->setStyleSheet("background-color: " + myPresenceColor.name());
  lblAbsenceColourPreview->setStyleSheet("background-color: " + myAbsenceColor.name());
  lblPresenceColourPreview->setText( myPresenceColor.name());
  lblAbsenceColourPreview->setText(myAbsenceColor.name());

  spinContextLineWidth->setValue(mySettings.value("mapping/contextLineWidth",0.1).toDouble());
  QColor myContextColor(mySettings.value("mapping/contextLineColour",QColor(Qt::gray).name()).toString());
  lblContextColourPreview->setStyleSheet("background-color: " + myContextColor.name());
  lblContextColourPreview->setText(myContextColor.name());
  leContextLayer->setText(mySettings.value("mapping/contextLayer").toString());
  cbxResetExtents->setChecked(mySettings.value("mapping/resetExtents",false).toBool());
}

OmgOptions::~OmgOptions()
{
}

void OmgOptions::on_cboModellerPluginName_currentIndexChanged()
{
  QString myKey = cboModellerPluginName->currentText();
  qDebug("Plugin changed to: " + myKey.toLocal8Bit());
  if (myKey=="Local Modeller Plugin")
  {
    //disable the url box because none is needed for this plugin
    leWebServiceAddress->setEnabled(false);
  }
  else
  {
    leWebServiceAddress->setEnabled(true);
  }
}

void OmgOptions::on_cboOutputFormat_currentIndexChanged()
{
  QHash<QString,QString> myHash = Omgui::getOutputFormatNotes();
  QString myKey = cboOutputFormat->itemData(cboOutputFormat->currentIndex()).toString();
  QString myNotes = myHash[myKey];
  lblFormatNotes->setText(myNotes);
}
void OmgOptions::accept()
{
  QSettings mySettings;
  //qDebug("Saving options");

  //User data dir settings
  mySettings.setValue("dataDirs/dataDir", leDataDirectory->text());
  mySettings.setValue("dataDirs/PreDataDir", leDataDirectory->text());
  //output format
  mySettings.setValue("outputFormat",cboOutputFormat->itemData(cboOutputFormat->currentIndex()).toString());
  //Web proxy settings
  mySettings.setValue("proxy/proxyEnabled", grpProxy->isChecked());
  mySettings.setValue("proxy/proxyHost", leProxyHost->text());
  mySettings.setValue("proxy/proxyPort", leProxyPort->text());
  mySettings.setValue("proxy/proxyUser", leProxyUser->text());
  mySettings.setValue("proxy/proxyPassword", leProxyPassword->text());

  //Gbif settings
  mySettings.setValue("openModeller/gbifPlugin/url",leGbifUrl->text());
  //Plugin url (only releveant for e.g. web services plugin)
  mySettings.setValue("openModeller/webServicesPlugin/url", leWebServiceAddress->text());
  //Plugin type
  mySettings.setValue("openModeller/modellerPluginType", cboModellerPluginName->currentText());
  mySettings.setValue("openModeller/webServicesPlugin/doLoggingFlag",  cbxModellerLoggingFlag->isChecked());
  mySettings.setValue("openModeller/modellerVerboseLogFlag",  cbxModellerVerboseFlag->isChecked());

  //Database settings
  mySettings.setValue("database/databaseEnabled", grpDatabase->isChecked());
  mySettings.setValue("database/databaseHost", leDbHost->text());
  mySettings.setValue("database/databaseUser", leDbUser->text());
  mySettings.setValue("database/databasePassword", leDbPassword->text());
  mySettings.setValue("database/databaseName", leDbName->text());
  mySettings.setValue("database/databaseTable", leDbTable->text());
  mySettings.setValue("database/taxonField", leDbTaxonField->text());
  mySettings.setValue("database/latitudeField", leDbLatitudeField->text());
  mySettings.setValue("database/longitudeField", leDbLongitudeField->text());
  //
  // Locale settings 
  //
  mySettings.setValue("locale/userLocale", cboLocale->currentText());
  mySettings.setValue("locale/overrideFlag", grpLocale->isChecked());
  // Tip of the day and gui settings
  mySettings.setValue("tipOfTheDay/tipOfTheDay", cbxShowTooltips->isChecked());
  mySettings.setValue("openModeller/runExperimentInThread", cbxExperimentInThread->isChecked());
  // System tray options
  mySettings.setValue("openModeller/useSystemTray", grpSystrayIcon->isChecked());
  mySettings.setValue("openModeller/closeToSystemTray", cbxMinimizeToSystray->isChecked());
  mySettings.setValue("openModeller/showSystemTrayMessages",cbxShowSystrayMessages->isChecked());

  //
  // Logging options
  //
  mySettings.setValue("openModeller/logToFile", grpLogToFile->isChecked());
  mySettings.value("openModeller/logFile",leLogFile->text());

  //
  // Map symbol colours
  //
  mySettings.setValue("mapping/localitySymbolSize",spinLocalitySymbolSize->value());
  mySettings.setValue("mapping/presenceColour",lblPresenceColourPreview->text());
  mySettings.setValue("mapping/absenceColour",lblAbsenceColourPreview->text());
  mySettings.setValue("mapping/contextLineWidth",spinContextLineWidth->value());
  mySettings.setValue("mapping/contextLineColour",lblContextColourPreview->text());
  mySettings.setValue("mapping/contextLayer",leContextLayer->text());
  mySettings.setValue("mapping/resetExtents",cbxResetExtents->isChecked());
  close(true);

}

void OmgOptions::on_toolSelectLogFile_clicked()
{
  QString myFileName = QFileDialog::getOpenFileName(
      this,
      tr("Select a directory where your data will be saved") , //caption
      QFileInfo(leLogFile->text()).absolutePath() //initial dir
    );
	leDataDirectory->setText(myFileName);
}

void OmgOptions::on_toolSelectDataDirectory_clicked()
{
  QString myFileName = QFileDialog::getExistingDirectory(
      this,
      tr("Select a directory where your data will be saved") , //caption
      leDataDirectory->text() //initial dir
    );
	leDataDirectory->setText(myFileName);
}

void OmgOptions::on_toolSelectContextLayer_clicked()
{
  QString myFileName = QFileDialog::getOpenFileName(
      this,
      tr("Select a context layer to overlay on your models") , //caption
      leContextLayer->text() //initial dir
    );
	leContextLayer->setText(myFileName);
}

void OmgOptions::on_toolPresenceColour_clicked()
{
  QColor myColour(lblPresenceColourPreview->text());
  myColour = QColorDialog::getColor( myColour, this );
  lblPresenceColourPreview->setStyleSheet("background-color: " + myColour.name());
  lblPresenceColourPreview->setText(myColour.name());
}
void OmgOptions::on_toolAbsenceColour_clicked()
{
  QColor myColour(lblAbsenceColourPreview->text());
  myColour = QColorDialog::getColor( myColour, this );
  lblAbsenceColourPreview->setStyleSheet("background-color: " + myColour.name());
  lblAbsenceColourPreview->setText(myColour.name());
}
void OmgOptions::on_toolContextColour_clicked()
{
  QColor myColour(lblContextColourPreview->text());
  myColour = QColorDialog::getColor( myColour, this );
  lblContextColourPreview->setStyleSheet("background-color: " + myColour.name());
  lblContextColourPreview->setText(myColour.name());
}
