/***************************************************************************
 *   Copyright (C) 2006 by Tim Sutton   *
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

#ifndef OMGUI_H
#define OMGUI_H
#include "omgmodellerplugininterface.h"
#include "omgscraperplugininterface.h"
class QString;
class QColor;
class QDomDocument;
class QDomElement;
class OmgLocality;
class OmgLayerSet;
#include <QHash>
#include <QPair>
#include <QImage>
#include <QTime>
/** \ingroup library
 * \brief This is a helper class with mainly static methods for easily
 * obtaining the directories and paths relating to the application
 * for example the users settings dir.
 */
class OMG_LIB_EXPORT Omgui 
{
  public: 
    /**
     * Find the place on the filesystem where user data
     * are stored. 
     *
     * Typically this will be ~/.omgui
     *
     * @return QString containing the relevant directory name
     */
    static const QString userSettingsDirPath();
    /**
     * Find the place on the filesystem where user defined algorithm
     * profiles are stored. 
     *
     * Typically this will be ~/.omgui/algorithmProfiles
     *
     * @return QString containing the relevant directory name
     */
    static const QString userAlgorithmProfilesDirPath();
    
    /** Find the place in the file system where file selector
     * tree documents are cached. These are cached on a 
     * per modeller plugin basis, and in the case of web services
     * plugin, in subdirs based on the WS host name.
     * File selector tree documents are cached xml documents
     * representing the GIS raster files available for 
     * use in layersets.
     */
    static const QString fileSelectorCachePath();
    /**
     * Find the place on the filesystem where user defined layersets 
     * are stored. 
     *
     * Typically this will be ~/.omgui/layersets
     *
     * @return QString containing the relevant directory name
     */
    static const QString userLayersetDirPath();
    /** Get the path to where the plugins live. Usually this is not needed 
     * as the getModellerPlugin, getScraperPlugin etc factory methods below
     * do all this for you.
     */
    static const QString pluginDirPath();


    /** Get the place where model outputs are to be stored.
     * By default this is in ~/.omgui/modelOutputs
     * @TODO :: But if modelOutputsDir is specified in QSettings, it will override
     * the default.
     */
    static const QString getModelOutputDir();

    /** This typedef is used to refer to a collection of layersets.
     * the key is the layerset name
     * the value is the layerset itself
     */
    typedef QMap<QString,OmgLayerSet> LayerSetMap;
    /** Get a QMap of the avaliable layersets in the users layersets directory
     * @return a QMap<QString,OmgLayerSet> where the QString key is the layerset name 
     **/
    static Omgui::LayerSetMap getAvailableLayerSets();

    /** Sort a string list into descending alphabetic order
     *  and return the result.
     *  @param theList - the QStringList to be sorted
     *  @return QStringList - sorted in descending alphabetical order
     */
    static QStringList sortList(QStringList theList);

    /** Remove any duplucate entries from a sorted list
     *  @param theList - the QStringList to be sorted
     *  @return QStringList - a list with no sequential duplicates
     */
    static QStringList uniqueList(QStringList theList);

    /** Scan a directory looking for any valid gdal layers and return the 
     * result as an xml document complient with the openModeller WebServices
     * interface format for AvailableLayers
     * @param theBaseDir directory root from which to start scanning
     * @return a string containing an AvailableLayers xml document.
     */
    static QString getLayers(QString theBaseDir);

    /** Scan the users work directory looking for experiment files
     * and return the list of files found.
     * @return A QStringList of files found
     */
    static QStringList getExperimentsList();

    /** Scan the users work directory looking for pre-analysis files
     * and return the list of files found.
     * @return A QStringList of files found
     */
    static QStringList getPreAnalysisList();

