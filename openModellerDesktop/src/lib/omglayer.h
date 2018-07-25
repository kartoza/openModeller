/***************************************************************************
                          omglayer.h  -  description
                             -------------------
    begin                : March 2006
    copyright            : (C) 2003 by Tim Sutton
    email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OMGLAYER_H
#define OMGLAYER_H

class QString;
#include <QFile>
#include "omgserialisable.h"
/** \ingroup library
  * \brief An abstract base class for any class that is serialiseable to xml
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgLayer : public OmgSerialisable 
{
  public:
    /** Constructor . */
    OmgLayer();
    /** Desctructor . */
    ~OmgLayer();
    /** copy constructor */
    OmgLayer(const OmgLayer& theLayer); 
    /** Assignement operator */
    OmgLayer& operator= (const OmgLayer& theLayer);
    
    /** Enumeration of possible types a layer can be. 
     * Note that format is used when specifying the output cellsize 
     * and extents for a model. 
     */
    enum LayerType {MASK,MAP,FORMAT};

    //
    // Accessors
    //

    /** The name of this layer - usually a full pathname and file name though this
     * may vary depending on the OmgModellerAdapter implementations */
    QString name() const;
    /** Whether this layer represents categorical data in discrete cless or
     * a continuous surface */
    bool isCategorical() const;
    /** The type of this layer. 
     * Note that type 'FORMAT' is used when specifying the output cellsize 
     * and extents for a model. 
     */
    LayerType type() const;

    //
    // Mutators
    //

    /** Set the layerName
     * @see name()
     */
    void setName(QString theName);
    /** Set the categorical flag
     * @see isCategorical()
     */
    void setCategorical(bool theFlag);
    /** Set the layer type
     * @see layerType()
     */
    void setType(LayerType theType);
    /** Return an xml representation of this layer
     * @NOTE this class is not fully serialiseable (yet??)
     */
    QString toXml() const;
    /** Read this object from xml and return result as true for success, false for failure.
     *  @see OmgSerialisable
     *  @TODO Implement this!!!
     */
    bool fromXml(const QString theXml) { return false; } ;
  private:
    /** A flag to store whehter this layer contains categorical or continuous data */
    bool mCategoricalFlag;
    /** The name for this layer - usually a full path and filename */
    QString mName;
    /** The type / role of this layer - mask / map / output format specifier */
    LayerType mType; 
};

#endif //OMGLAYER_H

