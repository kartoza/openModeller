/***************************************************************************
                          omggraph.cpp  -  description
                             -------------------
    begin                : April 2007
    copyright            : (C) 2007 by Tim Sutton
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

#include "omggraph.h"
#include "omgui.h"
#include <QListIterator>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QFontMetrics>
#include <QTime>
#include <cmath> //needed for fabs

OmgGraph::OmgGraph(QPainter * thepPainter)
{
  mpPainter = thepPainter;
  initialise();
}
//private - one of the above should be used
OmgGraph::OmgGraph()
{
}
OmgGraph::~OmgGraph()
{
}

void OmgGraph::initialise()
{
  mYAxisMax=0.0;
  mYAxisMin=0.0;
  mXAxisMin=0.0;
  mXAxisMax=0.0;
  mImageWidth = mpPainter->device()->width();
  mImageHeight = mpPainter->device()->height();
  mXGutterWidth=0;
  mYGutterWidth=0;
  mGradientWidth=mImageWidth;
  mGradientHeight=mImageHeight;
  mGridLinesFlag=true;
  mDrawDiagonal=false;
  mShowVerticesFlag=false;
  mShowVertexLabelsFlag=false;
  mSplinePointsFlag=false;
  mAreaFillFlag=false;
  mLegendHeight=0;
  QFont myFont("arial", 10, QFont::Normal);
  mAxisFont = myFont;
  mLegendFont = myFont;
  mVertexLabelFont = myFont;
}
 
void OmgGraph::setAxisFont(QFont theFont)
{
  mAxisFont=theFont;
}
void OmgGraph::setLegendFont(QFont theFont)
{
  mLegendFont=theFont;
}
void OmgGraph::setVertexLabelFont(QFont theFont)
{
  mVertexLabelFont=theFont;
}
void OmgGraph::setGridLinesEnabled(bool theFlag)
{
  mGridLinesFlag=theFlag;
}
bool OmgGraph::hasGridLinesEnabled()
{
  return mGridLinesFlag;
}

void OmgGraph::setDiagonalEnabled(bool theFlag)
{
  mDrawDiagonal=theFlag;
}
bool OmgGraph::hasDiagonalEnabled()
{
  return mDrawDiagonal;
}

void OmgGraph::setVerticesEnabled(bool theFlag)
{
  mShowVerticesFlag=theFlag;
}
bool OmgGraph::hasVerticesEnabled()
{
  return mShowVerticesFlag;
}
void OmgGraph::setVertexLabelsEnabled(bool theFlag)
{
  mShowVertexLabelsFlag=theFlag;
}
bool OmgGraph::hasVertexLabelsEnabled()
{
  return mShowVertexLabelsFlag;
}
void OmgGraph::setSpliningEnabled(bool theFlag)
{
  mSplinePointsFlag=theFlag;
}
bool OmgGraph::hasSpliningEnabled()
{
  return mSplinePointsFlag;
}
void OmgGraph::setAreaFillEnabled(bool theFlag)
{
  mAreaFillFlag=theFlag;
}
bool OmgGraph::hasAreaFillEnabled()
{
  return mAreaFillFlag;
}

void OmgGraph::clear(QColor theColour)
{
  mpPainter->fillRect( QRect(0,0,mImageWidth,mImageHeight), theColour );
}

void OmgGraph::addSeries(OmgDataSeries theSeries)
{
  mSeriesList.append(theSeries);
}
bool OmgGraph::removeSeriesAt(int theSeriesNo)
{
  if ((theSeriesNo < mSeriesList.count()) && (theSeriesNo >= 0))
  {
    mSeriesList.removeAt(theSeriesNo);
    return true;
  }
  else
  {
    return false;
  }
}
int OmgGraph::seriesCount() const
{
  return mSeriesList.count();
}

void OmgGraph::render( )
{
  if (mSeriesList.size() < 1)
  {
    qDebug("No graph drawn as no data series added");
    return;
  }
  //
  // First scan through to get max and min cell counts
  //
  bool myFirstItemFlag=true;
  QListIterator< OmgDataSeries > mySeriesIterator( mSeriesList );
  /*while (mySeriesIterator.hasNext()) 
  {
    OmgDataSeries mySeries = mySeriesIterator.next();
    //qDebug("Series xMin : " + QString::number(mySeries.xMin()).toLocal8Bit());
    //qDebug("Series xMax : " + QString::number(mySeries.xMax()).toLocal8Bit());
    //qDebug("Series yMin : " + QString::number(mySeries.yMin()).toLocal8Bit());
    //qDebug("Series yMax : " + QString::number(mySeries.yMax()).toLocal8Bit());
    if (mXAxisMax < mySeries.xMax() || myFirstItemFlag)
    {
      mXAxisMax = mySeries.xMax();
    }
    if (mXAxisMin > mySeries.xMin() || myFirstItemFlag)
    {
      mXAxisMin = mySeries.xMin();
    }
    if (mYAxisMax < mySeries.yMax() || myFirstItemFlag)
    {
      mYAxisMax = mySeries.yMax();
    }
    if (mYAxisMin > mySeries.yMin() || myFirstItemFlag)
    {
      mYAxisMin = mySeries.yMin();
		if(mYAxisMin == 1.0)
		{
			mYAxisMin = 0.0;
		}
    }
	myFirstItemFlag=false;
  }*/
  //qDebug("Graph xMin : " + QString::number(mXAxisMin).toLocal8Bit());
  //qDebug("Graph xMax : " + QString::number(mXAxisMax).toLocal8Bit());
  //qDebug("Graph yMin : " + QString::number(mYAxisMin).toLocal8Bit());
  //qDebug("Graph yMax : " + QString::number(mYAxisMax).toLocal8Bit());
  // rather than make the chart run right to the borders
  // of the graph area, we will add 1/100th space
  // at the top ...
  //mYAxisMax += (mYAxisMax - mYAxisMin)/100;

  mXAxisMin = 0.0;
  mYAxisMin = 0.0;
  mXAxisMax = 1.0;
  mYAxisMax = 1.0;

  //
  // Now calculate the graph drawable area after the axis labels have been taken
  // into account
  //
  calculateLegendHeight();
  calculateGutters();
  mGraphImageWidth = mImageWidth-mYGutterWidth; 
  mGraphImageHeight = mImageHeight-(mXGutterHeight+mLegendHeight); 
  QImage myImage(mGraphImageWidth,mGraphImageHeight,
      QImage::Format_ARGB32);
  QPainter myPainter(&myImage);
  myPainter.fillRect(QRectF(0,0,mGraphImageWidth,mGraphImageHeight),Qt::white);
  myPainter.setRenderHint(QPainter::Antialiasing);
  //
  // determine the scaling factor for x and y axis
  // to do this we use:
  //  ( image width / x max ) * x
  //  ( image height / y max ) * y
  //  ( 500 / 40 ) * 20 = 250
  //  ( 500 / 1000 ) * 800 = 400 

  float myXScaleFactor =  mGraphImageWidth / (mXAxisMax-mXAxisMin) ;
  float myYScaleFactor =  mGraphImageHeight / (mYAxisMax-mYAxisMin) ;
  
  QString myString = "\nX Scale Factor graph width / (xmax-xmin): " 
                   + QString::number(mGraphImageWidth) + "/"
                   + QString::number(mXAxisMax-mXAxisMin) + " = "
                   + QString::number(myXScaleFactor);
  myString        += "\nY Scale Factor graph height / (ymax - ymin): " 
                   + QString::number(mGraphImageHeight) + "/"
                   + QString::number(mYAxisMax-mYAxisMin) + " = "
                   + QString::number(myYScaleFactor);
  //qDebug(myString);


  
  //
  // now draw actual line graph
  //
  float myX=0.0;
  float myY=0.0;
  mySeriesIterator.toFront();
  while (mySeriesIterator.hasNext()) 
  {
    OmgDataSeries mySeries = mySeriesIterator.next();
    QPainterPath myPath;
    //make sure the line starts in the bottom left corner
    myX = 0;
    myY = 0;
    myY = mGraphImageHeight - myY;
    QPointF myLastPoint ( myX, myY );
    //this will be ignored / overridden in non fill mode
    myPath.moveTo( myLastPoint );
    //set the pen for the path outline
    QPen myPen;
    myPen.setWidth(3);
    myPen.setStyle(Qt::SolidLine);
    myPainter.setPen( myPen );
    //start iterating through the provided points
    //draw a line on the graph along the bar peaks; 
    int myXCounter = 0;
    for (int myCounter = 0; myCounter < mySeries.size(); ++ myCounter)
    {
      //qDebug("Getting x,y no " + QString::number(myCounter).toLocal8Bit() + " from series");
      myX = mySeries.xAt(myCounter);
      myY = mySeries.yAt(myCounter);
      // scale the values into the graph frame
      if (myX==mXAxisMin)
      {
        myX=0;
      }
      else
      {
        myX = ( myXScaleFactor * (myX - mXAxisMin) ) ;
      }
      if (myY==mYAxisMin)
      {
        myY=mGraphImageHeight;
      }
      else
      {
        myY = ( myYScaleFactor * (myY - mYAxisMin) ) ;
        myY = mGraphImageHeight - myY;
      }
      // store this point in our line 
      QPointF myPoint ( myX, myY );
      if (mSplinePointsFlag && myXCounter != 0) //dont bother splining first point
      {
        //control points for splining points into a curve
        float myControl1X=0.0;
        float myControl1Y=0.0;
        float myControl2X=0.0;
        float myControl2Y=0.0;
        const int XSTEEPNESS=2;
        const int YSTEEPNESS=2;
        if (myPoint.x() > myLastPoint.x())
        {
          myControl1X = myLastPoint.x();
          myControl2X = myLastPoint.x()+(fabs(myPoint.x()-myLastPoint.x())/XSTEEPNESS);
        }
        else
        {
          myControl1X = myLastPoint.x()-(fabs(myLastPoint.x()-myPoint.x())/XSTEEPNESS);
          myControl2X = myLastPoint.x();
        }
        if (myPoint.y() > myLastPoint.y())
        {
          myControl1Y = myPoint.y();
          myControl2Y = myLastPoint.y()+(fabs(myPoint.y()-myLastPoint.y())/YSTEEPNESS);
        }
        else
        {
          myControl1Y = myLastPoint.y()-(fabs(myLastPoint.y()-myPoint.y())/YSTEEPNESS);
          myControl2Y = myPoint.y();
        }
        if (myControl1Y < 0 ) myControl1Y=0;
        if (myControl2Y < 0 ) myControl2Y=0;
        if (myControl1Y > mGraphImageHeight) myControl1Y=mGraphImageHeight;
        if (myControl2Y > mGraphImageHeight) myControl2Y=mGraphImageHeight;

        if (myControl1X < 0) myControl1X=0;
        if (myControl2X < 0) myControl2X=0;
        if (myControl1X > mGraphImageWidth) myControl1X=mGraphImageWidth;
        if (myControl2X > mGraphImageWidth) myControl2X=mGraphImageWidth;

        QPointF myControlPoint1(myControl1X,myControl1Y);
        QPointF myControlPoint2(myControl2X,myControl2Y);
        myPainter.drawEllipse(static_cast<int>(myControl1X),static_cast<int>(myControl1Y),5,5);
        myPainter.drawText(static_cast<int>(myControl1X),static_cast<int>(myControl1Y),QString::number(myXCounter));
        myPainter.drawEllipse(static_cast<int>(myControl2X),static_cast<int>(myControl2Y),5,5);
        myPainter.drawText(static_cast<int>(myControl2X),static_cast<int>(myControl2Y),QString::number(myXCounter));
        //connect last point to this using spline curve
        myPath.cubicTo(myControlPoint1, myControlPoint2, myPoint);
        //myPath.cubicTo(myLastPoint, myPoint, myPoint);
      }
      else //just draw straight lines between vertices
      {
        if (myXCounter==0)
        {
          //just move to the point dont draw it in
          myPath.moveTo(myPoint);
        }
        else
        {
          myPath.lineTo(myPoint);
        }
      }
      myLastPoint = myPoint;
      ++myXCounter;
    }
    if (mAreaFillFlag)
    {
      //close of the point array so it makes a nice polygon
      //bottom right point
      myPath.lineTo( QPointF( 
            myLastPoint.x()  ,
            mImageHeight));
      //bottom left point
      myPath.lineTo( QPointF( 
            0, 
            mImageHeight));
      //set a gradient fill for the path
      QLinearGradient myGradient = customGradient(mySeries.fillColor());
      myPainter.setBrush(myGradient);
    }
    myPen.setColor(mySeries.lineColor());
    myPainter.setPen( myPen );
    //create the path and draw it
    myPainter.drawPath(myPath);

  }
  
  //
  // now paint in the vertices and vertex labels
  //
  mySeriesIterator.toFront();
  while (mySeriesIterator.hasNext()) 
  {
    OmgDataSeries mySeries = mySeriesIterator.next();
    QPen myPen;
    myPen.setWidth(5);
    myPen.setColor(mySeries.lineColor());
    myPainter.setPen( myPen );
    QFont myQFont("arial", 10, QFont::Normal);
    myPainter.setFont(myQFont);
    for (int myCounter = 0; myCounter < mySeries.size(); ++ myCounter)
    {
      myX = mySeries.xAt(myCounter);
      myY = mySeries.yAt(myCounter);
      QString myString = QString::number( myX  ) +
        " , " + QString::number( myY );
      //QString myString = "  "  + QString::number( myY );

      // scale the values into the graph frame
      if (myX!=mXAxisMin)
      {
        myX = ( myXScaleFactor * (myX - mXAxisMin) );
      }
      if (myY!=mYAxisMin)
      {
        myY = ( myYScaleFactor * (myY - mYAxisMin) ) ;
        myY = mGraphImageHeight - myY;
      }
      //
      // Uncomment this when debugging if you want
      // to see the scaled values printed on the chart
      //
      //myString += "\n   "  + QString::number( myX  ) +
      //  " , " + QString::number( myY );
      //qDebug(myString);
      if (mShowVerticesFlag)
      {
        myPen.setWidth(1);
        myPainter.drawEllipse(static_cast<int>(myX-2),static_cast<int>(myY-2),4,4);
      }
      if (mShowVertexLabelsFlag)
      {
        myPainter.setFont(mVertexLabelFont);
        QFontMetrics myFontMetrics( mVertexLabelFont );
        int myWidth = myFontMetrics.width(myString); //width of text
        int myHeight = myFontMetrics.height();
        myPen.setWidth(1);
        myPen.setColor(mySeries.lineColor());
        myPainter.setPen(myPen);
        myPainter.setBrush(mySeries.fillColor());
        //draw a rounded rect around the label the magic numbers below
        //just add a little padding so the label does not ride up
        //against the edge of the rect
        QPointF myTopLeftPoint =QPointF( myX + 3 , myY-(myHeight/2)-2);
        QPointF myBottomRightPoint = QPointF( myX+myWidth+10 , myY+(myHeight/2)+2 );
        myPainter.drawRoundRect ( QRectF(myTopLeftPoint,myBottomRightPoint),80,60 );
        //now paint the label itself
        myPen.setColor(Qt::black);
        myPainter.setPen(myPen);
        myPainter.drawText(QRectF(myTopLeftPoint,myBottomRightPoint),
               Qt::AlignCenter || Qt::AlignVCenter,myString);
      }
    }
  }
  //
  // Finish up
  //
  //
  // Draw the axes first otherwise if grid lines are
  // enabled they will overdraw labels which looks bad
  //
  drawAxes();
  if (mDrawDiagonal) 
  {
    drawDiagonal();
  }
  //draw the chart part onto the main image
  myPainter.end();
  mpPainter->drawImage(
      QPointF(mYGutterWidth,
              0
        ),myImage);
  //makeLegend();
}

