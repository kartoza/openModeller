/******************************************************
  climatedataprocessor.h  -  description
  -------------------
begin                : Thu May 15 2003
copyright            : (C) 2003 by Tim Sutton
email                : t.sutton@reading.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef OMGCLIMATEDATAPROCESSORCONTROLLER_H
#define OMGCLIMATEDATAPROCESSORCONTROLLER_H

#include "omgclimatedataprocessor.h"
#include "omgfilewriter.h"
#include "omgclimatefilereader.h"
#include "omgclimatefilegroup.h"
#include <QMap>
#include <QString>
#include <QObject>

/** \ingroup library
* This struct is simple container used in the 'run' method.
* @todo Remove this if possible
*/
struct OMG_LIB_EXPORT  OmgFileWriterStruct
{
    /** A filewriter pointer */
    OmgFileWriter * fileWriter;
    /** The fill path and file name of the file refenced */
    QString fullFileName;
};


/** \ingroup library
* \brief The OmgClimateDataProcessorController calculates specific climate variables using
 *DataProcessor functions.
 *@author Tim Sutton
 */

class OMG_LIB_EXPORT  OmgClimateDataProcessorController : public QObject {
    Q_OBJECT;
    public:
        /** Default constructor */
        OmgClimateDataProcessorController();

        /*
        OmgClimateDataProcessorController(
                int theFileStartYear,
                int theJobStartYear,
                int theJobEndYear,
                QString theInputFileTypeString,
                QString theOutputFileTypeString
                );
        */
        /** Destructor */
        ~OmgClimateDataProcessorController();

        // Getters and setters

        /** Mutator for filename of the meanTemp calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setMeanTempFileName ( QString theFileName);
        /** Accessor for filename of the meanTemp calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getMeanTempFileName ();

        /** Mutator for filename of the minTemp calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setMinTempFileName ( QString theFileName);
        /** Accessor for filename of the minTemp calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getMinTempFileName ();

        /** Mutator for filename of the maxTemp calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setMaxTempFileName ( QString theFileName);
        /** Accessor for filename of the maxTemp calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getMaxTempFileName ();

        /** Mutator for filename of the diurnalTemp calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setDiurnalTempFileName ( QString theFileName);
        /** Accessor for filename of the diurnalTemp calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getDiurnalTempFileName ();

        /** Mutator for filename of the mean precipitation calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setMeanPrecipFileName ( QString theFileName);
        /** Accessor for filename of the mean precipitation calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getMeanPrecipFileName ();

        /** Mutator for filename of the frost days calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setFrostDaysFileName ( QString theFileName);
        /** Accessor for filename of the frost days calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getFrostDaysFileName ();

        /** Mutator for filename of the solar radiation calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setTotalSolarRadFileName ( QString theFileName);
        /** Accessor for filename of the solar radiation calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getTotalSolarRadFileName ();

        /** Mutator for filename of the wind speed calculation inputs.
        * If the files are in series, the name should be of the first file in the series.
        * @param theFileName - The new filename for the calculation inputs.
        * @return void - No return.
        */
        void setWindSpeedFileName ( QString theFileName);
        /** Accessor for filename of the wind speed calculation inputs.
        * @return a QString containing the filename (which will be the first file
        * in the series if the files are in series).
        */
        const QString  getWindSpeedFileName ();

        /** Mutator for directory name for the calculation output files.
        * @param theFilePathString - The name of an existing directory where the
        * output files will be stored.
        * @return void - No return.
        */
        void setOutputFilePathString( QString theFilePathString);
        /** Accessor for the file output path.
        * @return a QString containing the directory name.
        */
        const QString getOutputFilePathString();

        /**
        * Mutator for int fileStartYear.
        * @param theYear - The year of the first 12 blocks of data
        * in the file (or the 12 files if the files are in series).
        * @return void - No return.
        * @note If the files are in series, jobStartYear should be the same as
        * fileStartYear and jobEndYear!
        */
        void setFileStartYear( const int theYear);
        /** Accessor for int fileStartYear. */
        const int getFileStartYear();

        /**
        * Mutator for int setJobStartYear.
        * @param theYear - The first year in the input file(s) to be read.
        * @return void - No return.
        * @note If the files are in series, jobStartYear should be the same as
        * fileStartYear and jobEndYear!
        */
        void setJobStartYear( const int theYear);
        /**
        * Accessor for int jobStartYear.
        * @return int - the current value of the first year in the input
        * files to be read.
        */
        const int getJobStartYear();

