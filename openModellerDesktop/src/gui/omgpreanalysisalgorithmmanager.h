/***************************************************************************
 *   Copyright (C) 2009 by Pedro Cavalcante                                *
 *   pedro.cavalcante@poli.usp.br                                          *
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

#ifndef OMGPREANALYSISALGORITHMMANAGER_H
#define OMGPREANALYSISALGORITHMMANAGER_H


#include <omgpreanalysis.h>
#include <omgpreanalysisalgorithm.h>
#include <omgpreanalysisalgorithmset.h>

//
// QT includes
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QMap>
#include <QResizeEvent>

#include <ui_omgpreanalysisalgorithmmanagerbase.h>
/** \ingroup gui
 * \brief A manager for setting properties and running pre-analysis algorithms.
 */
class OmgPreAnalysisAlgorithmManager : public QDialog, private Ui::OmgPreAnalysisAlgorithmManager
{
  Q_OBJECT;

public:
  OmgPreAnalysisAlgorithmManager(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgPreAnalysisAlgorithmManager();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
  void cellClicked(int theRow, int theColumn);
  void selectAlgorithm(QString theProfileName);
  void on_pbnRun_clicked();
  /** Refresh the list of algorithms in the alg table 
   * @param theGuid an optional parameter specifying the
   * algorithm that should be selected after the table is refreshed.
   * Leave it blank and the first cell will be selected by default
   * */
  void refreshAlgorithmTable(QString theGuid="");

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void occurrenceSelectorClicked();

signals:
  void preAnalysisCreated(OmgPreAnalysis *);
  /** For logging messages to the progress monitor dialog */
  void logMessage(QString);


private:

  OmgPreAnalysis *getPreAnalysis();
  void setParametersGui( OmgPreAnalysisAlgorithm &theAlgorithm);
  void refreshSpecies();
  QStringList getSpeciesList();

  //!a scrollarea to go inside the layout on the parameters tab
  QScrollArea * mpScrollArea3;
  //!a frame to go into the scroll area
  QFrame * mpQFrame4;
  //!a grid layout to apply to the frame - alg parameters will be added to this
  QGridLayout * mpLayout5;
  //!a combo box and a tool for Occurrence Data
  QComboBox mpSpecies;
  QToolButton mpOccurrenceSelector;

  //!a combo box for LayerSet selection
  QComboBox cboModelLayerSet;


  //! A container type for a widget that represents an entry box for a parameter
  typedef QMap<QString, QWidget *> ParametersMap;
  //! An instance of the above container type
  ParametersMap mParametersMap;
  //!for storing default settings for alg widgets - key is widget name, val is default
  typedef QMap<QString, QString> DefaultParametersMap;
  DefaultParametersMap mDefaultParametersMap;
  //! a container for storing the label widget that is associated with each parameter
  typedef QMap<QString, QWidget *> ParameterLabels;
  ParameterLabels mLabelsMap;
  //! a container for storing the description labels, one for each parameter
  typedef QMap<QString, QWidget *> DescriptionLabels;
  DescriptionLabels mDescriptionLabelsMap;

  //! A container for all the pre-analysis algorithm profiles we are working with
  OmgPreAnalysisAlgorithmSet mAlgorithmSet;

  //! We assume that no Pre-Analysis Algorithm will use more than 1 SamplePtr parameter
  QString mOccurrencesFile;

  QString mSpecies;
};

#endif /* OMGPREANALYSISALGORITHMMANAGER_H */

