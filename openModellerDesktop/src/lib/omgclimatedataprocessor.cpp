/***************************************************************************
                          dataprocessor.cpp  -  description
                             -------------------
    begin                : Wed Jan 8 2003
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

#include "omgclimatedataprocessor.h"
#include <iostream>
#include <math.h>
#include <cassert>

const int QUARTER = 3;
const float FREEZING_POINT=0;


OmgClimateDataProcessor::OmgClimateDataProcessor(float theNoDataVal)
{
  mNoData=theNoDataVal;
}

OmgClimateDataProcessor::~OmgClimateDataProcessor()
{}

float OmgClimateDataProcessor::meanOverLowestQ(QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::meanOverLowestQ () called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif

    float myLowestMeanFloat;
    float myCurrentMeanFloat;
    int myInt;
    int mySegmentInt;
    bool myFirstTimeFlag;

    myLowestMeanFloat = 0;
    myFirstTimeFlag = true;
    int myVectorLengthInt = theClimateVector.size();

    //Loop from first quarter - i.e. months 1, 2 and 3 to
    //last quarter - i.e. months 12, 1 and 2.
    for (myInt=0; myInt < theClimateVector.size(); myInt++)
    {
        myCurrentMeanFloat = 0;
#ifdef QGISDEBUG
        std::cout << "Processing element " <<    myInt << std::endl;
#endif
        //Check to see whether you are near end of vector and it is necessary to start reading
        //from the beginning again.
        if (myInt <= (myVectorLengthInt - (QUARTER)))
        {
            for (mySegmentInt=myInt ; mySegmentInt<=(myInt + QUARTER -1);mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myCurrentMeanFloat = myCurrentMeanFloat + myFloat;
#ifdef QGISDEBUG
                std::cout << "Processing element " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            myCurrentMeanFloat = myCurrentMeanFloat / QUARTER;
        }
        else
        {
            //Near end of vector so read last month(s)
            for (mySegmentInt = myInt; mySegmentInt < myVectorLengthInt ; mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myCurrentMeanFloat = myCurrentMeanFloat + myFloat;
#ifdef QGISDEBUG
                std::cout << "Wrap Processing element (from end) " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            //Read remainding months from beginning of vector
            for (mySegmentInt = 0; mySegmentInt<=(QUARTER - (myVectorLengthInt - myInt + 1));mySegmentInt++)
            {
                myCurrentMeanFloat = myCurrentMeanFloat + theClimateVector[mySegmentInt];
#ifdef QGISDEBUG
                std::cout << "Wrap Processing element (from start)" <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            myCurrentMeanFloat = myCurrentMeanFloat / QUARTER;
        }

        //Check whether value is lowest so far.If this is the first quarter always keep value.
        if (myFirstTimeFlag)
        {
            myLowestMeanFloat = myCurrentMeanFloat;
            myFirstTimeFlag = false;
        }
        else
        {
            //Test to see whether current value is the lowest so far.
            if (myLowestMeanFloat > myCurrentMeanFloat)
            {
                myLowestMeanFloat = myCurrentMeanFloat;
            }
        }

#ifdef QGISDEBUG
        std::cout << "Iteration " << myInt << " - current lowest mean over quarter is : " << myLowestMeanFloat << std::endl;
#endif
    }
#ifdef QGISDEBUG
    std::cout << "Completed - lowest mean over quarter is : " << myLowestMeanFloat << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" << std::endl;
#endif
    //Return lowest value over the quarter
    return myLowestMeanFloat;

}

float OmgClimateDataProcessor::meanOverHighestQ ( QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::meanOverHighestQ ( ) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif

    float myHighestMeanFloat;
    float myCurrentMeanFloat;
    int myInt;
    int mySegmentInt;
    bool myFirstTimeFlag;
    int myVectorLengthInt = theClimateVector.size();
#ifdef QGISDEBUG
    std::cout << "Vector length : " << myVectorLengthInt << std::endl;
#endif
    myHighestMeanFloat = 0;
    myFirstTimeFlag = true;

    //Loop from first quarter - i.e. months 1, 2 and 3 to
    //last quarter - i.e. months 12, 1 and 2.
    for (myInt=0; myInt<myVectorLengthInt; myInt++)
    {
        myCurrentMeanFloat = 0;
#ifdef QGISDEBUG
        std::cout << "Processing element " <<    myInt << std::endl;
#endif
        //Check to see whether you are near end of vector and it is necessary to start reading
        //from the beginning again.
        if (myInt <= (myVectorLengthInt - (QUARTER)))
        {
            for (mySegmentInt=myInt ; mySegmentInt<=(myInt + QUARTER -1);mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myCurrentMeanFloat = myCurrentMeanFloat + myFloat;
#ifdef QGISDEBUG
                std::cout << "Processing element " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            myCurrentMeanFloat = myCurrentMeanFloat / QUARTER;
        }
        else
        {
            //Near end of vector so read last month(s)
            for (mySegmentInt = myInt; mySegmentInt < myVectorLengthInt ; mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myCurrentMeanFloat = myCurrentMeanFloat + myFloat;
#ifdef QGISDEBUG
                std::cout << "Wrap Processing element (from end) " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            //Read remainding months from beginning of vector
            for (mySegmentInt = 0; mySegmentInt<=(QUARTER - (myVectorLengthInt - myInt + 1));mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myCurrentMeanFloat = myCurrentMeanFloat + myFloat;
#ifdef QGISDEBUG
                std::cout << "Wrap Processing element (from start)" <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
            }
            myCurrentMeanFloat = myCurrentMeanFloat / QUARTER;
        }

        //Check whether value is highest so far.If this is the first quarter always keep value.
        if (myFirstTimeFlag)
        {
            myHighestMeanFloat = myCurrentMeanFloat;
            myFirstTimeFlag = false;
        }
        else
        {
            //Test to see whether current value is the Highest so far.
            if (myHighestMeanFloat < myCurrentMeanFloat)
            {
                myHighestMeanFloat = myCurrentMeanFloat;
            }
        }

#ifdef QGISDEBUG
        std::cout << "Iteration " << myInt << " - current highest mean over quarter is : " << myHighestMeanFloat << std::endl;
#endif
    }
#ifdef QGISDEBUG
    std::cout << "Completed - highest mean over quarter is : " << myHighestMeanFloat << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" << std::endl;
#endif
    //Return Highest value over the quarter
    return myHighestMeanFloat;
}

int OmgClimateDataProcessor::firstMonthOfLowestQ (QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::firstMonthOfLowestQ ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif
    float myLowestFloat=0;
    float myCurrentFloat=0;
    int myInt=0;
    int mySegmentInt=0;
    bool myFirstTimeFlag=true;
    int myLowestMonthInt=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        //Loop from first quarter - i.e. months 1, 2 and 3 to
        //last quarter - i.e. months 12, 1 and 2.
        for (myInt = 0; myInt < myVectorLengthInt; myInt++)
        {
            myCurrentFloat = 0;
            //Check to see whether you are near end of vector and it is necessary to start reading
            //from the beginning again.

            if (myInt <= (myVectorLengthInt - (QUARTER)))
            {
                for (mySegmentInt=myInt ; mySegmentInt<=(myInt + QUARTER -1);mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;
#ifdef QGISDEBUG
                    std::cout << "Processing element " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
            }
            else
            {
                //Near end of vector so read last month(s)
                for (mySegmentInt = myInt; mySegmentInt < myVectorLengthInt; mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;
#ifdef QGISDEBUG
                    std::cout << "Wrap Processing element (from end) " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
                //Read remainding months from beginning of vector
                //for (mySegmentInt = 0; mySegmentInt < (QUARTER - (myVectorLengthInt - myInt) - 1); mySegmentInt++)
                for (mySegmentInt = 0; mySegmentInt <=(QUARTER - (myVectorLengthInt - myInt + 1));mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;
#ifdef QGISDEBUG
                    std::cout << "Wrap Processing element (from start)" <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
            }

            //Check whether value is lowest so far.  If this is the first quarter always keep value.
            if (myFirstTimeFlag == true)
            {
                myLowestMonthInt = myInt;
                myLowestFloat = myCurrentFloat;
                myFirstTimeFlag = false;
            }
            else
            {
                //Test to see whether current value is the lowest so far. If so store month number
                if (myCurrentFloat < myLowestFloat)
                {
                    myLowestMonthInt = myInt;
                    myLowestFloat = myCurrentFloat;
                }
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the firstMonthOfLowestQ method " << std::endl;
        myLowestMonthInt = mNoData;
#endif
    }


    //Return lowest value over the quarter (add 1 to get it into base 1 instead of base0)
    return myLowestMonthInt+1;


}
/** This method returns the month that starts the quarter with the highest
average values. For example, if 12 months were :

1   2   3   4   5   6   7   8   9   10  11  12
-----------------------------------------------
21  18  19  15  12  5   6   8   12  15  16  20

Then the return from this method would be 12 because
12,1 and 2 combined form the highest quarter. */
int OmgClimateDataProcessor::firstMonthOfHighestQ (QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::firstMonthOfHighestQ ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif

    float myHighestFloat=0;
    float myCurrentFloat=0;
    int myInt=0;
    int mySegmentInt=0;
    bool myFirstTimeFlag=true;
    int myHighestMonthInt=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        //Loop from first quarter - i.e. months 1, 2 and 3 to
        //last quarter - i.e. months 12, 1 and 2.
        for (myInt = 0; myInt < myVectorLengthInt; myInt++)
        {
            myCurrentFloat = 0;
            //Check to see whether you are near end of vector and it is necessary to start reading
            //from the beginning again.

            if (myInt <= (myVectorLengthInt - (QUARTER)))
            {
                for (mySegmentInt=myInt ; mySegmentInt<=(myInt + QUARTER -1);mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;
#ifdef QGISDEBUG
                    std::cout << "Processing element " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
            }
            else
            {
                //Near end of vector so read last month(s)
                for (mySegmentInt = myInt; mySegmentInt < myVectorLengthInt; mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;
#ifdef QGISDEBUG
                    std::cout << "Wrap Processing element (from end) " <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
                //Read remainding months from beginning of vector
                //for (mySegmentInt = 0; mySegmentInt < (QUARTER - (myVectorLengthInt - myInt) - 1); mySegmentInt++)
                for (mySegmentInt = 0; mySegmentInt <=(QUARTER - (myVectorLengthInt - myInt + 1));mySegmentInt++)
                {
                    float myFloat = theClimateVector[mySegmentInt];
                    if (mNoData==myFloat) { return static_cast<int>(mNoData); }
                    myCurrentFloat += myFloat;

#ifdef QGISDEBUG
                    std::cout << "Wrap Processing element (from start)" <<    myInt << ", value is " << theClimateVector[mySegmentInt] << std::endl;
#endif
                }
            }

            //Check whether value is highest so far.  If this is the first quarter always keep value.
            if (myFirstTimeFlag == true)
            {
                myHighestMonthInt = myInt;
                myHighestFloat = myCurrentFloat;
                myFirstTimeFlag = false;
            }
            else
            {
                //Test to see whether current value is the highest so far. If so store month number
                if (myCurrentFloat > myHighestFloat)
                {
                    myHighestMonthInt = myInt;
                    myHighestFloat = myCurrentFloat;
                }
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the firstMonthOfHighestQ method " << std::endl;
#endif
        myHighestMonthInt = static_cast<int>(mNoData);
    }


    //Return highest value over the quarter (add 1 to get it into base 1 instead of base0)
    return myHighestMonthInt+1;
}
/** This method will return the mean over three months in
theClimateVector, starting at theStartMonth. */
float OmgClimateDataProcessor::meanOverQuarter (QVector <float> theClimateVector, int theStartMonth)
{

#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::meanOverQuarter (float *theClimateVector, int myVectorLengthInt, int theStartMonth) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif


    int myInt=0;
    float myMeanFloat=0;
    int mySegmentInt=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {

        //Check to see whether months need to wrap
        if (theStartMonth + QUARTER < myVectorLengthInt)
        {
            //No wrapping necessary
            for (myInt = theStartMonth-1; myInt < (theStartMonth-1 + (QUARTER)); myInt++)
            {
                float myFloat = theClimateVector[myInt];
                if (mNoData==myFloat) { return mNoData; }
                myMeanFloat += myFloat;
#ifdef QGISDEBUG
                std::cout << "Added " << theClimateVector[myInt] << ", total is now : " << myMeanFloat << std::endl;
#endif
            }
        }
        else
        {
            //Wrapping necessary so read last month(s) as normal
            for (mySegmentInt=theStartMonth-1; mySegmentInt < myVectorLengthInt; mySegmentInt++)
            {
                float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myMeanFloat += myFloat;
#ifdef QGISDEBUG
                std::cout << "Wrap Added from end " << theClimateVector[mySegmentInt] << ", total is now : " << myMeanFloat << std::endl;
#endif
            }
            //Read remainding months from beginning of vector
            for (mySegmentInt = 0; mySegmentInt < (QUARTER - (myVectorLengthInt - theStartMonth) - 1); mySegmentInt++)
            {
               float myFloat = theClimateVector[mySegmentInt];
                if (mNoData==myFloat) { return mNoData; }
                myMeanFloat += myFloat;
#ifdef QGISDEBUG
                std::cout << "Wrap Added from start " << theClimateVector[mySegmentInt] << ", total is now : " << myMeanFloat << std::endl;
#endif
            }
        }
        myMeanFloat = (myMeanFloat / QUARTER);
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the meanOverQuarter method " << std::endl;
#endif
        myMeanFloat = mNoData;
    }
    return myMeanFloat;
}
/** Given an vector, this method will return the value of the smallest
element in the vector. */
float OmgClimateDataProcessor::lowestValue (QVector <float> theClimateVector)
{

#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::lowestValue ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif


    float myLowestFloat=0;
    int myInt=0;
    bool myFirstTimeFlag=true;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (myInt=0; myInt < myVectorLengthInt; myInt++)
        {
            //If this is the first run store value as lowest
            if (myFirstTimeFlag)
            {
                float myFloat = theClimateVector[myInt];
                if (mNoData==myFloat) { return mNoData; }
                myLowestFloat = myFloat;
#ifdef QGISDEBUG
                std::cout << "Lowest value set to " << myLowestFloat << " on first iteration." << std::endl;
#endif
                myFirstTimeFlag = false;

            }
            //Test to see whether value is lowest so far
            else
            {
                if (myLowestFloat > theClimateVector[myInt])
                {
                    myLowestFloat = theClimateVector[myInt];
                }
            }
        }
        //std::cout <<  "Lowest value was : " << myLowestFloat << std::endl;
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the lowestValue method " << std::endl;
#endif
        myLowestFloat = mNoData;
    }
    //Return lowest value
    return myLowestFloat;
}
/** Given an vector, this function will return the value of the largest element
in the vector. */
float OmgClimateDataProcessor::highestValue (QVector <float> theClimateVector)
{

#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::highestValue ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif


    float myHighestFloat=0;
    int myInt=0;
    bool myFirstTimeFlag=true;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (myInt=0; myInt < myVectorLengthInt; myInt++)
        {
            //If this is the first run store value as lowest
            if (myFirstTimeFlag == true)
            {
                float myFloat = theClimateVector[myInt];
                if (mNoData==myFloat) { return mNoData; }
                myHighestFloat = myFloat;
#ifdef QGISDEBUG
                std::cout << "Highest value set to " << myHighestFloat << " on first iteration." << std::endl;
#endif
                myFirstTimeFlag = false;

            }
            //Test to see whether value is highest so far
            else
            {
                float myFloat = theClimateVector[myInt];
                if (mNoData==myFloat) { return mNoData; }
                if (myHighestFloat < myFloat)
                {
                    myHighestFloat = myFloat;
                }
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the highest value method " << std::endl;
#endif
        myHighestFloat = mNoData;
    }
    //Return highest value
    return myHighestFloat;

}

/** Given two arrays (e.g. min temp and max temp) range will determine
the smallest and largest values that occur and return the difference.
The function is indescriminate as to whether the values are in the same
month or not.
@see greatestMonthlyRange
*/
float OmgClimateDataProcessor::greatestTotalRange (QVector <float> theClimateVector1,
        QVector <float> theClimateVector2)
{

#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::greatestTotalRange (float *theClimateVector1, int theArrayLength1, float *theClimateVector2, int theArrayLength2) called" << std::endl;
    std::cout << "==========================================================" << std::endl;
#endif
    //dont default to 0 as it may be lower (or higher) than any existing value in each vector
    float myHighestFloat=theClimateVector1[0]; //default to a valid value in the set
    float myLowestFloat=theClimateVector1[0];  //default to a valid value in the set
    float myRangeFloat=mNoData;
    int myInt=0;
    int myVectorLengthInt1 = theClimateVector1.size();
    int myVectorLengthInt2 = theClimateVector2.size();
    try
    {
        //process array1 first
        for (myInt = 0; myInt < myVectorLengthInt1; myInt++)
        {
            float myFloat = theClimateVector1[myInt];
            if (mNoData==myFloat) { return mNoData; }
            if (myHighestFloat < myFloat)
            {
                myHighestFloat = myFloat ;
            }
            if (myLowestFloat > myFloat)
            {
                myLowestFloat = myFloat;
            }
        }
        //now the second vector
        for (myInt = 0; myInt < myVectorLengthInt2; myInt++)
        {
            float myFloat = theClimateVector2[myInt];
            if (mNoData==myFloat) { return mNoData; }
            if (myHighestFloat < myFloat)
            {
                myHighestFloat = myFloat;
            }
            if (myLowestFloat > myFloat)
            {
                myLowestFloat = myFloat;
            }
        }
        myRangeFloat = myHighestFloat - myLowestFloat;
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the greatestTotalRange method " << std::endl;
#endif
        myRangeFloat = mNoData;

    }
    return myRangeFloat;
}
/** Given two arrays (e.g. min temp and max temp) range will determine
the smallest and largest values that occur and return the difference.
The value of the max-min difference for a given month that is the
greatest is returned.
@see greatestTotalRange */
float OmgClimateDataProcessor::greatestMonthlyRange (QVector <float> theClimateVector1,
        QVector <float> theClimateVector2)
{

  std::cout << "Vector 1 size: " << theClimateVector1.size();
  std::cout << "Vector 2 size: " << theClimateVector2.size();
  assert("Greatest monthly range is not implemented yet!");
    return 0;

}

bool OmgClimateDataProcessor::threshold(QVector <float> &theClimateVector, float theThreshold)
{
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (int myInt = 0; myInt < myVectorLengthInt; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) 
            { 
              continue; 
            }
            else if (theThreshold<=myFloat)
            {
              theClimateVector[myInt]=1;
            }
            else
            {
              theClimateVector[myInt]=0;
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the threshold method " << std::endl;
        return false;
#endif
    }
    return true;

}

float OmgClimateDataProcessor::threshold(float theFloat, float theThreshold)
{
    try
    {
            if (mNoData==theFloat) 
            { 
              return mNoData; 
            }
            else if (theThreshold<=theFloat)
            {
              return 1.0;
            }
            else
            {
              return 0.0;
            }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the threshold method " << std::endl;
        return mNoData;
#endif
    }
    return mNoData;
}

float OmgClimateDataProcessor::sum(QVector <float> theClimateVector)
{
    float myRunningTotFloat=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (int myInt = 0; myInt <= myVectorLengthInt-1; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) { return mNoData; }
            myRunningTotFloat += myFloat;
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the sum method " << std::endl;
#endif
        myRunningTotFloat = mNoData;
    }
    return myRunningTotFloat;
}


/** This function will return the sum of theClimateVector divided by the
number of elements in theClimateVector. */
float OmgClimateDataProcessor::mean (QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::meanOverYear ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==============================================================" << std::endl;
#endif

    float myRunningTotFloat=0;
    float myMeanOverYearFloat;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (int myInt = 0; myInt <= myVectorLengthInt-1; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) { return mNoData; }
            myRunningTotFloat += myFloat;
#ifdef QGISDEBUG
            std::cout << "Iteration " << myInt << ", running total is : " << myRunningTotFloat << std::endl;
#endif
        }
        myMeanOverYearFloat = myRunningTotFloat / myVectorLengthInt;
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the meanOverYear method " << std::endl;
#endif
        myMeanOverYearFloat = mNoData;
    }
#ifdef QGISDEBUG
    std::cout << "Completed - mean over year is : " << myMeanOverYearFloat << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" <<
     std::endl;
#endif
    return myMeanOverYearFloat;

}
/** This function will return the standard deviation of the climate vector. */
float OmgClimateDataProcessor::stddevOverYear (QVector <float> theClimateVector)
{

#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::stddevOverYear ( float *theClimateVector, int myVectorLengthInt) called" << std::endl;
    std::cout << "==============================================================" << std::endl;
#endif
    int myInt=0;
    float myRunningTotFloat=0;
    float myMeanOverYearFloat=0;
    float mySumSqdDevFloat=0;
    float myStddevOverYearFloat=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (myInt = 0; myInt <= myVectorLengthInt-1; myInt++)
        {
          float myFloat = theClimateVector[myInt];
          //if any element is null, cell is null!
          if (mNoData == myFloat) { return mNoData; }
          //assert (myFloat < 0);
          myRunningTotFloat += myFloat;
#ifdef QGISDEBUG
            std::cout << "Iteration " << myInt << ", running total is : " << myRunningTotFloat << std::endl;
#endif
        }
        myMeanOverYearFloat = myRunningTotFloat / myVectorLengthInt;

        //Calculate the sum of the squared deviations
        for (myInt = 0; myInt <  myVectorLengthInt; myInt++)
        {
            mySumSqdDevFloat += static_cast<float>(pow((theClimateVector[myInt] - myMeanOverYearFloat),2));
        }

        //divide result by sample size - 1 and get square root to get stdev
        myStddevOverYearFloat = static_cast<float>(sqrt(mySumSqdDevFloat / (myVectorLengthInt - 1)));
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the stddevOverYear method " << std::endl;
#endif
        myStddevOverYearFloat = mNoData;
    }