        /**
        * Mutator for int jobEndYear.
        * @param theYear - The last year in the input file(s) to be read.
        * @return void - No return
        * @note If the files are in series, jobStartYear should be the same as
        * fileStartYear and jobEndYear!
        */
        void setJobEndYear( const int theYear);
        /**
        * Accessor for int jobEndYear.
        * @return int - the current value of the last year in the input
        * files to be read.
        */
        const int getJobEndYear();

        /**
        * Mutator forOmgClimateFileReader::FileType inputFileType.
        * @param theInputFileType - The input filetype as specified inOmgClimateFileReader::FileType
        * @return void - No return
        */
        void setInputFileType( const OmgClimateFileReader::FileType theInputFileType);
        /**
        * Mutator forOmgClimateFileReader::FileType inputFileType.
        * This is an overloaded version of above that takes a string and looks up the enum.
        * @param theInputFileTypeString - The input filetype as a string. Valid options being:
        * CRES African climate data
        * ESRI & ASCII raster
        * Hadley Centre HadCM3 SRES Scenario
        * Hadley Centre HadCM3 IS92a Scenario
        * IPCC Observed Climatology
        * University of Reading Palaeoclimate data
        * Max Planck Institute fur Meteorologie (MPIfM) ECHAM4 data
        * CSIRO-Mk2 Model data
        * National Center for Atmospheric Research (NCAR) NCAR-CSM and NCAR-PCM data
        * Geophysical Fluid Dynamics Laboratory (GFDL) R30 Model data
        * Canadian Center for Climate Modelling and Analysis (CCCma) CGCM2 Model data
        * CCSR/NIES AGCM model data and CCSR OGCM model data
        * @return void - No return
        */
        void setInputFileType( const QString theInputFileTypeString);
        /**
        * Accessor forOmgClimateFileReader::FileType inputFileType.
        * @return aOmgClimateFileReader::FileType indicating the current input file type.
        */
        const OmgClimateFileReader::FileType getInputFileType();

        /**
        * Mutator for OmgFileWriter::FileType outputFileType.
        * This will determine how outputs from calculations will be written to disk.
        * @param theOutputFileType - a  OmgFileWriter::FileType value
        * @return void - No return
        */
        void setOutputFileType( const OmgFileWriter::FileType theOutputFileType);
        /**
        * Mutator for OmgFileWriter::FileType outputFileType.
        * Overloaded version of above that takes a string and looks up the enum.
        * @param theOutputFileTypeString - a QString containing the desired output file type.
        * Valid options include:
        * CSM for Matlab
        * CSM for Octave
        * Desktop GARP
        * ESRI ASCII Grid
        * Plain matrix with no header
        * @return void - No return
        */
        void setOutputFileType( const QString theOutputFileTypeString);
        /**
        * Accessor for OmgFileWriter::FileType outputFileType.
        * @return OmgFileWriter::FileType - the current output file type
        */
        const OmgFileWriter::FileType getOutputFileType();


        /**
        * Set up the filegroups for each filename that has been registered
        * @return bool - A boolean indicating success or failure of the operation
        */
        bool makeFileGroups();

        /**  Build a list of which calculations can be performed given the input files
         *    that have been registered. The boolean field indicates whether the user actually
         *    want to perform this calculation
         *    @see addUserCalculation */
        bool  makeAvailableCalculationsMap();
        /**
        * Accessor for the list of available calculations.
        * @return QMap<QString, bool>  - wherethe string is the name of the calculation and
        * bool indicates true if the user has asked for the calculation to be carried out,
        * false if he hasnt.
        */
        QMap <QString, bool > getAvailableCalculationsMap();

        /**
        * Add a calculation to the list of those requested to be carried out by the user.
        * The available availableCalculationsMap will be searched for a string match, and
        * if a match is found that map entry will be tagged as true - 'please calculate'
        * if it is currently false.
        * @param theCalculationName - the name of the calculation to be enabled.
        * @return bool - Returns false if no matching calculation name is found.
         */
        bool addUserCalculation(QString theCalculationName);

        /**
        * Start the data analysis process.
        * When everything else is set up, this is the method to call!
        * @todo If there ever was a good place to optimise performance,
        * this is it!For starters this method can be refactored so that the
        * vector from each active filegroup is retrieved first, and then
        * passed to any calculation that needs it, looping through all the filegroups
        * simultaneously. This will prevent multiple reads of the same file for
        * different calculations. RTFS for more info.
        * @return bool - A boolean indicating success or failure of the operation.
        */
        bool run();

