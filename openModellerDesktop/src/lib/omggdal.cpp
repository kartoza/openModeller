/**************************************************************************
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

#include "omggdal.h"

#include <QFileInfo>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QPen>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//gdal includes
#include <gdal_priv.h>
#include <cpl_string.h>
#include <gdal.h>
#include <gdal_alg.h>
#include <cpl_conv.h>
#include <gdalwarper.h>

//ogr includes (used e.g. for contouring output)
#include <cpl_conv.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>

// QGIS includes
#include <qgsrasterlayer.h> 
#include <qgsrasterbandstats.h> 
#include <qgsmaplayerregistry.h> 
#include <qgsapplication.h>
#include <qgsmaprenderer.h> 
//
// global callback function
//
int CPL_STDCALL progressCallback( double dfComplete, const char * pszMessage,
                      void * pProgressArg)
{
  static double dfLastComplete = -1.0;

  OmgGdal * mypOmgGdal = (OmgGdal *) pProgressArg;

  if( dfLastComplete > dfComplete )
  {
    if( dfLastComplete >= 1.0 )
      dfLastComplete = -1.0;
    else
      dfLastComplete = dfComplete;
  }

  if( floor(dfLastComplete*10) != floor(dfComplete*10) )
  {
    int    nPercent = (int) floor(dfComplete*100);

    if( nPercent == 0 && pszMessage != NULL )
    {
      //fprintf( stdout, "%s:", pszMessage );
    }

    if( nPercent == 100 )
    {
      //fprintf( stdout, "%d - done.\n", (int) floor(dfComplete*100) );
      mypOmgGdal->showProgress(100,100);
    }
    else
    {
      int myProgress = (int) floor(dfComplete*100);
      //fprintf( stdout, "%d.", myProgress);
      mypOmgGdal->showProgress(myProgress,100);
      //fflush( stdout );
    }
  }
  else if( floor(dfLastComplete*30) != floor(dfComplete*30) )
  {
    //fprintf( stdout, "." );
    //fflush( stdout );
  }

  dfLastComplete = dfComplete;

  return TRUE;
}




OmgGdal::OmgGdal() : QObject()
{
  //qDebug ("OmgGdal ctor called");
  GDALAllRegister();
  OGRRegisterAll();
}

OmgGdal::~OmgGdal()
{}


const QString  OmgGdal::getWorldFile(const QString theFileName)
{
  GDALDataset  *gdalDataset = (GDALDataset *) GDALOpen( theFileName.toLocal8Bit(), GA_ReadOnly );
  if ( gdalDataset == NULL )
  {
    return QString("");
  }
  //get the geotransform stuff from gdal
  double myTransform[6];
  if (gdalDataset->GetGeoTransform(myTransform) != CE_None)
  {
    std::cout << "Failed to get geo transform from GDAL, aborting" << std::endl;
    GDALClose(gdalDataset);
    return QString("");
  }
  else
  {
    GDALClose(gdalDataset);
  }
  QString myHeader;
  myHeader += "Pixel XDim " + QString::number(myTransform[1]) + "\r\n";
  myHeader += "Rot 0 \r\n";
  myHeader += "Rot 0 \r\n";
  myHeader += "Pixel YDim " + QString::number(myTransform[5]) + "\r\n";
  myHeader += "Origin X   " + QString::number(myTransform[0]) + "\r\n";
  myHeader += "Origin Y   " + QString::number(myTransform[3]) + "\r\n";
  return myHeader;
}

const QString  OmgGdal::getAsciiHeader(const QString theFileName)
{
  GDALDataset  *gdalDataset = (GDALDataset *) GDALOpen( theFileName.toLocal8Bit(), GA_ReadOnly );
  if ( gdalDataset == NULL )
  {
    return QString("");
  }

  //get dimesnions and no data value
  int myColsInt = gdalDataset->GetRasterXSize();
  int myRowsInt = gdalDataset->GetRasterYSize();
  double myNullValue=gdalDataset->GetRasterBand(1)->GetNoDataValue();
  //get the geotransform stuff from gdal
  double myTransform[6];
  if (gdalDataset->GetGeoTransform(myTransform) != CE_None)
  {
    std::cout << "Failed to get geo transform from GDAL, aborting" << std::endl;
    GDALClose(gdalDataset);
    return QString("");
  }
  else
  {
    GDALClose(gdalDataset);
  }

  QString myHeader;

  myHeader =  "NCOLS "        + QString::number(myColsInt) + "\r\n";
  myHeader += "NROWS "        + QString::number(myRowsInt) + "\r\n";
  float myYTop = myTransform[3];
  float myXLeft = myTransform[0];
  float myCellHeight = myTransform[5];
  float myAbsCellHeight = fabs(myCellHeight);
  float myHeight = myAbsCellHeight * myRowsInt;
  float myYBottom = myYTop - myHeight;
  //qDebug("YTop: " + QString::number(myYTop).toLocal8Bit());
  //qDebug("XLeft: " + QString::number(myXLeft).toLocal8Bit());
  //qDebug("CellHeight: " + QString::number(myCellHeight).toLocal8Bit());
  //qDebug("RowCount: " + QString::number(myRowsInt).toLocal8Bit());
  //qDebug("YBottom = YTop - (fabs(CellHeight) * fabs(RowsCount))");
  //qDebug("YBottom: " + QString::number(myYBottom).toLocal8Bit());
  myHeader += "XLLCORNER "    + QString::number(myXLeft) +  "\r\n";;
  myHeader += "YLLCORNER "   + QString::number(myYBottom) +  "\r\n";
  myHeader += "CELLSIZE "     + QString::number(myTransform[1]) +  "\r\n";
  myHeader += "NODATA_VALUE " + QString::number(myNullValue) +  "\r\n";

  return myHeader;
}


/*
Builds the list of file filter strings to later be used by
QgisApp::addRasterLayer()
 
We query GDAL for a list of supported raster formats; we then build
a list of file filter strings from that list.  We return a string
that contains this list that is suitable for use in a a
QFileDialog::getOpenFileNames() call.
 
This method was filched from QGIS QgsRasterLayer class and tweaked a bit
*/
void OmgGdal::buildSupportedRasterFileFilter(QString & theFileFiltersString)
{
  // first get the GDAL driver manager

  GDALDriverManager *myGdalDriverManager = GetGDALDriverManager();

  if (!myGdalDriverManager)
  {
    std::cerr << "unable to get GDALDriverManager\n";
    return;                   // XXX good place to throw exception if we
  }                           // XXX decide to do exceptions

  // then iterate through all of the supported drivers, adding the
  // corresponding file filter

  GDALDriver *myGdalDriver;           // current driver

  char **myGdalDriverMetadata;        // driver metadata strings

  QString myGdalDriverLongName("");   // long name for the given driver
  QString myGdalDriverExtension("");  // file name extension for given driver
  QString myGdalDriverDescription;    // QString wrapper of GDAL driver description

  QStringList metadataTokens;   // essentially the metadata string delimited by '='

  QString catchallFilter;       // for Any file(*.*), but also for those
  // drivers with no specific file
  // filter

  // Grind through all the drivers and their respective metadata.
  // We'll add a file filter for those drivers that have a file
  // extension defined for them; the others, welll, even though
  // theoreticaly we can open those files because there exists a
  // driver for them, the user will have to use the "All Files" to
  // open datasets with no explicitly defined file name extension.
  // Note that file name extension strings are of the form
  // "DMD_EXTENSION=.*".  We'll also store the long name of the
  // driver, which will be found in DMD_LONGNAME, which will have the
  // same form.

  for (int i = 0; i < myGdalDriverManager->GetDriverCount(); ++i)
  {
    myGdalDriver = myGdalDriverManager->GetDriver(i);

    Q_CHECK_PTR(myGdalDriver);

    if (!myGdalDriver)
    {
      qWarning("unable to get driver %d", i);
      continue;
    }
    // now we need to see if the driver is for something currently
    // supported; if not, we give it a miss for the next driver

    myGdalDriverDescription = myGdalDriver->GetDescription();

    // std::cerr << "got driver string " << myGdalDriver->GetDescription() << "\n";

    myGdalDriverMetadata = myGdalDriver->GetMetadata();

    // presumably we know we've run out of metadta if either the
    // address is 0, or the first character is null
    while (myGdalDriverMetadata && '\0' != myGdalDriverMetadata[0])
    {
      metadataTokens = QString(*myGdalDriverMetadata).split("=");
      // std::cerr << "\t" << *myGdalDriverMetadata << "\n";

      // XXX add check for malformed metadataTokens

      // Note that it's oddly possible for there to be a
      // DMD_EXTENSION with no corresponding defined extension
      // string; so we check that there're more than two tokens.

      if (metadataTokens.count() > 1)
      {
        if ("DMD_EXTENSION" == metadataTokens[0])
        {
          myGdalDriverExtension = metadataTokens[1];

        }
        else if ("DMD_LONGNAME" == metadataTokens[0])
        {
          myGdalDriverLongName = metadataTokens[1];

          // remove any superfluous (.*) strings at the end as
          // they'll confuse QFileDialog::getOpenFileNames()

          myGdalDriverLongName.remove(QRegExp("\\(.*\\)$"));
        }
      }
      // if we have both the file name extension and the long name,
      // then we've all the information we need for the current
      // driver; therefore emit a file filter string and move to
      // the next driver
      if (!(myGdalDriverExtension.isEmpty() || myGdalDriverLongName.isEmpty()))
      {
        // XXX add check for SDTS; in that case we want (*CATD.DDF)
        QString glob = "*." + myGdalDriverExtension;
        theFileFiltersString += myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ");;";

        break;            // ... to next driver, if any.
      }

      ++myGdalDriverMetadata;

    }                       // each metadata item

    if (myGdalDriverExtension.isEmpty() && !myGdalDriverLongName.isEmpty())
    {
      // Then what we have here is a driver with no corresponding
      // file extension; e.g., GRASS.  In which case we append the
      // string to the "catch-all" which will match all file types.
      // (I.e., "*.*") We use the driver description intead of the
      // long time to prevent the catch-all line from getting too
      // large.

      // ... OTOH, there are some drivers with missing
      // DMD_EXTENSION; so let's check for them here and handle
      // them appropriately

      // USGS DEMs use "*.dem"
      if (myGdalDriverDescription.startsWith("USGSDEM"))
      {
        QString glob = "*.dem";
        theFileFiltersString += myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ");;";
      }
      else if (myGdalDriverDescription.startsWith("DTED"))
      {
        // DTED use "*.dt0"
        QString glob = "*.dt0";
        theFileFiltersString += myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ");;";
      }
      else if (myGdalDriverDescription.startsWith("MrSID"))
      {
        // MrSID use "*.sid"
        QString glob = "*.sid";
        theFileFiltersString += myGdalDriverLongName + " (" + glob.toLower() + " " + glob.toUpper() + ");;";
      }
      else
      {
        catchallFilter += QString(myGdalDriver->GetDescription()) + " ";
      }
    }

    // A number of drivers support JPEG 2000. Add it in for those.
    if (  myGdalDriverDescription.startsWith("MrSID")
          || myGdalDriverDescription.startsWith("ECW")
          || myGdalDriverDescription.startsWith("JPEG2000")
          || myGdalDriverDescription.startsWith("JP2KAK") )
    {
      QString glob = "*.jp2 *.j2k";
      theFileFiltersString += "JPEG 2000 (" + glob.toLower() + " " + glob.toUpper() + ");;";
    }

    // A number of drivers support JPEG 2000. Add it in for those.
    if (  myGdalDriverDescription.startsWith("MrSID")
          || myGdalDriverDescription.startsWith("ECW")
          || myGdalDriverDescription.startsWith("JPEG2000")
          || myGdalDriverDescription.startsWith("JP2KAK") )
    {
      QString glob = "*.jp2 *.j2k";
      theFileFiltersString += "JPEG 2000 (" + glob.toLower() + " " + glob.toUpper() + ");;";
    }

    myGdalDriverExtension = myGdalDriverLongName = "";  // reset for next driver

  }                           // each loaded GDAL driver

  // can't forget the default case
  theFileFiltersString += catchallFilter + "All other files (*)";
#ifdef QGISDEBUG
  std::cout << "Raster filter list built: " << theFileFiltersString.local8Bit() << std::endl;
#endif
}  // buildSupportedRasterFileFilter

