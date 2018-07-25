/***************************************************************************
                          qgisappinterface.h
 Interface class for exposing functions in QgisApp for use by plugins
                             -------------------
  begin                : 2004-02-11
  copyright            : (C) 2004 by Gary E.Sherman
  email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgisappinterface.h 7114 2007-07-30 07:58:57Z mhugent $ */
#ifndef QGISIFACE_H
#define QGISIFACE_H

#include "qgisinterface.h"
#include <QDockWidget>
#include <QToolBox>

class OmgMainWindow;
class QTreeWidgetItem;
class OmgProjection;

/** \class QgisAppInterface
 * \brief Interface class to provide access to private methods in QgisApp
 * for use by plugins.
 *
 * Only those functions "exposed" by QgisInterface can be called from within a
 * plugin.
 */
class QgisAppInterface : public QgisInterface
{
    Q_OBJECT;

  public:
    /**
     * Constructor.
     * @param qgis Pointer to the QgisApp object
     */
    QgisAppInterface ( OmgMainWindow * thepOmgMainWindow );
    ~QgisAppInterface();

    /* Exposed functions */
    //! Zoom map to full extent
    void zoomFull();
    //! Zoom map to previous extent
    void zoomToPrevious();
    //! Zoom to active layer
    void zoomToActiveLayer() ;

    //! Add a vector layer
    QgsVectorLayer * addVectorLayer ( QString vectorLayerPath, QString baseName, QString providerKey );
    //! Add a raster layer given its file name
    QgsRasterLayer *  addRasterLayer ( QString rasterLayerPath,  QString baseName );
    //! Add a raster layer given a raster layer obj
    QgsRasterLayer *  addRasterLayer ( QgsRasterLayer * theRasterLayer, bool theForceRenderFlag = false );
    //! Add a wms layer - not implemented in om desktop
    QgsRasterLayer* addRasterLayer(const QString&, const QString&, const QString&, const QStringList&, const QStringList&, const QString&, const QString&) { return 0; };


    //! Add a project
    bool addProject ( QString theProjectName );
    //! Start a new blank project
    void newProject ( bool thePromptToSaveFlag = false );

    //! Get pointer to the active layer (layer selected in the legend)
    QgsMapLayer *activeLayer();

    //! Add an icon to the plugins toolbar
    int addToolBarIcon ( QAction *qAction );
    //! Remove an icon (action) from the plugin toolbar
    void removeToolBarIcon ( QAction *qAction );
    //! Add an icon to the raster toolbar
    int addRasterToolBarIcon ( QAction *qAction );
    //! Remove an icon (action) from the raster toolbar
    void removeRasterToolBarIcon ( QAction *qAction );
    //! Add an icon to the vector toolbar
    int addVectorToolBarIcon ( QAction *qAction );
    //! Remove an icon (action) from the vector toolbar
    void removeVectorToolBarIcon ( QAction *qAction );
    //! Add an icon to the database toolbar
    int addDatabaseToolBarIcon ( QAction *qAction );
    //! Remove an icon (action) from the database toolbar
    void removeDatabaseToolBarIcon ( QAction *qAction );
    //! Add an icon to the web toolbar
    int addWebToolBarIcon ( QAction *qAction );
    //! Remove an icon (action) from the web toolbar
    void removeWebToolBarIcon ( QAction *qAction );
    //! Add toolbar with specified name
    QToolBar* addToolBar ( QString name );
    //! Get the feature form for a feature
    bool openFeatureForm(QgsVectorLayer*, QgsFeature&, bool) { return false; };

    /** Open a url in the users browser. By default the QGIS doc directory is used
     * as the base for the URL. To open a URL that is not relative to the installed
     * QGIS documentation, set useQgisDocDirectory to false.
     * @param url URL to open
     * @param useQgisDocDirectory If true, the URL will be formed by concatenating
     * url to the QGIS documentation directory path (<prefix>/share/doc)
     */
    void openURL ( QString url, bool useQgisDocDirectory = true );

    /** Get the menu info mapped by menu name (key is name, value is menu id) */
    std::map<QString, int> menuMapByName();
    /** Get the menu info mapped by menu id (key is menu id, value is name) */
    std::map<int, QString> menuMapById();

    /** Return a pointer to the map canvas used by qgisapp */
    QgsMapCanvas * mapCanvas();