        /**
        * This is a helper method that will return a Description of the OmgClimateDataProcessorController vars.
        * This will indicate things like whether the files are in series or not, what calculations
        * are available in the available calculations map, and whether they are tagged true for
        * execution or not.
        * @return QString - containing the summary description for this climatedataprocessor.
        */
        QString getDescription();

        /**
        * Mutator for bool filesInSeriesFlag.
        * When files are in series, it means that the file format only contains one months
        * data block per file and that there should be 12 such files to provide monthly
        * data for an entire year. The files should be suffixed with the month numer they
        * represent e.g.
        * meanTemp01.asc
        * meanTemp02.asc
        * meanTemp03.asc
        * meanTemp04.asc
        * meanTemp05.asc
        * meanTemp06.asc
        * meanTemp07.asc
        * meanTemp08.asc
        * meanTemp09.asc
        * meanTemp10.asc
        * meanTemp11.asc
        * meanTemp12.asc
        * @param theFlag - a flag indicating true if files are in series, otherwise false.
        * @return void - No return.
        */
        void setFilesInSeriesFlag( const bool theFlag);
        /**
        * Accessor for bool filesInSeriesFlag.
        * @see setFilesInSeriesFlag
        * @return bool - true if files are in series.
        */
        const bool getFilesInSeriesFlag();

        /**
        * If the output format requires a header (e.g. Arc/Info ASCII grid, you can
        * define one using this method.
        * @param theOutputHeader - a QString containing the new header.
        * @return void - No return.
        */
        void setOutputHeader( const QString& theOutputHeader);
        /**
        * Accessor for QString outputHeader.
        * @return QString - the currently set file header.
        */
        const QString getOutputHeader();


    signals:
        /**
        * A signal emitted to notify listeners how many variables
        * are going to be calculated for each years data.
        *@param theNumber - The total number of variables
        *@return void - No return
        */
        void numberOfVariablesToCalc(int theNumber);
        /**
        * A signal emitted to notify listeners how many cells
        * will be passed through in each block.
        *@param theNumber - The total number of cells in any block
        *@return void - No return
        */
        void numberOfCellsToCalc(int theNumber);
        /**
        * A signal emitted to notify listeners that we are about to
        * start calculating a variable for one years data.
        *@param theName - A String containing the variable name e.g.
        *                        'Precipitation over coolest month'
        *@return void - No return
        */
        void variableStart(QString theName);
        /**
        * A signal emitted to notify listeners that we
        * completed calculating the given variable.
        *@param theFileName - the filename that the variable was outputted to
        *@return void - No return
        */
        void variableDone(QString theFileName);
        /**
        * A signal emitted to notify listeners that we
        * have completed calculating a given cell.
        *@param theResult - The calculated value for a cell
        *@return void - No return
        */
        void cellDone(float theResult);

    private:

        // Private methods

        /** This method is intended for debugging purposes only */
        void printVectorAndResult(QVector<float> theVector, float theResult);

        /** Set up an individual file group (called by makefileGroups for
         *   each filegroup that needs to be initialised) */
        OmgClimateFileGroup *initialiseFileGroup(QString theFileName,int theStartYear);

        /**This is a private method. It is a simple method to populate the
         * inputFileTypeMap attribute - this will usually be called by the
         * constructor(s). All keys (file type strings) will be  stored in upper case.*/
        bool makeInputFileTypeMap();

        /**This is a private method. It is a simple method to populate the
         * outputFileTypeMap attribute - this will usually be called by the
         * constructor(s). All keys (file type strings) will be  stored in upper case.*/
        bool makeOutputFileTypeMap();

        /** Little utility method to convert from int to string */
        QString intToString(int theInt);

        // Private attributes
        /** The directory where the processed results will be stored. */
        QString outputFilePath;
        /** The type of input files to be processed by the climate date processor. */
        OmgClimateFileReader::FileType inputFileType;

        /** The type of output files to be produced by the climate date processor. */
        OmgFileWriter::FileType outputFileType;

        /** This is a map (associative array) that stores the key/value pairs
         * for the INPUT filetype. The key is the verbose name for the file type
         * (as will typically appear in the user interface, and the value
         * is theOmgClimateFileReader::FileType equivalent.
         * @see makeInputFileTypeMap()
         * @see makeOutputFileTypeMap()
         */
        QMap <QString,OmgClimateFileReader::FileType > inputFileTypeMap;


