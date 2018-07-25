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
#ifndef OMGALGORITHMPARAMETER_H
#define OMGALGORITHMPARAMETER_H

#include "omgserialisable.h"

#include <QString>

/** \ingroup library
 * \brief An algorithm parameter is a representation of a single parameter
 * associated with an algorithm. It is (de)serialisable to xml.
 * @see OmgAlgorithm
 */
class OMG_LIB_EXPORT OmgAlgorithmParameter : public OmgSerialisable
{
public:
  OmgAlgorithmParameter();
  ~OmgAlgorithmParameter();
  OmgAlgorithmParameter(
    QString theId,
    QString theName,
    QString theDefault,
    QString theOverview,
    QString theDescription,
    QString theMinimum,
    QString theMaximum,
    QString theValue
    );

  //
  //Serialisable interface
  //
  
  /** This method is required for the serialiseable interface. It will 
   * encapsulate *all* algorithm parameters so that its complete state can
   * be serialised and later deserialised.
   * @see toModelCreationXml()
   * @note no special case is provided for model projection as no parameters
   * should normally be needed.
   */
  QString toXml() const;
  /** Implments the serialisable interface method to reinstate an algorithm.
   * It will try to populate all members of this class and fail gracefully when
   * they are not available (typically by leaving the member tagged as [Not Set]
   */
  bool fromXml(const QString theXml) ;

  /** This is a special case serialiser needed for when preparing an xml representation
   * of a parameter to send to the openmodeller library (or openmodeller web service
   * interface).
   * The xml produced will include no ancilliary meta information - only the essentials
   * needed for the algorithm. For example:
   * <Parameter Value="0.674" Id="StandardDeviationCutoff" /> 
   * @return a string containing the parameter xml representation or if something went wrong
   * and empty string.
   */
  QString toModelCreationXml() const;

  /** Provide a string representation of this param */
  QString toString() const;
  //mutators
  
  void setId(QString theId);
  void setName(QString theName);
  void setType(QString theType);
  void setDefault(QString theDefault);
  void setOverview(QString theOverview);
  void setDescription(QString theDescription);
  void setMinimum(QString theMinimum);
  void setMaximum(QString theMaximum);
  /** The user defined value for this parameter */
  void setValue(QString theValue);


  //accessors
  
  QString id() const;
  QString name() const ;
  QString type() const ;
  QString overview() const ;
  QString defaultValue() const ;
  QString description() const;
  QString minimum() const ;
  QString maximum() const ;
  QString value() const ;
 
private:
  QString mId;
  QString mName;
  QString mType;
  QString mDefault;
  QString mOverview;
  QString mDescription;
  QString mMinimum;
  QString mMaximum;
  QString mValue;
};

#endif //OMGALGORITHMPARAMETER_H
