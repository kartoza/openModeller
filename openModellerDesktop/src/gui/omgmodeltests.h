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

#ifndef OMGMODELTESTS_H
#define OMGMODELTESTS_H

#include <ui_omgmodeltestsbase.h>

#include <omgalgorithmset.h>
#include <omglocalitiesoptions.h>
#include <omgexperiment.h>
#include "omgmodeltestoptions.h"
#include <omgmodellerplugininterface.h>

#include <QPointer>

/** \ingroup gui
 * \brief a dialog that allows a user to set up model tests.
 * @author Alexandre Copertino Jardim
 * */

class OmgModelTests: public QDialog, private Ui::OmgModelTestsBase
{
  Q_OBJECT;

public:
  OmgModelTests(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgModelTests();

  void setExperiment(OmgExperiment * const thepExperiment);

  // get methods
  QString getTestName();

signals:
  void refreshExperiment();

private slots:
  void accept();
  void toolSelectLocalitiesFileClicked();
  void toolConfigureLocalitiesClicked();
  void on_toolModelSettings_clicked();
  void updateProgress (int theCurrentValue, int theMaximumValue);

private:
  bool checks();

  void clear();

  void setSpeciesList(QString theFileName);
  QStringList getSpeciesList(QString theFileName);

  void setAlgorithmList();
  void setExperimentSpeciesList();

  void setSpeciesList();

  QString mSpeciesFileName;
  bool mFirstNonIdRecord;

  OmgLocalitiesOptions myOptions;
  //QPointer<OmgExperiment> mpExperiment;
  OmgExperiment * mpExperiment;

  QStringList experimentSpeciesList;

  /** This is the Abstract adapter type. Depending on the users
   * options, it will be initiasised to either the OmgModellerLocalPlugin
   * or OmgModellerWebServices adapter (or possibly other adapter types in 
   * the future */
  OmgModellerPluginInterface * mpModellerPlugin;

  // Tests variables.
  QString myTestName;
  
  QStringList myAlgorithmList;
  int myAlgorithmCount;

  int myTaxaCount;
  QStringList myTaxaList;

  OmgModelTestOptions myModelTestOptions;
};

#endif // OMGMODELTESTS_H