QLinearGradient OmgGraph::redGradient()
{
  //define a gradient
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(0.0,QColor(242, 14, 25, 190));
  myGradient.setColorAt(0.7,QColor(175, 29, 37, 190));
  myGradient.setColorAt(1.0,QColor(114, 17, 22, 190));
  return myGradient;
}
QLinearGradient OmgGraph::greenGradient()
{
  //define a gradient 
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(0.0,QColor(48, 168, 5, 190));
  myGradient.setColorAt(0.7,QColor(36, 122, 4, 190));
  myGradient.setColorAt(1.0,QColor(21, 71, 2, 190));
  return myGradient;
}
QLinearGradient OmgGraph::blueGradient()
{
  //define a gradient 
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(0.0,QColor(30, 0, 106, 190));
  myGradient.setColorAt(0.7,QColor(30, 72, 128, 190));
  myGradient.setColorAt(1.0,QColor(30, 223, 196, 190));
  return myGradient;
}
QLinearGradient OmgGraph::grayGradient()
{
  //define a gradient 
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(0.0,QColor(5, 5, 5, 190));
  myGradient.setColorAt(0.7,QColor(122, 122, 122, 190));
  myGradient.setColorAt(1.0,QColor(220, 220, 220, 190));
  return myGradient;
}
QLinearGradient OmgGraph::randomGradient()
{
  //define a gradient 
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  QColor myColour = Omgui::randomColour();
  myGradient.setColorAt(0.0,myColour.lighter()); //lighter introduced in qt4.3
  myGradient.setColorAt(0.7,myColour);
  myGradient.setColorAt(1.0,myColour.darker()); //darker circa qt4.3
  return myGradient;
}
QLinearGradient OmgGraph::customGradient(QColor theColour)
{
  //if the user didnt set alpha transparency in the colour lets force some ok?
  if (theColour.alphaF()==1)
  {
    theColour.setAlphaF(0.6);
  }
  //define a gradient 
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(0.0,theColour.lighter()); //lighter introduced in qt4.3
  myGradient.setColorAt(0.7,theColour);
  myGradient.setColorAt(1.0,theColour.darker()); //darker circa qt4.3
  return myGradient;
}
QLinearGradient OmgGraph::highlightGradient()
{
  //define another gradient for the highlight
  QLinearGradient myGradient = QLinearGradient(mGradientWidth,0,mGradientWidth,mGradientHeight);
  myGradient.setColorAt(1.0,QColor(255, 255, 255, 50));
  myGradient.setColorAt(0.5,QColor(255, 255, 255, 100));
  myGradient.setColorAt(0.0,QColor(255, 255, 255, 150));
  return myGradient;
}