const QString OmgGdal::contour(const QString theInputFile)
{
  QString myOutputFileDir = "/tmp";
  QString myOutputFileName = "contour";
  GDALDatasetH hSrcDS;
  int  b3D = FALSE, bNoDataSet = FALSE;
  int myBandNumber = 1;
  double dfInterval = 1.0;
  double  dfNoData = 0.0;
  double dfOffset = 0.0;

  const char *pszDstFilename = myOutputFileDir.toLocal8Bit();
  const char *pszElevAttrib = NULL;
  QString myFormat = "ESRI Shapefile";
  double adfFixedLevels[1000];
  int    nFixedLevelCount = 0;

  //      Open source raster file.

  GDALRasterBandH hBand;

  hSrcDS = GDALOpen( theInputFile.toLocal8Bit(), GA_ReadOnly );
  if( hSrcDS == NULL )
  {
    emit error ("Unable to open source file");
  }

  hBand = GDALGetRasterBand( hSrcDS, myBandNumber );
  if( hBand == NULL )
  {
    CPLError( CE_Failure, CPLE_AppDefined,
              "Band %d does not exist on dataset.",
              myBandNumber );
  }
  dfNoData = GDALGetRasterNoDataValue( hBand, &bNoDataSet );
  //     Try to get a coordinate system from the raster.

  OGRSpatialReferenceH hSRS = NULL;

  const char *pszWKT = GDALGetProjectionRef( hBand );

  if( pszWKT != NULL && strlen(pszWKT) != 0 )
    hSRS = OSRNewSpatialReference( pszWKT );

  //      Create the outputfile
  OGRDataSourceH hDS;
  OGRSFDriverH hDriver = OGRGetDriverByName( myFormat.toLocal8Bit() );
  OGRFieldDefnH hFld;
  OGRLayerH hLayer;
  int nElevField = -1;

  if( hDriver == NULL )
  {
    fprintf( stderr, "Unable to find format driver named %s.\n",
             myFormat.toLocal8Bit().data() );
    return QString();
  }

  hDS = OGR_Dr_CreateDataSource( hDriver, pszDstFilename, NULL );
  if( hDS == NULL )
  {
    return QString();
  }

  hLayer = OGR_DS_CreateLayer( hDS, myOutputFileName.toLocal8Bit(), hSRS,
                               b3D ? wkbLineString25D : wkbLineString,
                               NULL );
  if( hLayer == NULL )
  {
    return QString();
  }


  hFld = OGR_Fld_Create( "ID", OFTInteger );
  OGR_Fld_SetWidth( hFld, 8 );
  OGR_L_CreateField( hLayer, hFld, FALSE );
  OGR_Fld_Destroy( hFld );

  if( pszElevAttrib )
  {
    hFld = OGR_Fld_Create( pszElevAttrib, OFTReal );
    OGR_Fld_SetWidth( hFld, 12 );
    OGR_Fld_SetPrecision( hFld, 3 );
    OGR_L_CreateField( hLayer, hFld, FALSE );
    OGR_Fld_Destroy( hFld );
    nElevField = 1;
  }
  // -------------------------------------------------------------------- */
  //      Invoke.                                                         */
  // -------------------------------------------------------------------- */
  CPLErr eErr;

  eErr = GDALContourGenerate( hBand, dfInterval, dfOffset,
                              nFixedLevelCount, adfFixedLevels,
                              bNoDataSet, dfNoData,
                              hLayer, 0, nElevField,
                              GDALTermProgress, NULL );
  //progressCallback, NULL );

  OGR_DS_Destroy( hDS );
  GDALClose( hSrcDS );
  return myOutputFileDir + "/" + myOutputFileName;
}

