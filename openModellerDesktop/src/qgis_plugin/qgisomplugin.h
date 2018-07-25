/***************************************************************************
    qgisomplugin.h
    -------------------
    begin                : Jan 21, 2004
    copyright            : (C) 2004 by Tim Sutton
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
/*  $Id: plugin.h 9138 2008-08-23 21:37:31Z jef $ */
/***************************************************************************
 *   QGIS Programming conventions:
 *
 *   mVariableName - a class level member variable
 *   sVariableName - a static class level member variable
 *   variableName() - accessor for a class member (no 'get' in front of name)
 *   setVariableName() - mutator for a class member (prefix with 'set')
 *
 *   Additional useful conventions:
 *
 *   theVariableName - a method parameter (prefix with 'the')
 *   myVariableName - a locally declared variable within a method ('my' prefix)
 *
 *   DO: Use mixed case variable names - myVariableName
 *   DON'T: separate variable names using underscores: my_variable_name (NO!)
 *
 * **************************************************************************/
#ifndef QgisOmPlugin_H
#define QgisOmPlugin_H

//QT4 includes
#include <QObject>
#include <QMovie>
#include <QIcon>

//QGIS includes
#include "qgisplugin.h"

//forward declarations
class QAction;
class QToolBar;
#include <QPointer>

class QgisInterface;

//om includes
#include <omgmodel.h>
#include <omgexperiment.h>
#include <omgexperimentprinter.h>
#include <omglocalitiesmodel.h>
#include <omgpreanalysis.h>
/**
* \class Plugin
* \brief [name] plugin for QGIS
* [description]
*/
class QgisOmPlugin: public QObject, public QgisPlugin
{
    Q_OBJECT
  public:

    //////////////////////////////////////////////////////////////////////
    //
    //                MANDATORY PLUGIN METHODS FOLLOW
    //
    //////////////////////////////////////////////////////////////////////

    /**
    * Constructor for a plugin. The QgisInterface pointer is passed by
    * QGIS when it attempts to instantiate the plugin.
    * @param theInterface Pointer to the QgisInterface object.
     */
    QgisOmPlugin( QgisInterface * theInterface );
    //! Destructor
    virtual ~QgisOmPlugin();

  public slots:
    //! init the gui
    virtual void initGui();
    //! open an existing experiment
    void openExperiment();
    //! show the new experiment dialog
    void newExperiment();
    //! run the current experiment dialog
    void runExperiment();
    //! load an experiment - and add its layers into the qgis toc
    void loadExperiment( OmgExperiment * thepExperiment );
    //! set the current experiment - does not add its layers to the toc
    void setCurrentExperiment( OmgExperiment * thepExperiment );
    //! show the alg manager
    void showAlgorithmManager();
    //! show the layerset manager
    void showLayerSetManager();
    //! show the data search tool
    void fileFetch();
    //! show the options dialog
    void settingsOptions();
    //! show the text file splitter dialog
    bool toolsFileSplitter();
    //! show the raster thresholding dialog
    //bool toolsRasterThreshold();
    //! show the csv export dialog
    void exportSamplesToCsv();
    //! unload the plugin
    void unload();
    //! Show the terms and conditions dialog
    void showLicense();
    //! Show the about dialog
    void about();
    //! show the help document
    void help();
    //! print a pdf of the experiment
    void printPdf();
    //! Load a shp into QGIS e.g. when created by the gbif scraper
    void showShapefile( QString theShapefile );
    

  private:

    ////////////////////////////////////////////////////////////////////
    //
    // MANDATORY PLUGIN PROPERTY DECLARATIONS  .....
    //
    ////////////////////////////////////////////////////////////////////

    int mPluginType;
    //! Pointer to the QGIS interface object
    QgisInterface *mQGisIface;
    ////////////////////////////////////////////////////////////////////
    //
    // ADD YOUR OWN PROPERTY DECLARATIONS AFTER THIS POINT.....
    //
    ////////////////////////////////////////////////////////////////////

    //! Pointer to the toolbar for openmodeller
    QToolBar * mpOmToolBar;
    void createActions();
    void createToolBars();
    void createMenus();
    /** Used for animating the running icon shown next to 
     * the currently running tree widget entry */
    void updateRunningIcon(const QRect & theRect );
    QAction *mpOpenAct;
    QAction *mpNewAct;
    QAction *mpFetchAct;
    QAction *mpPrintPdfAct;
    QAction *mpRefreshReportsAct;
    QAction *mpOptionsAct;
    //QAction *mpToolsRasterThresholdAct;
    QAction *mpToolsExternalTestsAct; 
    QAction *mpToolsFileSplitterAct;
    QAction *mpAboutOmgAct;
    QAction *mpSupportedFormatsAct;
    QAction *mpLicenseAct;
    QAction *mpHelpAct;
    QAction *mpSurveyAct;
    QAction *mpLayerSetManagerAct;
    QAction *mpExportToCsvAct;
    QAction *mpPreAnalysisAlgorithmManagerAct;
    QAction *mpAlgorithmManagerAct;
    QAction *mpAddLayerAct; //do we need this...

    QPointer<OmgModel>  mpCurrentModel;
    QPointer<OmgExperiment> mpCurrentExperiment;
    QPointer<OmgPreAnalysis> mpCurrentPreAnalysis;
    
    //running animated icon widget
    QIcon mRunningIcon;
    QMovie * mpRunningMovie;
};

#endif //QgisOmPlugin_H