void OmgGraph::drawAxes( )
{
  //anti alias lines in the graph
  mpPainter->setRenderHint(QPainter::Antialiasing);
  //determine labels sizes and draw them
  mpPainter->setFont(mAxisFont);
  QString myYMaxLabel = QString::number(static_cast < unsigned int >(mYAxisMax));
  QString myXMinLabel = QString::number(mXAxisMin);
  QString myXMaxLabel = QString::number(mXAxisMax);

  //
  // Now draw interval markers on the x axis
  //
  int myXDivisions = mGraphImageWidth/10;
  mpPainter->setPen( Qt::gray );
  for (int i=0;i<myXDivisions;++i)
  {
    QPolygon myPolygon;
    QPoint myPosition((i*myXDivisions)+mYGutterWidth , mImageHeight-(mXGutterHeight+mLegendHeight));
    myPolygon << myPosition;
    myPolygon << QPoint((i*myXDivisions)+mYGutterWidth , mImageHeight-((mXGutterHeight+mLegendHeight)-5));
    myPolygon << myPosition;
    myPolygon << QPoint(((i+1)*myXDivisions)+mYGutterWidth , mImageHeight-(mXGutterHeight+mLegendHeight));
    mpPainter->drawPolyline(myPolygon);
    if (mGridLinesFlag)
    {
      QPoint myTopPosition((i*myXDivisions)+mYGutterWidth , 0 );
      mpPainter->drawLine(myPosition,myTopPosition);
    }
  }
  //
  // Now draw interval markers on the y axis
  //
  int myYDivisions = mGraphImageHeight/10;
  mpPainter->setPen( Qt::gray );
  int myYOrigin = mImageHeight-(mXGutterHeight+mLegendHeight);
  for (int i=myYDivisions;i>0;--i)
  {
    QPolygon myPolygon;
    QPoint myPosition(mYGutterWidth,myYOrigin-(i*myYDivisions ));
    myPolygon << myPosition;
    myPolygon << QPoint(mYGutterWidth-5,myYOrigin-(i*myYDivisions ));
    myPolygon << myPosition;
    myPolygon << QPoint(mYGutterWidth,myYOrigin-((i-1)*myYDivisions ));
    mpPainter->drawPolyline(myPolygon);
    if (mGridLinesFlag)
    {
      QPoint myRightPosition(mImageWidth,myYOrigin-(i*myYDivisions ));
      mpPainter->drawLine(myPosition,myRightPosition);
    }
  }


  //now draw the axis labels onto the graph
  QFontMetrics myMetrics(mAxisFont);
  mpPainter->setPen(Qt::black);
  mpPainter->drawText(1, myMetrics.height(), myYMaxLabel);
  mpPainter->drawText(1, mImageHeight-(mXGutterHeight+mLegendHeight), QString::number(static_cast < unsigned int >(mYAxisMin)));
  mpPainter->drawText(mYGutterWidth,mImageHeight-(mXGutterHeight+mLegendHeight-myMetrics.height()) , myXMinLabel);
  mpPainter->drawText(mImageWidth-mXGutterWidth,mImageHeight-(mXGutterHeight+mLegendHeight-myMetrics.height()), myXMaxLabel );
}