const bool OmgGdal::convert(const QString theInputFile, const QString theOutputPath, const FileType theOutputFileType)
{
  bool myResult=false;
  //Compile output filename from input file and output path
  QFileInfo myFileInfo(theInputFile);
  QString myBaseString = theOutputPath +QString("/")+myFileInfo.baseName();  // excludes any extension

  QString myFileName;

  switch (theOutputFileType)
  {
    case GeoTiff :
      //compute outfile name
      myFileName = myBaseString+".tif";
      //qDebug("OmgGdal::convert - converting " + theInputFile.toLocal8Bit() 
      //    + " to " + myFileName.toLocal8Bit());
      myResult=gdal2Tiff (theInputFile, myFileName);
      break;
    case ArcInfoAscii :
      //compute outfile name
      myFileName = myBaseString+".asc";
      //qDebug("OmgGdal::convert - converting " + theInputFile.toLocal8Bit() 
      //    + " to " + myFileName.toLocal8Bit());
      myResult=gdal2Ascii (theInputFile, myFileName);
      break;
    default :
      emit error( tr("Output file type not supported!"));
      break;
  }
  return myResult;
}


//
// Private methods only below this point please!
//

const bool OmgGdal::gdal2gdal(const QString theFileName, 
    const QString theOutputFileName,
    const QString theFormat)
{
  //const QString myFormat = "GTiff";
  GDALDriver * mypDriver;
  char **papszMetadata;

  mypDriver = GetGDALDriverManager()->GetDriverByName(theFormat.toLocal8Bit());

  if( mypDriver == NULL )
  {
    emit error ("Unknown GDAL Driver!");
    return true;
  }

  papszMetadata = mypDriver->GetMetadata();
  if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
  {
    //printf( "Driver supports Create() method.\n" );
  }
  else
  {
    //printf( "Driver does not support Create() method.\n" );
  }
  if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATECOPY, FALSE ) )
  {
    //printf( "Driver supports CreateCopy() method.\n" );
  }
  else
  {
    //printf( "Driver does not support CreateCopy() method.\n" );
  }

  GDALDataset *mypSourceDataset =
    (GDALDataset *) GDALOpen( theFileName.toLocal8Bit(), GA_ReadOnly );
  GDALDataset *mypDestinationDataset;

  mypDestinationDataset = mypDriver->CreateCopy( theOutputFileName.toLocal8Bit(),
                          mypSourceDataset, FALSE,
						  NULL, progressCallback, this );
  if( mypDestinationDataset != NULL )
  {
    delete mypDestinationDataset;
  }
  return true;

}