#ifdef QGISDEBUG
    std::cout << "Completed - stddev over year is : " << myMeanOverYearFloat << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" << std::endl;
#endif

    return myStddevOverYearFloat;
}
/** This function will return an integer between 1 and 12 corresponding to
the month with the lowest value. */
int OmgClimateDataProcessor::monthWithLowestValue(QVector <float> theClimateVector)
{
    int myLowestInt=0;     //assume first month is lowest until proven otherwise
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (int myInt = 0; myInt < myVectorLengthInt; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) { return static_cast<int>(mNoData); }
            //Test to see whether current value is the lowest so far.
            if (myFloat < theClimateVector[myLowestInt])
            {
                myLowestInt = myInt ;
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the monthWithLowestValue method " << std::endl;
#endif
        myLowestInt = 0;
    }
#ifdef QGISDEBUG
    std::cout << "Completed - month with lowest value is : " << myLowestInt << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" << std::endl;
#endif
    return myLowestInt+1;


}
/** This function will return an integer between 1 and 12 corresponding to
the month with the highest value. */
int OmgClimateDataProcessor::monthWithHighestValue (QVector <float> theClimateVector)
{

    int myHighestInt=0;      //assume first month is highest until proven otherwise
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        for (int myInt = 0; myInt < myVectorLengthInt; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) { return static_cast<int>(mNoData); }
            //Test to see whether current value is the lowest so far.
            if (myFloat > theClimateVector[myHighestInt])
            {
                myHighestInt = myInt ;
            }
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the monthWithHighestValue method " << std::endl;
#endif
        myHighestInt = 0;
    }
