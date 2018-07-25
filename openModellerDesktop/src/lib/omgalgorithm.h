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
#ifndef OMGALGORITHM_H
#define OMGALGORITHM_H

#include <omgguid.h>
#include <omgalgorithmparameter.h>
#include "omgserialisable.h"

#include <QString>
#include <QVector>

/** \ingroup library
* \brief A representation of an algorithm, including its state and user selected
* parameter values. An algorithm is (de)serialisable to xml and each
* algorithm instance gets a unique guid.
*/
class OMG_LIB_EXPORT OmgAlgorithm : public OmgSerialisable, public OmgGuid
{
public:
  OmgAlgorithm();
  ~OmgAlgorithm();
  /** An enumerated type defining possible origins for the algorithm profile */
  enum Origin {ADAPTERPROFILE=100,USERPROFILE=200,UNDEFINED=300};

  //ancilliary helpers
  QString toHtml() const;
  int parameterCount();
  
  //
  //serialisable interface
  //
  
  /** This method is required for the serialiseable interface. It will 
   * encapsulate *all* algorithm properties so that its complete state can
   * be serialised and later deserialised.
   * @see toModelCreationXml()
   */
  QString toXml() const;
  /** Implments the serialisable interface method to reinstate an algorithm.
   * It will try to populate all members of this class and fail gracefully when
   * they are not available (typically by leaving the member tagged as [Not Set]
   */
  bool fromXml(const QString theXml) ;

  /** This is a special case serialiser needed for when preparing an xml representation
   * of an algorithm to send to the openmodeller library (or openmodeller web service
   * interface).
   * The xml produced will include no ancilliary meta information - only the essentials
   * needed for the algorithm. 
   * @return a string containing the algorithm xml representation or if something went wrong
   * and empty string.
   */
  QString toModelCreationXml() const;

  QString toSerializedModelXml(const QString theNormalizationXml, const QString theModelDefinitionXml) const;

  /** This is a special case serialiser needed for when preparing an xml representation
   * of an algorithm to send to the openmodeller library (or openmodeller web service
   * interface).
   * The xml produced will include no ancilliary meta information - only the essentials
   * needed for the algorithm. 
   * @param a string containing the serialised model definition as returned by an algorithm 
   * @param an (optional) string with the normalization xml for the model
   * in libopenmodeller
   * @return a string containing the algorithm xml representation or if something went wrong
   * and empty string.
   */
  QString toModelProjectionXml(QString theModel, QString theNormalization="") const;

  QString toString() const;
  //mutators

  void setId(QString theId);
  void setName(QString theName);
  void setAuthor(QString theAuthor);
  void setCodeAuthor(QString theCodeAuthor);
  void setContact(QString theContact);
  void setVersion(QString theVersion);
  void setCategorical(int theCategorical);
  void setAbsence(int theAbsence);
  void setOverview(QString theOverview);
  void setDescription(QString theDescription);
  void setBibliography(QString theBibliography);
  void setParameters(QVector<OmgAlgorithmParameter> theParameters);
  void addParameter(OmgAlgorithmParameter theParameter);
  void setOrigin(Origin theOrigin);
  //accessors

  /** This is the id assigned by the algorithm writer e.g. Bioclim */
  QString id();
  QString name() const;
  QString author();
  QString contact();
  QString codeAuthor();
  QString version();
  int categorical();
  int absence();
  QString overview();
  QString description();
  QString bibliography();
  QVector<OmgAlgorithmParameter> parameters();
  /** Get a parameter from the algorithm given its id.
   * Note a copy is returned. If no match could be 
   * found the returned parameter will have an
   * id of 'invalid' */
  OmgAlgorithmParameter parameter(QString theId);
  Origin origin();
private:

  QString mId;
  QString mName;
  QString mAuthor;
  QString mCodeAuthor;
  QString mContact;
  QString mVersion;
  int mCategorical;
  int mAbsence;
  QString mOverview;
  QString mDescription;
  QString mBibliography;
  QVector<OmgAlgorithmParameter> mParameters;
  /** A property to indicate the origin of this algorithm profile.
   * @NOTE this is NOT serialised/deserialised
   */
  Origin mOrigin;

};

#endif //OMGALGORITHM_H