const bool OmgGdal::gdal2Tiff(const QString theFileName, 
    const QString theOutputFileName)
{
   return gdal2gdal(theFileName, 
    theOutputFileName,
    "GTiff");
}
const bool OmgGdal::gdal2Ascii(const QString theFileName, 
    const QString theOutputFileName)
{
   return gdal2gdal(theFileName, 
    theOutputFileName,
    "AAIGrid");
}





void OmgGdal::showProgress (int theProgress,int theMaximum)
{
  emit updateProgress (theProgress,theMaximum);
}



bool OmgGdal::isValidGdalFile(const QString theFilename)
{
  //test for some common known not valid formats
  QStringList myList;
  myList << "prj" << "so" << "dylib" << "txt" << "xml" << "mea" << "exe" ;
  QFileInfo myFileInfo(theFilename);
  QString myExtension = myFileInfo.suffix();
  QStringListIterator myIterator(myList);
  while (myIterator.hasNext())
  {
    if (myIterator.next()==myExtension)
    {
      return false;
    }
  }
  if (theFilename.endsWith("~"))
  {
    return false;
  }
  //now lets try to load the file and see if gdal recognises
  
  GDALAllRegister();
  OGRRegisterAll();
  GDALDataset * myTestFile = (GDALDataset *)GDALOpen( theFilename.toLocal8Bit(), GA_ReadOnly );

  if( myTestFile == NULL )
  {
    //not GDAL compatible
    return false;
  }
  else
  {
    //is GDAL compatible
    GDALClose(myTestFile);
    return true;
  }
}

