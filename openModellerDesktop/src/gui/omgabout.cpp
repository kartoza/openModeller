/***************************************************************************
 *   Copyright (C) 2006 Peter Brewer, Tim Sutton   *
 *   peter@pembshideaway.co.uk, tim@linfiniti.com   *
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
#include "omgabout.h"
#include "omgui.h"
#include <omgmodellerpluginregistry.h>
#include <omgscraperpluginregistry.h>
#include <qgsproviderregistry.h>
#include <omgui.h>  // for version
#include <QString>
#include <QUrl>
#include <QSqlDatabase>
#include <QImageReader>

OmgAbout::OmgAbout(QWidget* parent, Qt::WFlags fl)
    : QDialog(parent,fl)
{
  //required by Qt4 to initialise the ui
  setupUi(this);
  //provide general info about omg
  QString myOmgVersion = Omgui::version();
  lblVersion->setText(myOmgVersion + tr(" using Qt ") + QT_VERSION_STR);

  QString myString;
  //provide info about the plugins available
  myString += "<b>" + tr("Available Modelling Plugins") + "</b><br>";
  myString += "<ol>\n<li>\n";
  QStringList myModellerList = OmgModellerPluginRegistry::instance()->names();
  myString += myModellerList.join("</li>\n<li>");
  myString += "</li>\n</ol>\n";
  myString += "<b>" + tr("Available Data Retrieval Plugins") + "</b><br>";
  myString += "<ol>\n<li>\n";
  QStringList myScraperList = OmgScraperPluginRegistry::instance()->names();
  myString += myScraperList.join("</li>\n<li>");
  myString += "</li>\n</ol>\n";
  myString += "<b>" + tr("Available QGIS Data Provider Plugins") + "</b><br>";
  myString += QgsProviderRegistry::instance()->pluginList(true);
  //qt database plugins
  myString += "<b>" + tr("Available Qt Database Plugins") + "</b><br>";
  myString += "<ol>\n<li>\n";
  QStringList myDbDriverList = QSqlDatabase::drivers ();
  myString += myDbDriverList.join("</li>\n<li>");
  myString += "</li>\n</ol>\n";
  //qt image plugins
  myString += "<b>" + tr("Available Qt Image Plugins") + "</b><br>";
  myString += "<ol>\n<li>\n";
  QList<QByteArray> myImageFormats = QImageReader::supportedImageFormats();
  QList<QByteArray>::const_iterator myIterator = myImageFormats.begin();
  while( myIterator != myImageFormats.end() )
  {
    QString myFormat = (*myIterator).data();
    myString += myFormat + "</li>\n<li>";
    ++myIterator;
  }
  myString += "</li>\n</ol>\n";
  //done, add to tb
  tbPlugins->insertHtml(myString);
  tbPlugins->moveCursor(QTextCursor::Start);
  // Set the changelog info
  // a clever shortcut way to load it from the resource file
  tbChangeLog->setSearchPaths(QStringList(":/"));
  tbChangeLog->setSource(QUrl("CHANGELOG.html"));
  tbContributors->setSearchPaths(QStringList(":/"));
  tbContributors->setSource(QUrl("CONTRIBUTORS.html"));

}

OmgAbout::~OmgAbout()
{}

void OmgAbout::on_btnOk_clicked()
{
    QDialog::accept();
}

