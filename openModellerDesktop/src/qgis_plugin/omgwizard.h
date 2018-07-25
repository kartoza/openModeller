/***************************************************************************
 *   Copyright (C) 2010 by Tim Sutton   *
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

#ifndef OMGWIZARD_H
#define OMGWIZARD_H

#include <ui_omgwizardbase.h>
#include "omgwizardpages.h"
#include "omgalgorithmset.h"
#include "omgspatialreferencesystem.h"
#include "omglayerset.h"
class OmgExperiment;
class OmgModellerPluginInterface;
#include "omglocalitiesoptions.h"
#include "omgmodeltestoptions.h"
class QgsVectorLayer;
//
// QT includes
//
#include <QWidget>
#include <QDialog>


/** \ingroup qgis_plugin
 * \brief a dialog that allows a user to set up a new experiment run.
 * subclasses omgexperiment designer
 * @author Tim Sutton
 * */

class OmgWizard : public QWizard, public Ui::OmgWizardBase
{
  Q_OBJECT;

  public:
    enum { EXPERIMENT_PAGE = 4, 
           OCCURRENCES_PAGE = 5, 
           FIELDS_PAGE = 6, 
           TAXA_PAGE = 7, 
           ALGORITHMS_PAGE = 8 , 
           LAYERSET_PAGE = 9,
           PROJECTION_PAGE = 10};
    OmgWizard(QWidget * parent = 0, Qt::WindowFlags flags = 0  );
    ~OmgWizard();
    /** Get the point / non spatial qgis layer associated with position
     *  in the list box. */
    QgsVectorLayer * getOccurrenceLayer( int theId );
    /* this method does the actual work of refreshing the map format combo */
    void refreshMapFormatCombo();

  protected slots:
    /*$PROTECTED_SLOTS$*/
    //void on_pbnCancel_clicked();
    void accept();
    void setStatus(QString theMessage);
    /* Return an experiment object that represents all the models to be processed */
    OmgExperiment *  getExperiment();
    /** Allow the user to set some options for localities data
     */
    void toolConfigureLocalitiesClicked();
    /* Select a file from the file system and populate the taxa list from the 
     * contents of the file. */
    void toolSelectLocalitiesFileClicked();

    void on_toolSelectDataDirectory_clicked();

    void on_toolModelSettings_clicked();
 
    /** Prepare the layerset to be used if the user has elected to 
     * model using rasters loaded in the current qgis project */
    void prepareQGISLayerSet( );
  signals:
    
    void experimentCreated(OmgExperiment *);
    /** For logging messages to the progress monitor dialog */
    void logMessage(QString);
    /** A signal used to keep track of the experiment creation progress
     * (before the experiment is actually run). */
    void experimentProgress(int theProgress);
    /** A signal used to tell the progress monitor how many models
     * this experiment will contain. */
    void experimentMaximum(int theSize);

  protected:  //protected so that qgis plugin can inherit this class nicely
    OmgExperimentPage * mpExperimentPage;
    OmgOccurrencesPage * mpOccurrencesPage;
    OmgFieldsPage * mpFieldsPage;
    OmgTaxaPage * mpTaxaPage;
    OmgAlgorithmPage * mpAlgorithmPage;
    OmgLayerSetPage * mpLayerSetPage;
    OmgProjectionPage * mpProjectionPage;

    /** This is the collection of available algs */
    OmgAlgorithmSet mAlgorithmSet;
    // this is defined at the bottom of lib/omglocality.h
    OmgSpatialReferenceSystem::WktMap mWktMap;
    /** This is the Abstract adapter type. Depending on the users
     * options, it will be initialised to either the OmgModellerLocalPlugin
     * or OmgModellerWebServices adapter (or possibly other adapter types in 
     * the future */
    OmgModellerPluginInterface * mpModellerPlugin;
    /** Checks to see if the input and output layers match.
    * @return bool True if layers are semantically equivalent.
    */
    bool checkLayersMatch();
    /** A helper function to set a combo box to an appropriate default value.
     * @param theCombo - a QComboBox pointer that will have its default changed.
     * @param theDefault - a QString with the desired default
     * @param theAddItemFlag - a boolean indicating if the item should be added to the list
     *                         if it doesnt exist already.
     * @return true if it succeeded, false otherwise. 
     */
    bool setComboToDefault(QComboBox * theCombo, QString theDefault, bool theAddItemFlag);
    void readSettings();
    void writeSettings(); 
    QStringList getListWidgetEntries(const QListWidget *theListWidget);
    QString mSpeciesFileName;
    bool mFirstNonIdRecord;

    OmgLocalitiesOptions myOptions;

    OmgModelTestOptions myModelTestOptions;
    
    //ephemeral layerset used only if qgis rasters are used for modelling
    OmgLayerSet mQGISLayerSet;
};

#endif