bool OmgGdal::isValidGdalProj(const QString theFilename)
{
  GDALAllRegister();
  OGRRegisterAll();
  //test whether the file has GDAL projection info
  GDALDataset * myTestFile = (GDALDataset *)GDALOpen( theFilename.toLocal8Bit(), GA_ReadOnly );

  QString myProjectionString = myTestFile->GetProjectionRef();

  if(myProjectionString.isEmpty())
  {
    //does not have projection info
    GDALClose(myTestFile);
    return false;
  }
  else
  {
    //does have projection info
    GDALClose(myTestFile);
    return true;
  }
}
/** Private method to calculate statistics for each band. Populates rasterStatsMap. */
void OmgGdal::calculateStats(BandStats * theBandStats,GDALDataset * gdalDataset)
{
  //std::cout << "Calculating statistics..." << std::endl;
  GDALRasterBand  *myGdalBand = gdalDataset->GetRasterBand( 1 );
  QString myColorInterpretation = GDALGetColorInterpretationName(myGdalBand->GetColorInterpretation());
  theBandStats->bandName=myColorInterpretation;
  theBandStats->bandNo=1;
  // get the dimensions of the raster
  int myColsInt = myGdalBand->GetXSize();
  int myRowsInt = myGdalBand->GetYSize();

  theBandStats->elementCountInt=myColsInt*myRowsInt;
  theBandStats->noDataDouble=myGdalBand->GetNoDataValue();
  //allocate a buffer to hold one row of ints
  int myAllocationSizeInt = sizeof(float)*myColsInt;
  float * myScanlineAllocInt = (float*) CPLMalloc(myAllocationSizeInt);
  bool myFirstIterationFlag = true;
  //unfortunately we need to make two passes through the data to calculate stddev
  for (int myCurrentRowInt=0; myCurrentRowInt < myRowsInt;myCurrentRowInt++)
  {
    CPLErr myResult = myGdalBand->RasterIO(
        GF_Read, 0, myCurrentRowInt, myColsInt, 1, 
        myScanlineAllocInt, myColsInt, 1, GDT_Float32, 0, 0 );
    if (myResult != CE_None)
    {
      emit error ("Unable to raster band");
      return;
    }
    for (int myCurrentColInt=0; myCurrentColInt < myColsInt; myCurrentColInt++)
    {
      //get the nth element from the current row
      double myDouble=myScanlineAllocInt[myCurrentColInt];
      //only use this element if we have a non null element
      if (myDouble != theBandStats->noDataDouble )
      {
        if (myFirstIterationFlag)
        {
          //this is the first iteration so initialise vars
          myFirstIterationFlag=false;
          theBandStats->minValDouble=myDouble;
          theBandStats->maxValDouble=myDouble;
        } //end of true part for first iteration check
        else
        {
          //this is done for all subsequent iterations
          if (myDouble < theBandStats->minValDouble)
          {
            theBandStats->minValDouble=myDouble;
          }
          if (myDouble > theBandStats->maxValDouble)
          {
            //	printf ("Maxval updated to %f\n",myDouble);
            theBandStats->maxValDouble=myDouble;
          }
          //only increment the running total if it is not a nodata value
          if (myDouble != theBandStats->noDataDouble)
          {
            theBandStats->sumDouble += myDouble;
            ++theBandStats->elementCountInt;
          }
        } //end of false part for first iteration check
      } //end of nodata chec
    } //end of column wise loop
  } //end of row wise loop
  //
  //end of first pass through data now calculate the range
  theBandStats->rangeDouble = theBandStats->maxValDouble-theBandStats->minValDouble;
  //calculate the mean
  theBandStats->meanDouble = theBandStats->sumDouble / theBandStats->elementCountInt;
  //for the second pass we will get the sum of the squares / mean
  for (int myCurrentRowInt=0; myCurrentRowInt < myRowsInt;myCurrentRowInt++)
  {
    CPLErr myResult = myGdalBand->RasterIO(
        GF_Read, 0, myCurrentRowInt, myColsInt, 1, myScanlineAllocInt, myColsInt, 1, GDT_Float32, 0, 0 );
    if (myResult != CE_None)
    {
      emit error ("Unable to raster band");
      return;
    }
    for (int myCurrentColInt=0; myCurrentColInt < myColsInt; myCurrentColInt++)
    {
      //get the nth element from the current row
      double myDouble=myScanlineAllocInt[myCurrentColInt];
      theBandStats->sumSqrDevDouble += static_cast<double>(pow(myDouble - theBandStats->meanDouble,2));
    } //end of column wise loop
  } //end of row wise loop
  //divide result by sample size - 1 and get square root to get stdev
  theBandStats->stdDevDouble = static_cast<double>(sqrt(theBandStats->sumSqrDevDouble /
        (theBandStats->elementCountInt - 1)));
  CPLFree(myScanlineAllocInt);
  //printf("CalculateStats::\n");
  //std::cout << "Band Name   : " << theBandStats->bandName << std::endl;
  //printf("Band No   : %i\n",theBandStats->bandNo);
  //printf("Band min  : %f\n",theBandStats->minValDouble);
  //printf("Band max  : %f\n",theBandStats->maxValDouble);
  //printf("Band range: %f\n",theBandStats->rangeDouble);
  //printf("Band mean : %f\n",theBandStats->meanDouble);
  //printf("Band sum : %f\n",theBandStats->sumDouble);
  return ;
}

