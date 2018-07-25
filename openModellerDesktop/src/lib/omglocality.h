/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
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
#ifndef OMGLOCALITY_H 
#define OMGLOCALITY_H

#include <QVector>
#include <QMap>
#include <QString>

/** \ingroup library
 * \brief  A vector type for storing the sampled environment variables
 * associated with a locality.*/
typedef QVector<float> OmgSampleVector;

/** \ingroup library
  * \brief The Locality class stores data for one location
  */
class OMG_LIB_EXPORT OmgLocality 
{
  public:
    OmgLocality();
    ~OmgLocality();
    //
    // accessors
    //
    /** Get the id for this locality */
    QString id() const;
    /** Get the label / description of this locality */       
    QString label() const;
    /** Get the latitude of this locality */       
    float latitude() const;
    /** Get the longitude of this locality */       
    float longitude() const;
    /** Get the abundance of this locality */       
    float abundance() const;
    /** Get the samples of this locality */       
    OmgSampleVector samples() const;

    //
    // Mutators
    //
    /** Set the id for this locality */
    void setId(QString theId);
    /** Set the label / description of this locality */       
    void setLabel(QString theLabel);
    /** Set the latitude of this locality */       
    void setLatitude(float theLatitude);
    /** Set the longitude of this locality */       
    void setLongitude(float theLongitude);
    /** Set the abundance of this locality */       
    void setAbundance(float theAbundance);
    /** Set the samples of this locality */       
    void setSamples(OmgSampleVector theSamples);
    
    //helper functions
    QString toString();
    QString toHtml();
    QString toXml();
    bool isValid();

    
  private:
  /** A unique Id for the locality. Uniqueness is not enforced at this stage. */
  QString mId;
  /** A label for the locality. */
  QString mLabel;
  /** The Latitude*/
  float mLatitude;
  /** The Longitude */
  float mLongitude;
  /** The Abundance */
  float mAbundance;
  /** The collection of sampled environment values associated with this
   * locality. Populating this member is not required for when you want to 
   * create a model, but should be populated when projecting a model. */
  OmgSampleVector mSamples;

};

//this must be after the above class def

/** A vector for storing localities */
typedef QVector<OmgLocality> OmgLocalities;

/** Load localities from a given file
 *  @param OmgLocalities Localities storage
 *  @param QString The file from where localities will be loaded
 *  @param QString Only lines with the corresponding label will be loaded
 *  @return unsigned int Number of localities loaded
 */
OMG_LIB_EXPORT unsigned int loadLocalities( OmgLocalities& theLocalities, QString theSpeciesFile, QString theLabel);

/** Splits data points into to 2 new subsets
 *  @param OmgLocalities Original OmgLocalities to split
 *  @param OmgLocalities First set of OmgLocalities to be returned
 *  @param OmgLocalities Second set of OmgLocalities to be returned
 *  @param double Proportion of localities to go to the first set
 */
OMG_LIB_EXPORT void splitLocality(const OmgLocalities& theLocalities, OmgLocalities& theFirstSet, OmgLocalities& theSecondSet, double proportion);



#endif //OMGLOCALITY_H