void OmgGraph::drawDiagonal()
{
  mpPainter->setPen( Qt::gray );
  QPoint myPoint1 (mYGutterWidth , mImageHeight-(mXGutterHeight+mLegendHeight));
  QPoint myPoint2 (mImageWidth , 0);
  mpPainter->drawLine (myPoint1 , myPoint2);
}

void OmgGraph::calculateGutters()
{
  //
  // Calculate the graph drawable area after the axis labels have been taken
  // into account
  //
  QFont myFont("arial", 10, QFont::Normal);
  QFontMetrics myFontMetrics( myFont );
  QString myYMaxLabel = QString::number(static_cast < unsigned int >(mYAxisMax));
  QString myXMinLabel = QString::number(mXAxisMin);
  QString myXMaxLabel = QString::number(mXAxisMax);
  //calculate the gutters
  if (myFontMetrics.width(myXMinLabel) < myFontMetrics.width(myYMaxLabel))
  {
    //add 2 so we can have 1 pix whitespace either side of label
    mYGutterWidth = myFontMetrics.width(myYMaxLabel )+2; 
  }
  else
  {
    //add 2 so we can have 1 pix whitespace either side of label
    mYGutterWidth = myFontMetrics.width(myXMinLabel )+2; 
  }
  mXGutterHeight = myFontMetrics.height()+2;
  //1 pix whtispace from right edge of image
  mXGutterWidth = myFontMetrics.width(myXMaxLabel)+1;
}

