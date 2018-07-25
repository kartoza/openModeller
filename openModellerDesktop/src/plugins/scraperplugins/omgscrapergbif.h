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

#ifndef OMGSCRAPERGBIF_H
#define OMGSCRAPERGBIF_H

#include <omglocality.h>
#include <omgscraperplugininterface.h>

//QT Includes
#include <QObject>

class QString;

class OMG_PLUGIN_EXPORT OmgScraperGbif :public QObject,  public OmgScraperPluginInterface
{
  Q_OBJECT;
  Q_INTERFACES(OmgScraperPluginInterface);

  public: 
  OmgScraperGbif(QObject* parent=0);
  ~OmgScraperGbif();
  bool search(QString theTaxonName, QString theFileName);
  /** Get the user friendly name for this plugin
   *  @return QString - the name of this plugin
   **/
  const  QString getName();
  /** Get the terms of useage agreement for this plugin
   * @return QString containinig agreement text.
   */
  const QString getLicense();

  private slots:
    /** first part of the request is to find out the id of the species as used in gbif 
     * @return A url of where the page containing data can be acquired
     */
    QString taxonIdRequestDone(QString theString);
  //!a parser to pull localities out of a web page //
  bool localitiesRequestDone(QString theString);

};

#endif
