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

#ifndef OMGPREANALYSISALGORITHMSET_H
#define OMGPREANALYSISALGORITHMSET_H

class QString;
#include <QMap>
#include "omgpreanalysisalgorithm.h"
#include "omgserialisable.h"
/** \ingroup library
  * \brief An container class for algorithm definitions. It also has ability to self initialise
  * not yet using the active openModeller plugin, it's simply hardcoded
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgPreAnalysisAlgorithmSet  :  public OmgSerialisable
{
    public:
        /** Constructor . */
        OmgPreAnalysisAlgorithmSet();
        /** Desctructor . */
        ~OmgPreAnalysisAlgorithmSet();

        /** Add a pre-analysis algorithm to the algorithmset
         * @param theAlgorithm the alg to add to the set
         * @param theOverwriteFlag - whether to write any preexisting alg 
         *        with the same name (default is false)
         * @see OmgAlgorithm 
         */
        bool addAlgorithm(OmgPreAnalysisAlgorithm thePreAnalysisAlgorithm, bool theOverwriteFlag=false);
        /** Remove a pre-analysis algorithm from the algorithmset
         * @see OmgAlgorithm
         */
        bool removeAlgorithm(OmgPreAnalysisAlgorithm thePreAnalysisAlgorithm);
        /** Overloaded method to remove a pre-analysis algorithm from 
         * the algorithmset given only its algorithm Name.
         */
        bool removeAlgorithm(QString thePreAnalysisAlgorithm);

        /** Retrieve an algorithmset using the currently active plugin in 
         * qsettings. Now it does nothing and passes the work to loadAlgorithms. */
        static OmgPreAnalysisAlgorithmSet getFromActivePlugin();

        /** Append any algorithms that can be deserialised from the
         * data directory onto this algorithm set. Now it only loads them hardcoded.
         * @param theSearchDir - the directory where xml parameters should
         * be searched for.*/
        void loadAlgorithms(QString theSearchDir);
        //
        // Accessors
        //

        /** The name of this pre-analysis algorithmset - in plain english */
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
         * Clear all entries from the pre-analysis algorithm set
         */
        void clear();


        /**
         * Return the count of algorithms in the pre-analysis algorithm set - excluding the mask
         */
        int count() const;
        /** Return a string representation of this PreAnalysisAlgorithmSet
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

        /** Get an algorithm from the set given its guid 
         * return OmgAlgorithm or 0 if not found */
        OmgPreAnalysisAlgorithm getAlgorithm (QString theGuid);

        /** Get an algorithm from the set given its name 
         * return OmgAlgorithm or 0 if not found */
        OmgPreAnalysisAlgorithm getAlgorithmByName (QString name);

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
        OmgPreAnalysisAlgorithm operator [] (int);

    private:
        QString mName;
        QString mDescription;
        typedef QMap<QString,OmgPreAnalysisAlgorithm> AlgorithmsMap;
        AlgorithmsMap mAlgorithmsMap;
};

#endif //OMGALGORITHMSET_H

