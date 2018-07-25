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
#include "omgwebpagefetcher.h"
#include <QString>
#include <QDebug>
#include <iostream>
#include <cassert>
#include <QApplication>
#include <QSettings>
OmgWebPageFetcher::OmgWebPageFetcher(QObject *parent)
    : QObject(parent),
    mProxyId(0),
    mHostId(0),
    mHeaderId(0),
    mGetId(0)
{
  mTimeOut = 30; // 30second timeout for web requests
  //
  // Setup signals and slots
  //
  connect(&mQHttp, SIGNAL(stateChanged(int)),
      this, SLOT(slotStateChanged(int)));
  connect(&mQHttp, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
      this, SLOT(slotResponseHeaderReceived(const QHttpResponseHeader &)));
  connect(&mQHttp, SIGNAL(requestFinished(int, bool)),
      this, SLOT(slotRequestFinished(int, bool)));
}


OmgWebPageFetcher::~OmgWebPageFetcher()
{
  //finish should have been called already which deletes qhttp pointer
}



QString OmgWebPageFetcher::getPage(QString theUrl)
{
  if (theUrl.isEmpty())
  {
    emit statusChanged("Can not have a request with empty url!");
    return QString("");
  }
  qDebug("Initialising Web Page Fetcher, current id should be 0, Current Id is: " + QString::number(mQHttp.currentId()).toLocal8Bit());
  //mQHttp.clearPendingRequests();
  mQUrl = QUrl(theUrl);
  emit statusChanged("Set url to :  " +  mQUrl.path());
  mBusyFlag=true;
  //
  // Setup proxy if needed
  //
  QSettings mySettings;
  bool myProxyFlag = mySettings.value("proxy/proxyEnabled", "0").toBool();
  if (myProxyFlag)
  {
    QString myHost = mySettings.value("proxy/proxyHost", "").toString();
    int myPort = mySettings.value("proxy/proxyPort", "").toInt();
    QString myUser = mySettings.value("proxy/proxyUser", "").toString();
    QString myPassword = mySettings.value("proxy/proxyPassword", "").toString();
    mProxyId = mQHttp.setProxy(myHost, myPort , myUser, myPassword);
    qDebug("User:" + myUser.toLocal8Bit());
    qDebug("Pwd: ********** ");
    qDebug("Host:" + myHost.toLocal8Bit());
    qDebug("Port:" + QString::number(myPort).toLocal8Bit());
    qDebug ("Id for proxy setup: " + QString::number(mProxyId).toLocal8Bit());
  }
  else //no proxy so we can just move on to getting the host
  {
    mHostId = mQHttp.setHost(mQUrl.host());
    qDebug("Set host to" + mQUrl.host().toLocal8Bit() + " request id is " + QString::number(mHostId).toLocal8Bit());
    //see slotRequesFinished.....
    //QTimer::singleShot( mTimeOut * 1000, this, SLOT(slotTimeOut()) );
  }
  // block until page is returned
  while (mBusyFlag)
  {
    QApplication::processEvents();
  }
  return mPageContent ;
}

void OmgWebPageFetcher::finish()
{
  qDebug ("Web page fetcher finish() reached....");
  // Don't go back to slotRequestFinished
  /*
  mQHttp.blockSignals(true);
  disconnect(&mQHttp);
  mQHttp.abort();
  mQHttp.clearPendingRequests();
  mQHttp.close();
  */
  mProxyId=0;
  mHostId=0;
  mHeaderId=0;
  mGetId=0;
  emit statusChanged("Requests completed, closing connection");
  qDebug ("....done\n\n");
  mBusyFlag=false;
}