    void setCurrentProjection(const OmgProjection*){};
    void currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*){};
    void loadQGisPlugin(QString, QString, QString){};
    void showAttributeTable(QgsVectorLayer*){};

    /** Gives access to main QgisApp object

        Plugins don't need to know about QgisApp, as we pass it as QWidget,
        it can be used for connecting slots and using as widget's parent
    */
    QWidget * mainWindow();

    /** Add action to the plugins menu */
    void addPluginToMenu ( QString name, QAction* action );
    /** Remove action from the plugins menu */
    void removePluginMenu ( QString name, QAction* action );
    /** Add action to the database menu */
    void addPluginToDatabaseMenu(QString, QAction*){};
    /** Remove action from the database menu */
    void removePluginDatabaseMenu(QString, QAction*){};
    /** Add action to the raster menu */
    void addPluginToRasterMenu(QString, QAction*){};
    /** Remove action from the raster menu */
    void removePluginRasterMenu(QString, QAction*){};
    /** Add action to the vector menu */
    void addPluginToVectorMenu(QString, QAction*){};
    /** Remove action from the vector menu */
    void removePluginVectorMenu(QString, QAction*){};
    /** Add action to the web menu */
    void addPluginToWebMenu(QString, QAction*){};
    /** Remove action from the web menu */
    void removePluginWebMenu(QString, QAction*){};

    /** add a dockwidget (not implemented in om desktop) */
    virtual void addDockWidget(Qt::DockWidgetArea, QDockWidget*){};

    virtual void refreshLegend ( QgsMapLayer* ) {};

    QToolBox* getToolBox() { return 0; }
    virtual QgsLegendInterface* legendInterface() { return NULL;};
    virtual void zoomToNext() { return ;};
    virtual bool setActiveLayer(QgsMapLayer*) { return NULL;};
    virtual QList<QgsComposerView*> activeComposers() { QList<QgsComposerView *> myList; return myList; };
    virtual void removeDockWidget(QDockWidget*) { return ;};
    virtual void showLayerProperties(QgsMapLayer*) { return ;};
    virtual bool registerMainWindowAction(QAction*, QString) { return NULL;};
    virtual bool unregisterMainWindowAction(QAction*) { return NULL;};
    virtual QToolBar * advancedDigitizeToolBar() { return NULL;};
    virtual QAction * actionSimplifyFeature() { return NULL;};
    virtual QAction * actionDeleteRing() { return NULL;};
    virtual QAction * actionDeletePart() { return NULL;};
    virtual QAction * actionNodeTool() { return NULL;};
    virtual QAction* actionSelectRectangle() { return NULL; };
    virtual QAction* actionSelectPolygon(){ return NULL; };
    virtual QAction* actionSelectFreehand(){ return NULL; };
    virtual QAction* actionSelectRadius(){ return NULL; };
    virtual void insertAddLayerAction(QAction*){ return ; };
    virtual void removeAddLayerAction(QAction*){ return ; };

    /** Add window to Window menu. The action title is the window title
     * and the action should raise, unminimize and activate the window. */
    virtual void addWindow( QAction *action ) {};
    /** Remove window from Window menu. Calling this is necessary only for
     * windows which are hidden rather than deleted when closed. */
    virtual void removeWindow( QAction *action ) {};

    /** Accessors for inserting items into menus and toolbars.
     * An item can be inserted before any existing action.
     */

    //! Menus
    virtual QMenu *fileMenu() { return NULL;};
    virtual QMenu *editMenu() { return NULL;};
    virtual QMenu *viewMenu() { return NULL;};
    virtual QMenu *layerMenu() { return NULL;};
    virtual QMenu *settingsMenu() { return NULL;};
    virtual QMenu *pluginMenu() { return NULL;};
    virtual QMenu *rasterMenu() { return NULL;};
    virtual QMenu *vectorMenu() { return NULL;};
    virtual QMenu *databaseMenu() { return NULL;};
    virtual QMenu *webMenu() { return NULL;};
    virtual QMenu *firstRightStandardMenu() { return NULL;};
    virtual QMenu *windowMenu() { return NULL;};
    virtual QMenu *helpMenu() { return NULL;};

    //! ToolBars
    virtual QToolBar *fileToolBar() { return NULL;};
    virtual QToolBar *layerToolBar() { return NULL;};
    virtual QToolBar *mapNavToolToolBar() { return NULL;};
    virtual QToolBar *digitizeToolBar() { return NULL;};
    virtual QToolBar *attributesToolBar() { return NULL;};
    virtual QToolBar *pluginToolBar() { return NULL;};
    virtual QToolBar *rasterToolBar() { return NULL;};
    virtual QToolBar *vectorToolBar() { return NULL;};
    virtual QToolBar *databaseToolBar() { return NULL;};
    virtual QToolBar *webToolBar() { return NULL;};
    virtual QToolBar *helpToolBar() { return NULL;};

    //! File menu actions
    virtual QAction *actionNewProject() { return NULL;};
    virtual QAction *actionOpenProject() { return NULL;};
    virtual QAction *actionFileSeparator1() { return NULL;};
    virtual QAction *actionSaveProject() { return NULL;};
    virtual QAction *actionSaveProjectAs() { return NULL;};
    virtual QAction *actionSaveMapAsImage() { return NULL;};
    virtual QAction *actionFileSeparator2() { return NULL;};
    virtual QAction *actionProjectProperties() { return NULL;};
    virtual QAction *actionFileSeparator3() { return NULL;};
    virtual QAction *actionPrintComposer() { return NULL;};
    virtual QAction *actionFileSeparator4() { return NULL;};
    virtual QAction *actionExit() { return NULL;};

    //! Edit menu actions
    virtual QAction *actionCutFeatures() { return NULL;};
    virtual QAction *actionCopyFeatures() { return NULL;};
    virtual QAction *actionPasteFeatures() { return NULL;};
    virtual QAction *actionAddFeature() { return NULL;};
    virtual QAction *actionEditSeparator1() { return NULL;};
    virtual QAction *actionCapturePoint() { return NULL;};
    virtual QAction *actionCaptureLine() { return NULL;};
    virtual QAction *actionCapturePologon() { return NULL;};
    virtual QAction *actionDeleteSelected() { return NULL;};
    virtual QAction *actionMoveFeature() { return NULL;};
    virtual QAction *actionSplitFeatures() { return NULL;};
    virtual QAction *actionAddVertex() { return NULL;};
    virtual QAction *actionDelerteVertex() { return NULL;};
    virtual QAction *actioMoveVertex() { return NULL;};
    virtual QAction *actionAddRing() { return NULL;};
    virtual QAction *actionAddPart() { return NULL;};
    virtual QAction *actionAddIsland() { return NULL;};
    virtual QAction *actionEditSeparator2() { return NULL;};
    virtual QAction *actionCapturePolygon() { return NULL;};
    virtual QAction *actionDeleteVertex() { return NULL;};
    virtual QAction *actionMoveVertex() { return NULL;};

    //! View menu actions
    virtual QAction *actionPan() { return NULL;};
    virtual QAction *actionPanToSelected() { return NULL;};
    virtual QAction *actionZoomIn() { return NULL;};
    virtual QAction *actionZoomOut() { return NULL;};
    virtual QAction *actionSelect() { return NULL;};
    virtual QAction *actionIdentify() { return NULL;};
    virtual QAction *actionMeasure() { return NULL;};
    virtual QAction *actionMeasureArea() { return NULL;};
    virtual QAction *actionViewSeparator1() { return NULL;};
    virtual QAction *actionZoomFullExtent() { return NULL;};
    virtual QAction *actionZoomToLayer() { return NULL;};
    virtual QAction *actionZoomToSelected() { return NULL;};
    virtual QAction *actionZoomLast() { return NULL;};
    virtual QAction *actionZoomActualSize() { return NULL;};
    virtual QAction *actionViewSeparator2() { return NULL;};
    virtual QAction *actionMapTips() { return NULL;};
    virtual QAction *actionNewBookmark() { return NULL;};
    virtual QAction *actionShowBookmarks() { return NULL;};
    virtual QAction *actionDraw() { return NULL;};
    virtual QAction *actionViewSeparator3() { return NULL;};

    //! Layer menu actions
    virtual QAction *actionNewVectorLayer() { return NULL;};
    virtual QAction *actionAddOgrLayer() { return NULL;};
    virtual QAction *actionAddRasterLayer() { return NULL;};
    virtual QAction *actionAddPgLayer() { return NULL;};
    virtual QAction *actionAddWmsLayer() { return NULL;};
    virtual QAction *actionLayerSeparator1() { return NULL;};
    virtual QAction *actionOpenTable() { return NULL;};
    virtual QAction *actionToggleEditing() { return NULL;};
    virtual QAction *actionLayerSaveAs() { return NULL;};
    virtual QAction *actionLayerSelectionSaveAs() { return NULL;};
    virtual QAction *actionRemoveLayer() { return NULL;};
    virtual QAction *actionLayerProperties() { return NULL;};
    virtual QAction *actionLayerSeparator2() { return NULL;};
    virtual QAction *actionAddToOverview() { return NULL;};
    virtual QAction *actionAddAllToOverview() { return NULL;};
    virtual QAction *actionRemoveAllFromOverview() { return NULL;};
    virtual QAction *actionLayerSeparator3() { return NULL;};
    virtual QAction *actionHideAllLayers() { return NULL;};
    virtual QAction *actionShowAllLayers() { return NULL;};

    //! Plugin menu actions
    virtual QAction *actionManagePlugins() { return NULL;};
    virtual QAction *actionPluginSeparator1() { return NULL;};
    virtual QAction *actionPluginListSeparator() { return NULL;};
    virtual QAction *actionPluginSeparator2() { return NULL;};
    virtual QAction *actionPluginPythonSeparator() { return NULL;};
    virtual QAction *actionShowPythonDialog() { return NULL;};

    //! Settings menu actions
    virtual QAction *actionToggleFullScreen() { return NULL;};
    virtual QAction *actionSettingsSeparator1() { return NULL;};
    virtual QAction *actionOptions() { return NULL;};
    virtual QAction *actionCustomProjection() { return NULL;};

    //! Help menu actions
    virtual QAction *actionHelpContents() { return NULL;};
    virtual QAction *actionHelpSeparator1() { return NULL;};
    virtual QAction *actionQgisHomePage() { return NULL;};
    virtual QAction *actionCheckQgisVersion() { return NULL;};
    virtual QAction *actionHelpSeparator2() { return NULL;};
    virtual QAction *actionAbout() { return NULL;};
  private:

    /// QgisInterface aren't copied
    QgisAppInterface ( QgisAppInterface const & );

    /// QgisInterface aren't copied
    QgisAppInterface & operator= ( QgisAppInterface const & );

    //! Pointer to the OmgMainWindow object
    OmgMainWindow * mpOmgMainWindow;
};


#endif //#define QGISAPPINTERFACE_H
