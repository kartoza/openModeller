/***************************************************************************
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
#ifndef OMGMAINWONDOW_H
#define OMGMAINWINDOW_H

//QT Includes
#include <QMainWindow>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QProgressBar>
#include <QProgressDialog>
#include <QString>
#include <QPicture>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QSystemTrayIcon>
#include <QPointer>
#include <QPoint>

//Local Includes
#include <omgmodel.h>
#include <omgexperiment.h>
#include <omgexperimentprinter.h>
#include <omglocalitiesmodel.h>
#include <omgpreanalysis.h>
#include <ui_omgmainwindowbase.h>

#ifdef WITH_QGIS
#include <qgsmapcanvas.h>
#include <qgsmaptool.h>
#endif

#ifdef WITH_OSSIMPLANET_QT
#include <QtOpenGL/QGLFormat>
#include <ossimPlanetQt/ossimPlanetQtGlWidget.h>
#endif

class QgisAppInterface;
/** \ingroup gui
 * \brief The main application dialog. Here the menus toolbars etc are 
 * displayed and the results of experiments can be viewed.
@author Tim Sutton
*/
class OmgMainWindow : public QMainWindow, private Ui::OmgMainWindowBase
{
  Q_OBJECT
    public:
      OmgMainWindow(QWidget* parent = 0, Qt::WFlags fl = 0 );
      ~OmgMainWindow();
   public slots:
      void setCurrentExperiment(OmgExperiment * theExperiment);
      void setCurrentPreAnalysis(OmgPreAnalysis * thepPreAnalysis);
      void setModelCreationProgress (QString theModelGuid, int theProgress);
      void setModelProjectionProgress (QString theModelGuid, int theProgress);
      /** Slot called when all work (including post processing)
          is completed for a model.
          @param QString theModelGuid
          */
      void modelCompleted(QString theModelGuid);
      void preAnalysisCompleted();
      void refreshExperiment();
      void setExperimentProgress (int theProgress);
      void setExperimentMaximum(int theMaximum);
      void logMessage(QString theMessage);
      void logMessage(QString theModelGuid, QString theMessage);
      void logError(QString theModelGuid, QString theMessage);
   private slots:
      void saveMapAsImage();
      void setCurrentModel(OmgModel * thepModel);
      void setCurrentPreAnalysisReport();
      void setCurrentProjection(OmgProjection const * thepProjection);
      void clearCurrentExperiment();
      void currentItemChanged(QTreeWidgetItem * thepCurrentItem, QTreeWidgetItem * thepPreviousItem);
      void currentPreAnalysisItemChanged(QTreeWidgetItem * thepCurrentItem, QTreeWidgetItem * thepPreviousItem);
      void treeContextMenuEvent(const QPoint & thePoint);
      void reportModeChanged(QString theReportMode);
      void setTreeColumnSizes(int position, int index);
      void setEnableModelDetailWidgets(bool theBool);
      void setViewMode(QString theMode);
      void printAlgorithmProfilesReport();
      void printExperimentReport();
      void printPdf();
      void printExperimentPdf();
      void printPreAnalysisSummaryPdf();
      void runExperiment();
      void runPreAnalysis();
      void resetExperiment();
      void stopExperiment();
      void createTrayIcon();
      //
      // Slots called from context menus
      //
      void removeModelFromExperiment();
      void runModel();
      void removeAlgorithmFromExperiment();
      void removeTaxonFromExperiment();
      //
      // Autoconnect slots and menus
      //
      void on_tabModelViewDetails_currentChanged(int);
      void on_cboSortBy_currentIndexChanged(QString);
      void fileNew();
      void fileSave();
      void fileOpen();
      void closeExperiment();
      void fileFetch();
      void fileExit();
      void settingsOptions();
      void about();
      void supportedFormats();
      void showLicense();
      void showLayerSetManager();
      void showPreAnalysisAlgorithmManager();
      void showAlgorithmManager();
      bool checkLicenseIsAgreed();
      //bool toolsConvert();
      bool toolsContour();
      bool toolsRasterThreshold();
      bool toolsExternalTests();
      bool toolsClimateConvert();
      bool toolsWarp();
      bool toolsFileSplitter();
      void exportSamplesToCsv();
      void refreshReports();
      /** Display some useful help documentation in the report tab */
      void showHelp();
      /** Show the user survey wizard */
      void showSurvey();
      /** Populate the experiement tree. The tree will have a root 
       * node for the experiment, subnodes for taxa or algorithm (depending
       * on sort order) and then child nodes for models and projections. */
      void populateTree();
      /** Populate the pre-analysis tree. The tree will have a root 
       * node for the pre-analysis, subnodes for taxa or algorithm (depending
       * on sort order) and then child nodes for models and projections. */
      void populatePreAnalysisTree();
      /** Show thumbnails and summary stats for the selected
       * algorithm. */
      void showAlgorithmSummary(QString theAlgorithmName);
      /** Show thumbnails and summary stats for the selected
       * taxon. */
      void showTaxonSummary(QString theTaxonName);
      /** Handler for experiment stopped signal */
      void experimentStopped();
      /** Used for animating the running icon shown next to 
       * the currently running tree widget entry */
      void updateRunningIcon(const QRect & theRect );
      /** Publish the active model to WBCMS repository */
      void publishModel();

      
// A problem with cmake currently prevents this ifdef from working
//I will reinstate it when cmake issue is resolved
// @see http://public.kitware.com/pipermail/cmake/2007-March/013161.html

//#ifdef WITH_QGIS
      void zoomInMode();
      void zoomOutMode();
      void panMode();
      void zoomFull();
      void zoomPrevious();
      void loadQGisPlugin(QString theName, 
            QString theDescription, QString theFullPathName);
      /*
      void identifyMode();
      void measureMode();
      void measureAreaMode();
      */
      void addLayer();
    signals:
      /** This signal is needed to notify
       * qgis plugins of state change. Primarily
       * it is used to get copyrightlabel plugin
       * to be aware that its label has changed. */
      void projectRead();
      void newProject();
//#endif
   protected:
      void closeEvent(QCloseEvent *event);
    private:
      void resizeEvent ( QResizeEvent * event );
      void readSettings();
      void writeSettings(); 
      //currently there doesnt seem to be a way to set these up in designer
      //so we are going to manually create the toolbars etc
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void showModelReport(QString theReportMode);
      void showPreAnalysisReport(QString theReportMode);
      void showProjectionReport(OmgProjection const * thepProjection, QString theReportMode);
      void disableControlsWhileRunning();
      void enableControlsAfterRunning();
      void disableMapControls();
      void enableMapControls();
      void setReportStyleSheet();
      /** Get text to display in report tab on application start */
      QString helpText();
      /** Get text to display in report tab on experiment start */
      QString experimentHelpText();

