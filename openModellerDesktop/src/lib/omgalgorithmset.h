/***************************************************************************
                          omgalgorithm.h  -  description
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

#ifndef OMGALGORITHMSET_H
#define OMGALGORITHMSET_H

class QString;
#include <QMap>
#include "omgalgorithm.h"
#include "omgserialisable.h"
/** \ingroup library
  * \brief An container class for algorithm definitions. It also has ability to self initialise
  * using the active openModeller plugin (and by looking for algorithms profiles stored
  * as xml in the filesystem.
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgAlgorithmSet  :  public OmgSerialisable
{
    public:
        /** Constructor . */
        OmgAlgorithmSet();
        /** Desctructor . */
        ~OmgAlgorithmSet();

        /** Add a algorithm to the algorithmset
         * @param theAlgorithm the alg to add to the set
         * @param theOverwriteFlag - whether to write any preexisting alg 
         *        with the same name (default is false)
         * @see OmgAlgorithm 
         */
        bool addAlgorithm(OmgAlgorithm theAlgorithm, bool theOverwriteFlag=false);
        /** Remove a algorithm from the algorithmset
         * @see OmgAlgorithm
         */
        bool removeAlgorithm(OmgAlgorithm theAlgorithm);
        /** Overloaded method to remove a algorithm from 
         * the algorithmset given only its algorithm Name.
         */
        bool removeAlgorithm(QString theAlgorithm);

        /** Retirieve an algorithmset using the currently active plugin in 
         * qsettings. */
        static OmgAlgorithmSet getFromActivePlugin();
        //
        // Accessors
        //

        /** The name of this algorithmset - in plain english */
        QString name() const;
        /** A description for the layerset e.g. including details like 
         * hostname etc for remote services */
        QString description() const;
        /** Helper method to return a list of all the algorithm names.
        */
        QStringList nameList();
        //
        // Mutators
        //

        /** Set the algorithmSet Name
         * @see name()
         */
        void setName(QString theName);

        /** Set a description for the algorithmset
         * @see description()
         */
        void setDescription(QString theDescription);


        //
        // Ancilliary helper methods
        //

	      /**
	      * Clear all entries from the alg set
	      */
	      void clear();


        /**
         * Retrun the count of algorithms in the algorithmset - excluding the mask
         */
        int count() const;
        /** Return a string representation of this AlgorithmSet
        */
        QString toString();

        /** Return an xml representation of this AlgorithmSet
         * @see OmgSerialisable
         */

        QString toXml() const;
        /** Initialise this algorithmset from an xml representation of this AlgorithmSet
         * @see OmgSerialisable
         */
        bool fromXml(const QString) ;


        /** Return an html representation of this AlgorithmSet
         * e.g.
         * <ul>
         *  <li for each algorithm >
         * </ul>
         */
        QString toHtml();

        /** Append any algoirthms that can be deserialised from the
         * data directory onto this algorithm set. 
         * @param theSearchDir - the directory where xml parameters should
         * be searched for.*/
        void loadAlgorithms(QString theSearchDir);

        /** Save the algorithms in this set as individiual xml files 
         * rather than as a single file representing the set 
         * @param theSearchDir - the directory where xml parameters should
         * @param theUserProfilesFlag - whether to save only algs whose
         *        origin is OmgAlgorithm::USERPROFILE. Default is true!
         * be saved to.*/
        void saveAlgorithms(QString theSaveDir, bool theUserProfilesFlag=true);
        
        /** Get an algorithm from the set given its guid 
         * return OmgAlgorithm or 0 if not found */
        OmgAlgorithm getAlgorithm (QString theGuid);

        /** Find out if an algorithm of a given name exists in the alg set */
        bool contains(QString theName);
            
        //
        // Overloaded operators
        //

        /** Overloaded [] operator so that OmgAlgorithmSet can be treated 
         * like an array.
         * @param int thePosition - position of element to retrieve from the collection
         * @return OmgAlgorithm - the algorithm at this position
         */
        OmgAlgorithm operator [] (int);
        
    private:
        QString mName;
        QString mDescription;
        typedef QMap<QString,OmgAlgorithm> AlgorithmsMap;
        AlgorithmsMap mAlgorithmsMap;
};

#endif //OMGALGORITHMSET_H

