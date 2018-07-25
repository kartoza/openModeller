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
#ifndef OMGWEBPAGEFETCHER_H
#define OMGWEBPAGEFETCHER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QHttp>
#include <QHttpResponseHeader>
/** \ingroup library
 * \brief This class encapsulates everything needed to fetch the web page at a
 * given url. It will emit a signal with a string containing the result 
 * when done.
 * @author Tim Sutton
*/
class OMG_LIB_EXPORT OmgWebPageFetcher : public QObject
{
Q_OBJECT;
public:
    OmgWebPageFetcher(QObject *parent=0);
    ~OmgWebPageFetcher();
    QString url();
    QString getPage(QString theUrl);

protected slots:

  void slotStateChanged(int state);
  void slotResponseHeaderReceived ( const QHttpResponseHeader & resp );
  void slotRequestFinished ( int id, bool error );
  void slotTimeOut();

signals:
  void requestGetFinished(QString);
  void statusChanged(QString);
private:
  QUrl mQUrl;
  int mTimeOut;
  QHttp mQHttp;
  int mProxyId;
  int mHostId;
  int mHeaderId;
  int mGetId;
  QString mRequestQString;
  QString mPageContent;
  QHttpResponseHeader mQHttpResponseHeader;
  bool mBusyFlag;
  //private methods

  void requestHeadFinished(int id);
  void finish();

};

#endif //OMGWEBPAGEFETCHER