#ifdef QGISDEBUG
    std::cout << "Completed - month with highest value is : " << myHighestInt << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------" << std::endl;
#endif
    return myHighestInt+1;

}
/** This function will return the value of the element in theClimateVector
that corresponds to theMonth. */
float OmgClimateDataProcessor::valueGivenMonth (QVector <float> theClimateVector,int theMonth)
{
    float myValueFloat=0;
    int myVectorLengthInt = theClimateVector.size();
    try
    {
        if (theMonth < 1 || theMonth > 12 || myVectorLengthInt != 12)
            throw "Month out of bounds";
        myValueFloat = theClimateVector[theMonth-1];
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the valueGivenMonth function" << std::endl;
#endif
        myValueFloat = mNoData;
    }
    return myValueFloat;
}
/** This value will return the mean value (normally temperature or precipitation) of months in theClimateVector where the corresponding months in
theFrostArray have frost free days. The frost vector should contain the number of frost days
per month, so a value of 0 in the frost vector means there were no frost days that month. If there
are no frost free months, mNoData will be returned.*/
float OmgClimateDataProcessor::meanValueOverFrostFreeMonths (QVector<float> theFrostVector,
        QVector <float> theClimateVector)
{
#ifdef QGISDEBUG
    std::cout << "OmgClimateDataProcessor::meanValueOverFrostFreeMonths (float *theFrostArray, int theFrostArrayLength, float *theClimateVector, int theClimateVectorLength)" << std::endl;
    std::cout << "==============================================================" << std::endl;
#endif

    int myInt =0;
    float myRunningTotalFloat =0;
    float myMonthCountInt =0;
    float myMeanFloat=mNoData;
    int myFrostVectorLengthInt = theFrostVector.size();
    int myClimateVectorLengthInt = theClimateVector.size();

    try
    {
        //Check that the frost and climate arrays are the same size
        if (myFrostVectorLengthInt != myClimateVectorLengthInt)
        {
            throw ("Vector sizes must be equal!");
        }
        for (myInt = 0; myInt < myFrostVectorLengthInt; myInt++)
        {
            float myFloat = theClimateVector[myInt];
            if (mNoData==myFloat) { return mNoData; }
            //iterate through frost vector looking for frost free months
            if (myFloat == 0)
            {
                //If month is frost free add value to myRunningTotalDbl and add month to monthcounter
                myRunningTotalFloat += theClimateVector[myInt];
                myMonthCountInt += 1;
            }
        }

        //return average value
        if (myMonthCountInt > 0)
        {
            myMeanFloat = myRunningTotalFloat / myMonthCountInt;
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the meanValueOverFrostFreeMonths function" << std::endl;
#endif
        myMeanFloat = mNoData;
    }

    return myMeanFloat;
}




int OmgClimateDataProcessor::numberOfMonthsAboveZero (QVector <float> theClimateVector)
{

    int myInt=0;
    int myMonthCountInt=0;

    try
    {
        for (myInt=0; myInt < theClimateVector.size(); myInt++)
        {
            if (theClimateVector[myInt] == mNoData)
            {
                return static_cast<int>(mNoData);
            }
            if (theClimateVector[myInt] > FREEZING_POINT)
                myMonthCountInt += 1;
        }
    }
    catch (...)
    {
#ifdef QGISDEBUG
        std::cout << "A fatal error occured in the numberOfMonthsAboveZero function" << std::endl;
#endif
        myMonthCountInt = static_cast<int>(mNoData);
    }
    return myMonthCountInt;
}

