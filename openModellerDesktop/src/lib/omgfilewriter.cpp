/***************************************************************************
  filewriter.cpp  -  description
  -------------------
begin                : Tue May 13 2003
copyright            : (C) 2003 by Tim Sutton
email                : t.sutton@reading.ac.uk
 
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "omgfilewriter.h"
#include <iostream>


OmgFileWriter::OmgFileWriter()

{
  mInputNoData=-9999.0;
  mOutputNoData=-9999.5;
}


OmgFileWriter::OmgFileWriter(QString theFileNameString, OmgFileWriter::FileType theFileFormat)
{
  //!@note currently file format is fixed to ascii grid - that will change soon
  qDebug("Writing to file format %i",theFileFormat);
  //replace any spaces in the filename with underscores and
  //suffix the correct filename extension
  QString myFileNameString=theFileNameString;
  bool endOfStringFlag=false;
  while(!endOfStringFlag)
  {
    int myInt =  myFileNameString.indexOf(" ");
    if(myInt != -1)    //-1 means no match found
    {
      myFileNameString.replace(myInt,1,"_");
    }
    else
    {
      endOfStringFlag=true;
    }
  }
  mFile.setFileName(theFileNameString);
  seperatorString=QString(" ");
  if (!mFile.open(QIODevice::WriteOnly))
  {
    //std::cout << "OmgFileWriter::Cannot open file : " << myFileNameString << std::endl;
    isWriteableFlag=false;
  }
  else
  {
    mTextStream.setDevice(&mFile);
    fileNameString = myFileNameString;
    //std::cout << "OmgFileWriter::Opened file ... " << fileNameString << " successfully." << std::endl;
    isWriteableFlag=true;
  }
}

OmgFileWriter::~OmgFileWriter()
{
}

bool OmgFileWriter::writeElement(float theElementFloat)
{
  // Translate input no data to desired output nodata. This was originally implemented for the
  // following reason:
  //
  // Right there is a bit of kludging going on here:
  // 1) if you send mInputNoData out the output stream, it truncates it to -9999
  // 2) if gdal reads -9999 as the first cell, it assumes dataset is int 16, causeing all values
  //    thereafter to be read as int16, losing any data after the decimal place
  // 3) using a decimal place wich isnt well represented by float32 will cause problems.
  //    we initially used -9999.99 as no data, but when gdal reads this from the asc file again
  //    it incorrectly receivese the value of -9999.8998433943 or similar. THis causes all the
  //    stats for the file to be incorrect. So now we use -9999.5 as default
  // 4) in dataprocessor, comparisons of no data are made between the input file data (which is -9999 usually)
  //    and the data processords idea of what no data shoud be. Consequently we need to rewrite nodata now.
  //
  if (theElementFloat==mInputNoData) { theElementFloat=mOutputNoData; }
  //if (mFile==0)
  //{
  //  return false;
  //}
  //if (mTextStream==0)
  //{
  // return false;
  //}
  //write the number to the file
  mTextStream << theElementFloat << seperatorString;
  //!@todo Add some error checking so we return a more meaningful value here...
  return true;
}

bool OmgFileWriter::writeNoData()
{
  mTextStream << mOutputNoData << seperatorString;
  //!@todo Add some error checking so we return a more meaningful value here...
  return true;
}

const QString OmgFileWriter::fileName()
{
  return fileNameString;
}

/*
 * This method sends a line break to the output file.
 */
bool OmgFileWriter::sendLineBreak()
{
  //cout << "OmgFileWriter::writeElement Writing element to   " << fileNameString << endl;
  mTextStream << QString("\n").toLocal8Bit();
  //!@todo Add some error checking so we return a more meaningful value here...
  return true;

}

void OmgFileWriter::close()
{
  mFile.close();
}

bool OmgFileWriter::writeString(QString theQString)
{
  //write the string to the file
  mTextStream << theQString;
  //!@todo Add some error checking so we return a more meaningful value here...
  return true;

}

/** Mutator for input no data value */
void OmgFileWriter::setInputNoData (float theValue)
{
  mInputNoData=theValue;
}

/** Mutator for output no data value */
void OmgFileWriter::setOutputNoData (float theValue)
{
  mOutputNoData=theValue;
}
