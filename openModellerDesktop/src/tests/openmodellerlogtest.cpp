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
#include <QtTest/QtTest>
//om includes
#include <openmodeller/om.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Log.hh>
//
#include <QTextStream>


//void modelCallback( float theProgress, void *thePlugin );
//void mapCallback( float theProgress, void *thePlugin );
class OmgOmLogCallback : public QObject, public Log::LogCallback
{
  public:
  OmgOmLogCallback() : 
    QObject(),
    LogCallback()
  {

  }

  ~OmgOmLogCallback( )
  {

  }

  //do not try to use the text stream!
  OmgOmLogCallback( QTextStream& theTextStream ) : 
    QObject(), 
    LogCallback( )
  {

  }


  void operator()(Log::Level theLevel, const std::string& theLogMessage )
  {
      qDebug("--->");
      qDebug(theLogMessage.c_str());
  }
};

class openmodellerlogtest: public QObject
{
  Q_OBJECT;
  private:
  OpenModeller * mpOpenModeller;
  OmgOmLogCallback * mpLogCallBack;
  QTextStream mTextStream;
  private slots:
  void loadOpenModeller();
};

void openmodellerlogtest::loadOpenModeller()
{
  QTextStream* output = new QTextStream();
  mpOpenModeller = new OpenModeller();
  Log::instance()->setLevel( Log::Debug );
  //Log::instance()->setLevel( Log::Default );
  //set up callbacks
  mpLogCallBack = new OmgOmLogCallback(*(output));
  Log::instance()->setCallback(mpLogCallBack );
  AlgorithmFactory::searchDefaultDirs();

  //mpOpenModeller->setModelCallback( modelCallback, this );
  //mpOpenModeller->setMapCallback( mapCallback, this );
  //QVERIFY(myList.size()>0);
  //QFAIL("Could not write temporary layerset xml to disk");
}

QTEST_MAIN(openmodellerlogtest) 
#include "moc_openmodellerlogtest.cxx"

