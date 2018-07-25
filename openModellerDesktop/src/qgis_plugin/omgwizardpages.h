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

#ifndef OMGWIZARDPAGES_H
#define OMGWIZARDPAGES_H

#include <ui_omgexperimentpagebase.h>
#include <ui_omgoccurrencespagebase.h>
#include <ui_omgfieldspagebase.h>
#include <ui_omgtaxapagebase.h>
#include <ui_omgalgorithmpagebase.h>
#include <ui_omglayersetpagebase.h>
#include <ui_omgprojectionpagebase.h>

/** \ingroup qgis_plugin
 * \brief a dialog that allows a user to set up a new experiment run.
 * @author Tim Sutton
 * */

class OmgExperimentPage : public QWizardPage, public Ui::OmgExperimentPageBase
{
  Q_OBJECT;

  public:
    OmgExperimentPage();
    ~OmgExperimentPage();
    bool validatePage();
};

class OmgOccurrencesPage : public QWizardPage, public Ui::OmgOccurrencesPageBase
{
  Q_OBJECT;

  public:
    OmgOccurrencesPage();
    ~OmgOccurrencesPage();
    int nextId() const;
    bool isComplete() const;
  public slots:
    void on_lstPointLayers_currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous );
    void on_radUseOccurrencesLayer_toggled( bool theFlag );
    void on_radUseOccurrencesFile_toggled( bool theFlag );
};

class OmgFieldsPage : public QWizardPage, public Ui::OmgFieldsPageBase
{
  Q_OBJECT;

  public:
    OmgFieldsPage();
    ~OmgFieldsPage();
    void initializePage();
};

class OmgTaxaPage : public QWizardPage, public Ui::OmgTaxaPageBase
{
  Q_OBJECT;

  public:
    OmgTaxaPage();
    ~OmgTaxaPage();
    void initializePage();
    bool validatePage();
    void setSpeciesList(QString theFileNameQString);
    /** Look inside a text file and get a list of all unique taxon names in it.
     * @TODO move this into omgui.cpp
     * @param QString the file name of the file to be parsed
     * @return QStringList  - a list of all the names found.
     */
    QStringList getSpeciesList(QString theFileName);
    
    QString speciesFileName() { return mSpeciesFileName; };
  private:
    QString mSpeciesFileName;
    bool mFirstNonIdRecord;
};
class OmgAlgorithmPage : public QWizardPage, public Ui::OmgAlgorithmPageBase
{
  Q_OBJECT;

  public:
    OmgAlgorithmPage();
    ~OmgAlgorithmPage();
};
class OmgLayerSetPage : public QWizardPage, public Ui::OmgLayerSetPageBase
{
  Q_OBJECT;

  public:
    OmgLayerSetPage();
    ~OmgLayerSetPage();
    bool validatePage();
};
class OmgProjectionPage : public QWizardPage, public Ui::OmgProjectionPageBase
{
  Q_OBJECT;

  public:
    OmgProjectionPage();
    ~OmgProjectionPage();
    void initializePage();
  public slots:
    void on_toolFormatLayer_clicked();
};
#endif
