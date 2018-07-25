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

#ifndef OMGLAYERSET_H
#define OMGLAYERSET_H

class QString;
#include <QMap>
#include "omgguid.h"
#include "omglayer.h"
#include "omgserialisable.h"
/** \ingroup library
  * \brief A layerset is a container for a collection of layer objects
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgLayerSet  :  public OmgSerialisable, public OmgGuid
{
  public:
    /** Constructor . */
    OmgLayerSet();
    /** Desctructor . */
    ~OmgLayerSet();

    /** Add a layer to the layerset
     * @see OmgLayer 
     */
    bool addLayer(OmgLayer theLayer);
    /** Remove a layer from the layerset
     * @see OmgLayer
     */
    bool removeLayer(OmgLayer theLayer);
    /** Overloaded method to remove a layer from 
     * the layerset given only its layername.
     */
    bool removeLayer(QString theLayer);

    //
    // Accessors
    //

    /** The name of this layer - usually a full pathname and file name though this
     * may vary depending on the OmgModellerAdapter implementations */
    QString name() const;
    QString description() const;
    OmgLayer mask() const;
    /** Helper method to return a name list of all the 
     * non mask layer names.
     */
    QStringList nameList() const;
    typedef QMap<QString,OmgLayer> LayersMap;
    /** Return a copy of the internal qmap with all associated layers in it */
    LayersMap layers() const;
    //
    // Mutators
    //

    /** Set the layerSet Name
     * @see name()
     */
    void setName(QString theName);
    
    /** Set a description for the layerset
     * @see description()
     */
    void setDescription(QString theDescription);
    
    /** Set the mask layer for this layerset.
     * A mask is defined by the non null cells in a gdal dataset.
     */
    void setMask(OmgLayer theMask);
    

    //
    // Ancilliary helper methods
    //

    /**
     * Return the name of the mask layer
     */
    QString maskName() const;
    
    /**
     * Retrun the count of layers in the layerset - excluding the mask
     */
    int count() const;
    /** Return a string representation of this LayerSet
     */
    QString toString() const;

    /** Return an xml representation of this LayerSet
     * @see OmgSerialisable
     */
    
    QString toXml(bool includeExtraNodes=true) const ;
    /** Initialise this layerset from an xml representation of this LayerSet
     * @see OmgSerialisable
     */
    bool fromXml(const QString) ;
      
      
    /** Return an html representation of this LayerSet
     * e.g.
     * <ul>
     *  <li for each layer >
     * </ul>
     */
    QString toHtml() const;
    
    /** Save the layerset to disk. If no filename is specified it will
     * save itself into the users layersets dir using guid as the base for its
     * filename.
     * @param theFileName - a name for the file (not required)
     * @return bool - true if save succeeded
     */
    bool save(QString theFileName="") const;
  private:
    QString mName;
    QString mDescription;
    OmgLayer mMask;
    LayersMap mLayersMap;
};

#endif //OMGLAYERSET_H

