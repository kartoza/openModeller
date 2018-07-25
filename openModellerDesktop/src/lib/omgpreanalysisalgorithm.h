/***************************************************************************
 *   Copyright (C) 2009 by Pedro Cavalcante                                *
 *   pedro.cavalcante@poli.usp.br                                          *
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
#ifndef OMGPREANALYSISALGORITHM_H
#define OMGPREANALYSISALGORITHM_H

#include <omgguid.h>
#include <omgalgorithmparameter.h>
#include "omgserialisable.h"

#include <QString>
#include <QVector>

#include <openmodeller/pre/PreAlgorithm.hh>

/** \ingroup library
* \brief A representation of an algorithm, including its state and user selected
* parameter values. An algorithm is (de)serialisable to xml and each
* algorithm instance gets a unique guid.
*/
class OMG_LIB_EXPORT OmgPreAnalysisAlgorithm : public OmgSerialisable, public OmgGuid
{
public:
  OmgPreAnalysisAlgorithm();
  ~OmgPreAnalysisAlgorithm();
  /** An enumerated type defining possible origins for the algorithm profile */
  // enum Origin {ADAPTERPROFILE=100,USERPROFILE=200,UNDEFINED=300};

  //ancilliary helpers
  // QString toHtml() const;
  int parameterCount();

  QString toXml() const;
  bool fromXml(const QString theXml) ;

  QString toString() const;
  //mutators

  void setId(QString theId);
  void setName(QString theName);
  void setFactoryName(QString theFactoryName);
  void setOverview(QString theOverview);
  void setParameters(QVector<OmgAlgorithmParameter> theParameters);
  void addParameter(OmgAlgorithmParameter theParameter);


  // here you point to the object of the Pre-Analysis Algorithm
  void setAlgorithm(PreAlgorithm *theAlgorithm);


  // void setParameters(QVector<OmgAlgorithmParameter> theParameters);
  // void addParameter(OmgAlgorithmParameter theParameter);
  //accessors

  /** This is the id assigned by the algorithm writer e.g. Bioclim */
  QString id();
  QString name() const;
  QString factoryName();
  QString author();
  QString contact();
  QString codeAuthor();
  QString version();
  int categorical();
  int absence();
  QString overview();
  QString description();
  QString bibliography();
  PreAlgorithm *algorithm();
  QVector<OmgAlgorithmParameter> parameters();
  /** Get a parameter from the algorithm given its id.
   * Note a copy is returned. If no match could be 
   * found the returned parameter will have an
   * id of 'invalid' */
  OmgAlgorithmParameter parameter(QString theId);

private:

  QString mId;
  QString mName;
  QString mFactoryName;
  QString mOverview;
  QVector<OmgAlgorithmParameter> mParameters;

  PreAlgorithm *mAlgorithm;

};

#endif //OMGPREANALYSISALGORITHM_H