    /** Return a hash containing the list of output image formats
     * supported by openModeller.
     * @return QHash<QString,QString> where the first string is a user
     * friendly name (i18n enabled with tr()) and the second string
     * is the format name expected by the openModeller library.
     */
    static QHash<QString,QString> getOutputFormats();
    /** Return a hash containing the list of min/max ranges for each
     * of the output formats supported by openModeller.
     * @return QHash<QString,QPair <double,double> > where the first string is a user
     * friendly name (i18n enabled with tr()) and the 
     * hash entry value is a pair containing the minimum and maximum
     * value for that file format (as implemented in the openModeller lib.
     */
    static QHash<QString,QPair<double,double> > getOutputFormatRanges();
    
    /** Return a hash containing the list of output image nodata values
     * for formats supported by openModeller.
     * @return QHash<QString,float> where the first string is a user
     * friendly name (i18n enabled with tr()) and the second 
     * is the nodata value assigend to rasters in this format when 
     * created by the openModeller library.
     */
    static QHash<QString,float> getOutputFormatNoData();

    /** Return a hash containing the list of output image formats
     * supported by openModeller.
     * @return QHash<QString,QString> where the first string is a user
     * friendly format  and the second string  is the description.
     */
    static QHash<QString,QString> getOutputFormatNotes();
    
    /** Get the file extension for the default image format.
     * @return A QString containing the 3 letter file extension
     */
    static QString getOutputFormatExtension();

    /** A helper method to easily write a file to disk.
     * @param theFileName - the filename to be created or overwritten
     * @param theData - the data that will be written into the file
     * @param theAppendFlag - (optional) append to existing file? Defaults to false.
     * @return bool - false if the file could not be written
     */
    static bool createTextFile(QString theFileName, QString theData, bool theAppendFlag = false);

    /** A helper method to easily read a file from disk.
     * @param theFileName - the filename to be read
     * @return QString - the file contents - empty if a problem occurred
     */
    static QString readTextFile(QString theFileName);

    /** A helper method to xml encode any ampersands in a string
     * & will become &amp; 
     * @param QString - the string to be properly encoded
     * @return A QString with the special chars properly encoded
     */
    static QString xmlEncodeAmpersands(QString theString);
    /** A helper method to xml encode any special chars in a string
     * (< > & etc) will become (&lt; &gt; &amp; etc)
     * @param QString - the string to be properly encoded
     * @return A QString with the special chars properly encoded
     */
    static QString xmlEncode(QString theString);

    /** A helper method to xml encode any special chars in a string.
     * (< > & etc) will become (&lt; &gt; &amp; etc)
     * @param QString - the string to be properly encoded - passed by reference
     */
    static void xmlEncodeInPlace(QString &theString);

    /** A helper method to xml deencode any special chars in a string
     * (&lt; &gt; &amp; etc) will become (< > & etc)
     * @param QString - the string to be properly decoded
     * @return A QString with the encoded chars properly decoded
     */
    static QString xmlDecode(QString theString);

    /** A helper method to xml deencode any special chars in a string
     * (&lt; &gt; &amp; etc) will become (< > & etc)
     * @param QString - the string to be properly decoded - passed by reference
     */
    static void xmlDecodeInPlace(QString &theString);

    /** A helper method to get a random color
     * @return a QColor
     */
    static QColor randomColour();
    /** A helper to get the standard style sheet for reports etc.
     * @return QString containing the default application stylesheet.
     */
    static QString defaultStyleSheet();
    /** Add Standard Html header
     * @return QString containing the default html header
     */
    static QString getHtmlHeader();
    /** Add Standard Html footer
     * @return QString containing the default html footer
     */
    static QString getHtmlFooter();
    /**
     * Delete a shapefile and its associated shx, dbf etc
     * @param theFileBase - the shapefile name e.g. /tmp/foo.shp
     * @return true if file did not exist or delete completed ok
     *         otherwise false if any problem occurred.
     */
    static bool deleteShapeFile(QString theFileBase);
    /** Convert a localities vector OmgLocalities into a shapefile.
     * @param theShapefileName - name of the shapefile to save to -
     *         it will be deleted if it exists and we have perms to delete.
     * @param theLocalities - pass by reference an OmgLocalities object.
     * @return true on success.
     */
    static bool localitiesToShapefile(QString theShapefileName,OmgLocalities const &theLocalities); 
    /** Get the directory containing the qt .qm translation files. 
     * @return QString - the absolute path to the i18n dir.
     */
    static QString i18nPath();
    /** Get a string list of the suportted locales (translations)
     * for this application */
    static QStringList i18nList();

