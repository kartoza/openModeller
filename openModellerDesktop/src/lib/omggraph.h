/***************************************************************************
                          omgguid.h  -  description
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

#ifndef OMGGRAPH_H
#define OMGGRAPH_H
#include "omgdataseries.h"

#include <QString>
#include <QPainter>
#include <QLinearGradient>
#include <QList>
#include <QColor>
/** \ingroup library
  * \brief A utility class for creating graphs
  * @author Tim Sutton
  */

class OMG_LIB_EXPORT OmgGraph 
{
  public:
    /** Constructor . */
    OmgGraph(QPainter * thepPainter);
    /** Desctructor . */
    virtual ~OmgGraph();
    /** Get the graph as a pixmap */
    void render( );
    /** Used to add a new series to the chart */
    void addSeries(OmgDataSeries);
    /** Used to remove a series from the chart 
     * @param theSeriesNo - the number in the series to remove
     * where the first is 0. Note that if you have 3 series,
     * and you remove the 1st, 2 will become 1, 3 become 2 and 
     * so on.
     * @return true if removal was successful otherwise false if
     * it failed.
     */
    bool removeSeriesAt(int theSeriesNo);
    /** Get the number of series in this graph */
    int seriesCount() const;
    /** Clear the graph by painting onto it with a solid colour
     * @param theColour - a colour to use to fill / clear the 
     * drawing area.
     */
    void clear(QColor theColour=Qt::white);



// accessors and mutators
    /** Set the font for axis scale */
    void setAxisFont(QFont theFont);
    /** Set the font for the legend */
    void setLegendFont(QFont theFont);
    /** Set the font for the vertex labels */
    void setVertexLabelFont(QFont theFont);
    
    /** Set whether to render the vertices onto the graph */
    void setGridLinesEnabled(bool theFlag=true);
    /** Whether to render the vertices onto the graph */
    bool hasGridLinesEnabled();
    
    /** Set whether to render the vertices onto the graph */
    void setVerticesEnabled(bool theFlag=true);
    /** Whether to render the vertices onto the graph */
    bool hasVerticesEnabled();

    /** Set whether to render the vertex labels onto the graph */
    void setVertexLabelsEnabled(bool theFlag=true);
    /** Whether to render the vertex labels onto the graph */
    bool hasVertexLabelsEnabled();

    /** Set whether to render a BR to TL diagonal onto the graph */
    void setDiagonalEnabled(bool theFlag=true);
    /** Whether to render a BR to TL diagonal onto the graph */
    bool hasDiagonalEnabled();

    /** Set whether to try to make the line into a spline curve */
    void setSpliningEnabled(bool theFlag=true);
    /** Whether to try to make the line into a spline curve */
    bool hasSpliningEnabled();
    
    /** Set whether to fill areas under the graph*/
    void setAreaFillEnabled(bool theFlag=true);
    /** Whether to fill areas under the graph */
    bool hasAreaFillEnabled();
  private:
    //private ctor - one of the public ctors should be used by clients
    OmgGraph();
    /** Draw a highlight.
     * @param QPointF for the top left corner of the highlight.
     * @param QPointF for the bottom right corner of the highlight.
     */
    void drawHighlight( QPointF theTopLeftPoint, QPointF theBottomRightPoint );
    //called by ctors to prepare class
    void initialise(); 
    QLinearGradient greenGradient();
    QLinearGradient redGradient();
    QLinearGradient blueGradient();
    QLinearGradient grayGradient();
    QLinearGradient customGradient(QColor theColour);
    QLinearGradient randomGradient();
    QLinearGradient highlightGradient();

    qreal mGradientHeight;
    qreal mGradientWidth;
    
    void drawAxes();
    void drawDiagonal();
    void makeLegend();
    void calculateGutters();
    void calculateLegendHeight();

    //
    // data members
    //
    
    QPainter * mpPainter;
    QList <OmgDataSeries> mSeriesList;
    
    int mImageHeight;
    int mImageWidth;
    
    int mGraphImageWidth;
    int mGraphImageHeight;
    
    float mXAxisMin;
    float mXAxisMax;
    int mXGutterWidth;
    int mXGutterHeight;
    int mLegendHeight;
    
    float mYAxisMin;
    float mYAxisMax;
    int mYGutterWidth;
    int mYGutterHeight;

    QFont mAxisFont;
    QFont mLegendFont;
    QFont mVertexLabelFont;

    
    /** whether to render the vertices onto the graph */
    bool mGridLinesFlag;
    /** whether to render the vertices onto the graph */
    bool mShowVerticesFlag;
    /** whether to render a diagonal from bl to tr on the graph */
    bool mDrawDiagonal;
    /** whether to render the vertex labels onto the graph */
    bool mShowVertexLabelsFlag;
    /** whether to try to make the line into a spline curve */
    bool mSplinePointsFlag;
    /** Whether to fill areas under the graph */
    bool mAreaFillFlag;
};

#endif //OMGGRAPH_H