void OmgGraph::makeLegend()
{
  QPen myPen;
  myPen.setWidth(1);
  myPen.setStyle(Qt::SolidLine);
  mpPainter->setPen( myPen );
  //put the legend in a box
  //QPointF myTopLeftPoint ( 0.0 , mImageHeight-mLegendHeight);
  //QPointF myBottomRightPoint ( mImageWidth , mImageHeight );
  //last two params are amount of roundness of corners in x and y direction
  //mpPainter->drawRoundRect ( QRectF(myTopLeftPoint,myBottomRightPoint),80,60 );
  
  myPen.setWidth(1);
  int myLastXPos=20; // leave some space on the left gutter
  mpPainter->setFont(mLegendFont);
  const int myLeftSpace=20;
  const int myBoxWidth=20;
  const int myBoxToTextSpace=10;
  const int mySpaceAfterText=10;
  const int myYSpace=10;
  int myLastYPos=mImageHeight-(mLegendHeight-myYSpace);
  QFontMetrics myFontMetrics( mLegendFont );
  QListIterator< OmgDataSeries > mySeriesIterator( mSeriesList );
  while (mySeriesIterator.hasNext()) 
  {
    OmgDataSeries mySeries = mySeriesIterator.next();
    QString myLabel = mySeries.label();
    int myLabelAndMarkerWidth=0;
    myLabelAndMarkerWidth += myBoxWidth; 
    myLabelAndMarkerWidth += myBoxToTextSpace; //a little space between box and text
    myLabelAndMarkerWidth += myFontMetrics.width(myLabel); //width of text
    myLabelAndMarkerWidth += mySpaceAfterText; //a little space after the text and before the next box
    //we need to know if the label when added to the current
    //line will over run the right edge
    //if  it does we need to rather leave blank space to the end of the line and
    //add the label and its marker to the start of the next line.
    int myDistanceToEnd = mImageWidth - (myLastXPos % mImageWidth);
    if (myDistanceToEnd < myLabelAndMarkerWidth)
    {
      myLastXPos = myLeftSpace; // leave some whitespace on the left
      myLastYPos += myFontMetrics.height() + myYSpace;;
    }
    //draw the little legend box first
    QPointF myTopLeftPoint ( myLastXPos , myLastYPos);
    myLastXPos += myBoxWidth; 
    QPointF myBottomRightPoint ( myLastXPos , myLastYPos-myFontMetrics.height() );
    //last two params are amount of roundness of corners in x and y direction
    myPen.setColor(mySeries.lineColor());
    mpPainter->setBrush(mySeries.fillColor());
    mpPainter->drawRoundRect ( QRectF(myTopLeftPoint,myBottomRightPoint),80,60 );
    myLastXPos += myBoxToTextSpace; //a little space between box and text
    myTopLeftPoint =QPointF( myLastXPos , myLastYPos);
    myLastXPos += myFontMetrics.width(myLabel); //width of text
    myBottomRightPoint = QPointF( myLastXPos , myLastYPos-myFontMetrics.height() );
    myPen.setColor(Qt::black);
    mpPainter->drawText(QRectF(myTopLeftPoint,myBottomRightPoint),Qt::AlignVCenter,myLabel);
    myLastXPos += mySpaceAfterText; //a little space after the text and before the next box
    //first check we arent overrunning the space avaiable
    if (myLastXPos > mImageWidth)
    {
      myLastXPos = myLeftSpace; // leave some whitespace on the left
      myLastYPos += myFontMetrics.height() + myYSpace;;
    }
  }
  //QFont myQFont("arial", 15, QFont::Normal);
  //mpPainter->setFont(myQFont);
  //mpPainter->drawEllipse(myTopLeftPoint.x(),myTopLeftPoint.y(),12,12);
  //mpPainter->drawText(myTopLeftPoint,"TL");
  //mpPainter->drawEllipse(myBottomRightPoint.x()-12,myBottomRightPoint.y(),12,12);
  //mpPainter->drawText(myBottomRightPoint,"BR");
  //drawHighlight ( myTopLeftPoint, myBottomRightPoint );
}