void OmgGdal::writeImage(QString theInputFileString, 
    QString theOutputFileString,
    int theWidth,
    int theHeight)
{
  qDebug("Started with input :  " + theInputFileString.toLocal8Bit());
  qDebug("Started with output : " + theOutputFileString.toLocal8Bit());
  QFileInfo myRasterFileInfo ( theInputFileString );
  QgsRasterLayer *  mpRasterLayer = new QgsRasterLayer ( myRasterFileInfo.filePath(),
            myRasterFileInfo.completeBaseName() );
  mpRasterLayer->setDrawingStyle(QgsRasterLayer::SingleBandPseudoColor);
  mpRasterLayer->setColorShadingAlgorithm(QgsRasterLayer::PseudoColorShader);  
  mpRasterLayer->setContrastEnhancementAlgorithm(
      QgsContrastEnhancement::StretchToMinimumMaximum, false);
  mpRasterLayer->setMinimumValue(mpRasterLayer->grayBandName(),0.0, false);
  mpRasterLayer->setMaximumValue(mpRasterLayer->grayBandName(),10.0);
  // Now render layers onto a pixmap 
  QImage myImage( theWidth , theHeight , QImage::Format_RGB32 );
  mpRasterLayer->thumbnailAsImage( &myImage );
  myImage.save (theOutputFileString);
  return ;
}