      enum SortMode {BY_TAXON,BY_ALGORITHM};
      SortMode mSortMode;
      //this little group needed for our system tray
      QMenu * mTrayIconMenu; 
      QAction * mMinimizeAct;
      QAction * mMaximizeAct;
      QAction * mRestoreAct;
      QAction * mQuitAct;
      QPointer <QSystemTrayIcon> mTrayIcon;
      //
      // status bar widgets
      //
      QProgressBar * mpModelProgress;
      QProgressBar * mpExperimentProgress;
      QLabel * mpExperimentProgressLabel;

      //an experiment printer (can run the print prep in a separte thread)
      QPointer <OmgExperimentPrinter>  mpExperimentPrinter;

      //for when we want a blocking progress dialog e.g. for printing
      QProgressDialog mProgressDialog;
      
      //running animated icon widget
      QIcon mRunningIcon;
      QMovie * mpRunningMovie;

      //menu and toolbar stuff
      QMenu *mpFileMenu;
      QMenu *mpEditMenu;
      QMenu *mpDataPrepMenu;
      QMenu *mpPostProcessingMenu;
      QMenu *mpReportsMenu;
      QMenu *mpSettingsMenu;
      QMenu *mpHelpMenu;
      QToolBar *mpFileToolBar;
      QToolBar *mpDataPrepToolBar;
      QToolBar *mpPostProcessingToolBar;
      QToolBar *mpReportsToolBar;
      QAction *mpNewAct;
      QAction *mpRunAct;
      QAction *mpResetAct;
      QAction *mpOpenAct;
      QAction *mpCloseAct;
      QAction *mpSaveAct;
      QAction *mpSaveAsImageAct;
      QAction *mpFetchAct;
      QAction *mpPrintPdfAct;
      QAction *mpRefreshReportsAct;
      QAction *mpExitAct;
      QAction *mpOptionsAct;
      //QAction *mpToolsConvertAct;
      QAction *mpToolsClimateConvertAct;
      QAction *mpToolsContourAct;
      QAction *mpToolsRasterThresholdAct;
      QAction *mpToolsExternalTestsAct; 
      QAction *mpToolsWarpAct;
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
      QAction *mpAvailableAlgorithmsReportAct;
      QAction *mpExperimentReportAct;
#ifdef WITH_QGIS
      QAction *mpAddLayerAct; //do we need this...
      QAction *mpZoomInAct;
      QAction *mpZoomOutAct;
      QAction *mpPanAct;
      QAction *mpZoomFullAct;
      QAction *mpZoomPreviousAct;
      /*
      QAction *mpIdentifyAct;
      QAction *mpMeasureAct;
      QAction *mpMeasureAreaAct;
      */
      QgsMapCanvas * mpMapCanvas;
      QToolBar * mpMapToolBar;
      QgsMapTool * mpPanTool;
      QgsMapTool * mpZoomInTool;
      QgsMapTool * mpZoomOutTool;
      QgsMapTool * mpZoomFullTool;
      QgsMapTool * mpZoomPreviousTool;
      /*
      QgsMapTool * mpIdentifyTool;
      QgsMapTool * mpMeasureTool;
      QgsMapTool * mpMeasureAreaTool;
      */
      //! interface to QgisApp for plugins
      QPointer <QgisAppInterface> mpQgisInterface;
      friend class QgisAppInterface;
      // when an experiment is first loaded we
      // need to reset the map cavnase extents
      bool mResetExtentsFlag;
#endif      
      //
      // Context menus and their actions
      //
      QMenu *mpExperimentContextMenu;
      QMenu *mpAlgorithmContextMenu;
      QMenu *mpTaxonContextMenu;
      QMenu *mpModelContextMenu;
      QAction *mpDeleteModelAct;
      QAction *mpRunModelAct;
      QAction *mpPublishModelAct;
      QAction *mpDeleteAlgorithmAct;
      QAction *mpDeleteTaxonAct;
      