void OmgGraph::calculateLegendHeight()
{
  const int myLeftSpace=20;
  const int myBoxWidth=20;
  const int myBoxToTextSpace=10;
  const int mySpaceAfterText=10;
  const int myYSpace=10;
  int myTotalWidth = myLeftSpace;
  QFontMetrics myFontMetrics( mLegendFont );
  QListIterator< OmgDataSeries > mySeriesIterator( mSeriesList );
  while (mySeriesIterator.hasNext()) 
  {
    OmgDataSeries mySeries = mySeriesIterator.next();
    QString myLabel = mySeries.label();
    int myLabelAndMarkerWidth=0;
    myLabelAndMarkerWidth += myBoxWidth; 
    myLabelAndMarkerWidth += myBoxToTextSpace; //a little space between box and text
    myLabelAndMarkerWidth += myFontMetrics.width(myLabel); //width of text
    myLabelAndMarkerWidth += mySpaceAfterText; //a little space after the text and before the next box
    //we need to know if the label when added to the current
    //line will over run the right edge
    //if  it does we need to rather leave blank space to the end of the line and
    //add the label and its marker to the start of the next line.
    int myDistanceToEnd = mImageWidth - (myTotalWidth % mImageWidth);
    if (myDistanceToEnd < myLabelAndMarkerWidth)
    {
      myTotalWidth += myDistanceToEnd;
    }
    myTotalWidth += myLabelAndMarkerWidth;
  }
  //qDebug("Total label width:" + QString::number(myTotalWidth).toLocal8Bit());
  //qDebug("Total image width:" + QString::number(mImageWidth).toLocal8Bit());
  int myTotalHeight = 0;
  if ( myTotalWidth > mImageWidth )
  {
    if (myTotalWidth % mImageWidth) //not exaclty divisible
    {
      myTotalHeight = (( myTotalWidth / mImageWidth )+1) * ( myFontMetrics.xHeight() + myYSpace );
    }
    else //exactly divisible
    {
      myTotalHeight = ( myTotalWidth / mImageWidth ) * ( myFontMetrics.xHeight() + myYSpace );
    }
  }
  else
  {
    myTotalHeight = myFontMetrics.xHeight() + myYSpace ;
  }
  myTotalHeight += ( myYSpace * 2 ); // one more pad out for the top and bottom
  mLegendHeight = myTotalHeight;
  //qDebug("Legend Height: " + QString::number(mLegendHeight).toLocal8Bit());
}

