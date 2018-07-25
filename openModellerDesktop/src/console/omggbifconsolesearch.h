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

#ifndef OMGGBIFCONSOLESEARCH_H 
#define OMGGBIFCONSOLESEARCH_H

#include <omgscrapergbif.h> 

//qt includes
#include <QSettings>

#include <QString>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QListIterator>

//standard includes
#include <iostream>
#include <assert.h>

class QString;
class QStringList;

class OmgGbifConsoleSearch : public QObject
{
  Q_OBJECT;

public:
  OmgGbifConsoleSearch(QObject *parent=0);
  ~OmgGbifConsoleSearch();
  bool search(QString theFileName);

public slots:
  void gettingTaxonID();
  void gettingBody();
  void fileWritten(QString theShapeFile, QString theTextFile,QString theTaxonName, int theCount);
  void fileNotWritten(QString theTaxonName);
  void showPoints(QString theFileName);
  void setStatus(QString);
  void appendLogMessage(QString);
  void setError(QString);

signals:
  void searchDone();

private:
  
  void setSearchList(QString theFileName);
  void processNextTaxon();
  bool mBusyFlag;
  int mListPosition;
  int mListSize;
  OmgScraperGbif * mOmgScraperGbif;
  QStringList mNameList;
 
};
#endif