      QLabel * mpPictureWidget;
      QPointer<OmgModel>  mpCurrentModel;
      OmgLocalitiesModel * mpLocalitiesModel;
      QPointer<OmgExperiment> mpCurrentExperiment;
      QPointer<OmgPreAnalysis> mpCurrentPreAnalysis;
      //OmGuiReportBase *  mReport;

      //
      // Methods for building the model tree
      //
      /** Populate the experiment tree with the top level experiment node */
      void makeExperimentNode();
      /** Get the root node representing the experiment. If the experiment 
       * node does not exist, it will be created.
       * @return QTreeWidgetItem * pointer to the experiment node.
       */
      QTreeWidgetItem * experimentNode();
      /** Populate the experiment tree with the top level experiment node */
      void makeTaxonTreeNodes();
      /** Get the node representing a taxon group. It assumes sort order 
       * is by taxon.
       * @param theLabel - name of taxon whose group node we are looking for.
       * @return a QListWidgetItem pointer if found or otherwise NULL
       */
      QTreeWidgetItem * taxonNode(QString theLabel);
      /** Populate the experiment tree with the top level experiment node */
      void makeAlgorithmTreeNodes();
      /** Get the node representing a algorithm group. It assumes sort order 
       * is by algorithm.
       * @param theLabel - name of algorithm whose group node we are looking for.
       * @return a QListWidgetItem pointer if found or otherwise NULL
       */
      QTreeWidgetItem * algorithmNode(QString theLabel);
      /** Populate the pre-analysis tree with the top level pre-analysis node */
      void makePreAnalysisNode();
      /** Get the node representing a pre-analysis group. It doesn't assume sort order.
       * @return a QListWidgetItem pointer if found or otherwise NULL
       */
      QTreeWidgetItem *preAnalysisNode();
      /** Make a model tree node given its model no in the experiment.
       *  The node will be attached to the correct parent node (taxon or 
       *  algorithm name) depending on teh tree sort order.
       *  @param theModelNumber - model number in the current experiment 
       *  that the node will be created for.
       *  @return false if there was a problem and the node could not 
       *  be created, otherwise true.
       */
      bool makeModelTreeNode(int theModelNumber);
      /** Make a pre-analysis tree node given its model no in the pre-analysis.
       *  The node will be attached to the correct parent node (taxon or 
       *  algorithm name) depending on teh tree sort order.
       */
      void makePreAnalysisTreeNodes();
      /** Get the node representing a model given its guid. 
       * @param theGuid - Globally unique identifier of the model whose matching 
       * node should be found.
       * @return a QListWidgetItem pointer if found or otherwise NULL
       */
      QTreeWidgetItem * modelNode(QString theGuid);
      /** Make a projection tree node given its projection guid in the experiment.
       *  The node will be attached to the correct model parent node.
       *  @param theProjectionGuid - projection guid in the current experiment 
       *  that the node will be created for.
       *  @return false if there was a problem and the node could not 
       *  be created, otherwise true.
       */
      bool makeProjectionTreeNode(QString theGuid);
      /** Get the node representing a projection given its guid. 
       * @param theGuid - Globally unique identifier of the projection whose matching 
       * node should be found.
       * @return a QListWidgetItem pointer if found or otherwise NULL
       */
      QTreeWidgetItem * projectionNode(QString theGuid);
#ifdef WITH_OSSIMPLANET_QT
      void initialiseOssimPlanet();
      ossimPlanetQtGlWidget * mpPlanet;
#endif
};

#endif