     /** An enumerated type for describing formating options when printing
      * time out.
      */
     enum TimeForm {longTimeForm,shortTimeForm} ;

    /** Get the version string for this application. e.g.
     * openModeller Desktop 1.0.7 (r2312)
     * @return a QString containing internationaliased version string.
     */
    static QString version();

    static void createThumbnail(
                           QString theSourceImageFileName,
                           QString theOutputFileName,
                           QString theTopLabel,
                           QString theBottomLabel,
                           QString theBottomLabel2="" //optional
                           );
    
    /** Load a vector layer into the map based on the occurrences in the model.
     * @param theModel - pointer to an OmgModel instance that will have 
     *                   its occurrences loaded.
     * @return bool - true on success, false of there was a problem
     */
    static bool loadModelOccurrencesLayer( OmgModel * theModel );

    /** A helper method to rewrite seconds as a string in the form
     * 2 hours 20 minutes 12 seconds. If a given unit (e.g. mins) is
     * zero that part wil be left out e.g.
     * 2 hours 12 seconds
     * @note this is a template function in order to support both
     * int and long being passed to it.
     * @note For template functions the implemementation and declaration
     * must be in the same file to successfully compile and link. 
     * @see http://www.cplusplus.com/doc/tutorial/templates.html for 
     * mode info (near the bottom of the page)
     * @param theSeconds - the seconds you want to convert to string
     * @param theForm - a TimeForm enum indicating if we should 
     *      print in short form of
     *      2h 20m 12s 
     *      or in the long form of
     *      2 hours 20 minutes 12 seconds
     *      Defaults to short form
     * @return QString - a string representation of the seconds
     */
    template <class T> 
      static QString secondsToString(T theSeconds, Omgui::TimeForm theForm=Omgui::shortTimeForm)
    {
      QString myString;
      //division and modulus must be done with the same type as the template class 
      //otherwise a warning is generated
      int mySecsPerHour = 3600;
      int mySecsPerMinute = 60;
      int myHours = static_cast<int> (theSeconds / mySecsPerHour);
      int myRemainder = static_cast<int> (theSeconds % mySecsPerHour);
      int myMinutes = static_cast<int> (myRemainder / mySecsPerMinute);
      int mySeconds = static_cast<int> (myRemainder % mySecsPerMinute);
      if (theForm == Omgui::shortTimeForm)
      { //short form
        if (myHours > 0)
        {
          myString += QString::number(myHours) + "h ";
        }
        if (myMinutes > 0)
        {
          myString += QString::number(myMinutes) + "min ";
        }
        if (mySeconds > 0)
        {
          myString += QString::number(mySeconds) + "s ";
        }
      }
      else
      { //long form
        if (myHours > 0)
        {
          myString += QString::number(myHours) + " " + QObject::tr("Hours") + " ";
        }
        if (myMinutes > 0)
        {
          myString += QString::number(myMinutes) + " " + QObject::tr("Minutes") + " ";
        }
        if (mySeconds > 0)
        {
          myString += QString::number(mySeconds) + " " + QObject::tr("Seconds") + " ";
        }

      }
      return myString;
    }

  private:
    /** A helper function used my getLayers to scan a directory looking for valid
     * gdal filea.
     * @note This is a recursive function!
     * @param theDirName root directory to scan in
     * @param theDocument document to which layers will be added
     * @param theParentElement parent node to which found layers should be added
     */
    static void traverseDirectories(const QString theDirName, QDomDocument &theDocument, QDomElement &theParentElement);

};
  

#endif