void OmgWebPageFetcher::slotRequestFinished(int id, bool error)
{
  qDebug("Request " + QString::number(id).toLocal8Bit()  + " finished");
  qDebug("Proxy Id " + QString::number(mProxyId).toLocal8Bit() );
  qDebug("Host Id " + QString::number(mHostId).toLocal8Bit() );
  qDebug("Header Id " + QString::number(mHeaderId).toLocal8Bit() );
  qDebug("Body Id " + QString::number(mGetId).toLocal8Bit() );
  if(error)
  {
    qDebug("Request Finished Error: " +  mQHttp.errorString().toLocal8Bit());
    emit statusChanged(  "Error: " +  mQHttp.errorString() );
    //finish();
  }
  else if(id == mProxyId) //proxy setup ok (optional)
  {
    qDebug("Proxy set!");
    mHostId = mQHttp.setHost(mQUrl.host());
    qDebug("Set host to" + mQUrl.host().toLocal8Bit() + " request id is " + QString::number(mHostId).toLocal8Bit());
    //now we sit back and wait for the host id to come, then we
    //will ask for the header, then we will get the body...
    //QTimer::singleShot( mTimeOut * 1000, this, SLOT(slotTimeOut()) );
  }
  else if(id == mHostId) //host retrieved
  {
    qDebug("Host set!");
    if(!mQUrl.encodedQuery().isNull() || !mQUrl.fragment().isEmpty() )
    {
      mRequestQString = mQUrl.toString();
    }
    else
    {
      mRequestQString = mQUrl.path();
      //!@note when status changes to closing, client will be notified
    }
    qDebug("Requesting headers for url \n" + mRequestQString.toLocal8Bit());
    mHeaderId = mQHttp.head(mRequestQString);
    qDebug("Header id for this request is " + QString::number(mHeaderId).toLocal8Bit());
  }
  else if(id == mHeaderId) //head retrieved
  {
    qDebug("Page header Received!");
    requestHeadFinished(id);
  }
  else if(id == mGetId)  //body retrieved
  {
    qDebug("Page body Received");
    mPageContent = mQHttp.readAll();
    mQHttp.blockSignals(true);
    //qDebug() << myContentString;
    qDebug ("Request completed...state id is now " +
           QString::number(mQHttp.state()).toLocal8Bit() 
           + "\nClosing connection");
    //
    // NOTE Its important to *first* call finish and then only emit the request finished signal
    // otherwise clients might start to manipulate thise object before it has properly cleaned
    // itself up!
    //
    finish();
    //must be last thing we do!
    //emit requestGetFinished(mPageContent);
  }
  else // id did not match any expected
  {
    qDebug ("Request finished for unknown id");
  }
}

void OmgWebPageFetcher::slotTimeOut()
{
  //qDebug() <<   " * * * Web Page Fetcher request timed out * * * " ;
  if(mQHttp.state() == QHttp::Connecting)
  {
    assert(mQHttp.currentId() == mHeaderId ||
           mQHttp.currentId() == mGetId);
    emit statusChanged("Error: timeout on connection");
    finish();
  }
}

void OmgWebPageFetcher::slotResponseHeaderReceived(const QHttpResponseHeader& resp)
{
  mQHttpResponseHeader = resp;
}

void OmgWebPageFetcher::requestHeadFinished(int id)
{
  qDebug ("Header request finished");
  qDebug ("Content type returned : " + mQHttpResponseHeader.contentType().toLocal8Bit());
  assert(id == mHeaderId);
  int myStatusCode=mQHttpResponseHeader.statusCode();
  // 405 -> it might not support HEAD request's...
  // 200 -> ok, operation successful
  // See also http://www.the-eggman.com/seminars/html_error_codes.htm
  if( (myStatusCode == 200 || myStatusCode == 202 || myStatusCode == 405) &&
      (mQHttpResponseHeader.contentType() == "text/html" ||
       mQHttpResponseHeader.contentType() == "text/plain" ||
       mQHttpResponseHeader.contentType() == "text/xml") )
  {
    //move on to request the body now
    mGetId = mQHttp.get(mRequestQString);
    qDebug("Page header received successfully");
    emit statusChanged("Page header received successfully: " + mQHttpResponseHeader.statusCode());
  }
  else if (myStatusCode == 301)
  { 
    qDebug("301 Error");
    emit statusChanged("requestHeadFinished Error: " + QString::number(myStatusCode) + " Page moved permanently!");
    finish();
   }
   else if (myStatusCode == 302)
  {
    qDebug("302 Error");
    emit statusChanged("requestHeadFinished Error: " + QString::number(myStatusCode) + " Page moved temporarily!");
    finish();
  }
  else
  {
    qDebug("Header retrieval Error" + QString::number(myStatusCode).toLocal8Bit());
    emit statusChanged("requestHeadFinished Error: " + myStatusCode  );
    finish();
  }
}



QString OmgWebPageFetcher::url()
{
  return mQUrl.toString();
}


void OmgWebPageFetcher::slotStateChanged(int state)
{
  switch (state)
  {
  case QHttp::Unconnected:
    qDebug("---->State Change<---- Unconnected");
    emit statusChanged("Unconnected");
    break;
  case QHttp::HostLookup:
    qDebug("---->State Change<---- Host Lookup");
    emit statusChanged("Host Lookup");
    break;
  case QHttp::Connecting:
    qDebug("---->State Change<---- Connecting");
    emit statusChanged("Connecting");
    break;
  case QHttp::Sending:
    qDebug("---->State Change<---- Sending");
    emit statusChanged("Sending");
    break;
  case QHttp::Reading:
    qDebug("---->State Change<---- Reading");
    emit statusChanged("Reading");
    break;
  case QHttp::Connected:
    qDebug("---->State Change<---- Connected");
    emit statusChanged("Connected");
    break;
  case QHttp::Closing: ;
    qDebug("---->State Change<---- Closing");
    emit statusChanged("Closing");
    break;
  default :
    qDebug("---->State Change<---- Illegal state");
    emit statusChanged("Illegal state");
    break;
  }
}