        /** This is a map (associative array) that stores the key/value pairs
         * for the OUTPUT filetype. The key is the verbose name for the file type
         * (as will typically appear in the user interface, and the value
         * is the OmgFileWriter::FileType equivalent.
         * @see makeInputFileTypeMap()
         * @see makeOutputFileTypeMap()
         */
        QMap <QString, OmgFileWriter::FileType > outputFileTypeMap;

        /** This is a map (associative array) that stores which calculations can be performed
         *   given the input files that have been registered with this climatedataprocessor.
         *   The boolean flag will be used to indicate whether the user actually wants to
         *   perform the calculation on the input dataset(s).
         *   @see makeAvailableCalculationsMap
         *   @see addUserCalculation
         */

        QMap <QString, bool > availableCalculationsMap;


        /** A filegroup containing files with mean temperature data. */
        OmgClimateFileGroup * meanTempFileGroup;
        /** The file name that contains mean temp data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString meanTempFileName;

        /** A filegroup containing files with minimum temperature data. */
        OmgClimateFileGroup *minTempFileGroup;
        /** The file name that contains min temp data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString minTempFileName;

        /** A filegroup containing files with maximum temperature data. */
        OmgClimateFileGroup * maxTempFileGroup;
        /** The file name that contains max temp data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString maxTempFileName;

        /** A filegroup containing files with diurnal temperature data. */
        OmgClimateFileGroup * diurnalTempFileGroup;
        /** The file name that contains diurnal temp data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString diurnalTempFileName;

        /** A filegroup containing files with mean precipitation data. */
        OmgClimateFileGroup * meanPrecipFileGroup;
        /** The file name that contains mean precipitation data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString meanPrecipFileName;

        /** A filegroup containing files with number of frost days data. */
        OmgClimateFileGroup * frostDaysFileGroup;
        /** The file name that contains mean frost data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString frostDaysFileName;

        /** A filegroup containing files with solar radiation data. */
        OmgClimateFileGroup * totalSolarRadFileGroup;
        /** The file name that contains mean solar radiation data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString totalSolarRadFileName;

        /** A filegroup containing files with wind speed data. */
        OmgClimateFileGroup * windSpeedFileGroup;
        /** The file name that contains wind speed data. If the file type is
        * one where the data is stored in series (a single file per month),
        * this member will store the name of the first file in the series. */
        QString windSpeedFileName;

        /** For certain input types (notably cres, arcinfo and Reading paleoclimate),
         * each months data is stored in a discrete file. Files should be numbered
         * e.g. meantemp01.asc, meantemp2.asc...meantemp12.asc for each month.
         * This flag lets us know whether data is in a series of seperate files for each month
         * or can all be found in the same file. */
        bool filesInSeriesFlag;

        /** This is a standard header (e.g. arc/info header) that will be appended to any output grids. */
        QString outputHeader;
};

#endif //CLIMATEDATAPROCESSORCONTROLLER
/*
   bool meanPrecipOverDriestQ
   bool meanTempOverWarmestQ
   bool meanPrecipOverWettestQ
   bool meanTempOverCoolestM
   bool lowestTempOverCoolestM
   bool meanPrecipOverDriestM
   bool meanTempOverWarmestM
   bool highestTempOverWarmestM
   bool meanPrecipOverWettestM
   bool meanTemp
   bool meanPrecip
   bool meanDiurnal
   bool meanFrostDays
   bool meanRadiation
   bool meanWindSpeed
   bool stdevMeanTemp
   bool stdevMeanPrecip
   bool meanPrecipOverCoolestM
   bool meanDiurnalOverCoolestM
   bool meanRadiationOverCoolestM
   bool meanRadiationOverDriestM
   bool meanPrecipOverWarmestM
   bool meanDiurnalOverWarmestM
   bool meanRadiationOverWarmestM
   bool meanRadiationOverWettestM
   bool meanPrecipOverCoolestQ
   bool meanRadiationOverCoolestQ
   bool meanRadiationOverDriestQ
   bool meanPrecipOverWarmestQ
   bool meanRadiationOverWarmestQ
   bool meanRadiationOverWettestQ
   bool annualTempRange
   bool meanTempOverFrostFreeM
   bool meanPrecipOverFrostFreeM
   bool monthCountAboveFreezing
   */