const bool OmgGdal::makeLegend(const QString theOutputFileName, int theWidth)
{
  int myFinalWidth = theWidth;
  int myLegendWidth = 100;
  int myLegendHeight =  10 ;
  QImage myLegendImage(myLegendWidth, 1, QImage::Format_ARGB32);
  QPainter myQPainter; 

  //set up the three class breaks for pseudocolour mapping
  double myRangeSizeDouble = 90;  //hard coded for now
  double myBreakSizeDouble = myRangeSizeDouble / 3;
  double myClassBreakMin1 = 0;
  double myClassBreakMax1 = myClassBreakMin1 + myBreakSizeDouble;
  double myClassBreakMin2 = myClassBreakMax1;
  double myClassBreakMax2 = myClassBreakMin2 + myBreakSizeDouble;
  double myClassBreakMin3 = myClassBreakMax2;

  //
  // Create the legend 
  //
  myQPainter.begin(&myLegendImage);
  int myPosInt = 0;
  for (double myDouble = 0; myDouble < myRangeSizeDouble; myDouble += myRangeSizeDouble / 100.0)
  {
    //draw pseudocolor legend
    //check if we are in the first class break
    if ((myDouble >= myClassBreakMin1) && (myDouble < myClassBreakMax1))
    {
      int myRedInt = 0;
      int myBlueInt = 255;
      int myGreenInt = static_cast < int >(((255 / myRangeSizeDouble) * (myDouble - myClassBreakMin1)) * 3);
      myQPainter.setPen(QPen(QColor(myRedInt, myGreenInt, myBlueInt, QColor::Rgb), 0));
    }
    //check if we are in the second class break
    else if ((myDouble >= myClassBreakMin2) && (myDouble < myClassBreakMax2))
    {
      int myRedInt = static_cast < int >(((255 / myRangeSizeDouble) * ((myDouble - myClassBreakMin2) / 1)) * 3);
      int myBlueInt = static_cast < int >(255 - (((255 / myRangeSizeDouble) * ((myDouble - myClassBreakMin2) / 1)) * 3));
      int myGreenInt = 255;
      myQPainter.setPen(QPen(QColor(myRedInt, myGreenInt, myBlueInt, QColor::Rgb), 0));
    }
    //otherwise we must be in the third classbreak
    else
    {
      int myRedInt = 255;
      int myBlueInt = 0;
      int myGreenInt = static_cast < int >(255 - (((255 / myRangeSizeDouble) * ((myDouble - myClassBreakMin3) / 1) * 3)));
      myQPainter.setPen(QPen(QColor(myRedInt, myGreenInt, myBlueInt, QColor::Rgb), 0));
    }
    myQPainter.drawPoint(myPosInt++, 0);
  }


  myQPainter.end();




  //create a matrix to
  QMatrix myQWMatrix;
  //scale the raster legend up a bit 
  //note that scaling parameters are factors, not absolute values,
  // so scale (0.25,1) scales the painter to a quarter of its size in the x direction
  //TODO We need to decide how much to scale by later especially for rgb images which are only 3x1 pix
  //hard coding thes values for now.
  //assume 100px so scale by factor of 1.5 (=150px wide)
  //also we add font width to allow space for lettering overlap 
  float myScaleFactor = myFinalWidth / myLegendWidth ;
  myQWMatrix.scale(myScaleFactor, myLegendHeight);

#if WIN32  
  QFont myQFont("Verdana", 10, QFont::Normal);
#else
  QFont myQFont("Arial", 10, QFont::Normal);
#endif

  QFontMetrics myQFontMetrics(myQFont);
  //an amount in pixels to leave small amounts of whitespace between image and text parts
  int  myVerticalSpace = 5;
  int  myHorizontalSpace = 3;
  QImage myImage2( myFinalWidth  , 
                  myLegendHeight + myQFontMetrics.height() + (myVerticalSpace *2), 
                  QImage::Format_ARGB32);
  myImage2.fill(Qt::white);
  QPainter myPainter2(&myImage2);
  //draw the pseudocolors with the matrix appleid
  //in the process
  myPainter2.drawImage(0,0, myLegendImage.transformed(myQWMatrix));
  //
  // Overlay the layername
  //
  myPainter2.setPen(Qt::black);
  myPainter2.setFont(myQFont);
  myPainter2.setRenderHint(QPainter::TextAntialiasing,true);
  myPainter2.drawText(myHorizontalSpace, myLegendHeight + (myQFontMetrics.height() /2) + myVerticalSpace , tr("Low"));
  myPainter2.drawText(myFinalWidth - (myQFontMetrics.width("255")+myHorizontalSpace ), 
      myLegendHeight + (myQFontMetrics.height() /2)+ myVerticalSpace ,
      tr("High"));
  QString myString = tr("Medium Probability");
  myPainter2.drawText((myFinalWidth / 2) -( myQFontMetrics.width(myString) /2 ) , 
        myLegendHeight + (myQFontMetrics.height() /2) + myVerticalSpace , 
        myString);
  //
  // finish up
  //
  myPainter2.end();
  myImage2.save(theOutputFileName);
  return true;

}//end of makeLegend function