void OmgGraph::drawHighlight( QPointF theTopLeftPoint, QPointF theBottomRightPoint )
{
  // in the beginning do it all like a rounded rect
  const float SWEEPANGLE=90.0;
  float myAngle=0.0;
  const float WIDTH=5.0;
  const float HEIGHT=5.0;
  QPainterPath myHighlightPath;
  myHighlightPath.moveTo(theBottomRightPoint.x(),theTopLeftPoint.y());
  //top right corner
  myHighlightPath.arcTo(theBottomRightPoint.x()-WIDTH,theTopLeftPoint.y()-HEIGHT, WIDTH, HEIGHT, myAngle , SWEEPANGLE);
  myAngle+=90;
  //line to top left 
  myHighlightPath.lineTo(theTopLeftPoint.x()+WIDTH, theTopLeftPoint.y()-HEIGHT);
  //top left corner
  myHighlightPath.arcTo(theTopLeftPoint.x(), theTopLeftPoint.y()-HEIGHT, WIDTH, HEIGHT, myAngle , SWEEPANGLE);
  myAngle+=90;
  //line to two thirds way down on left
  myHighlightPath.lineTo(theTopLeftPoint.x(),theTopLeftPoint.y() + 
      ((theBottomRightPoint.y()-theTopLeftPoint.y())/3));
  // calculate the lower midpoint for the curve
  qreal myLowerMidPointX = theTopLeftPoint.x()+((theBottomRightPoint.x()-theTopLeftPoint.x())/2);
  qreal myLowerMidPointY = theTopLeftPoint.y()+((theBottomRightPoint.y()-theTopLeftPoint.y())/2);
  QPointF myLowerMidPoint(myLowerMidPointX,myLowerMidPointY);
  // calculate the upper midpoint for the curve
  qreal myUpperMidPointX = theTopLeftPoint.x()+((theBottomRightPoint.x()-theTopLeftPoint.x())/2);
  qreal myUpperMidPointY = theTopLeftPoint.y();
  QPointF myUpperMidPoint(myUpperMidPointX,myUpperMidPointY);
  // calculate the endpoint one third down on the right
  QPointF myEndPoint(theBottomRightPoint.x(),theTopLeftPoint.y() + 
      ((theBottomRightPoint.y()-theTopLeftPoint.y())/3));
  //draw a bezier curve throught the center point to the end point
  myHighlightPath.cubicTo(myUpperMidPoint,myLowerMidPoint,myEndPoint);
  //close path back to original start pos
  myHighlightPath.closeSubpath();
  mpPainter->drawPath(myHighlightPath);
}
