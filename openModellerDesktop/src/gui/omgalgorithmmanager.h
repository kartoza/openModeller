/***************************************************************************
 *   Copyright (C) 2005 by Peter Brewer   *
 *   peter@pembshideaway.co.uk   *
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

#ifndef OMGALGORITHMMANAGER_H
#define OMGALGORITHMMANAGER_H

#include <ui_omgalgorithmmanagerbase.h>
#include <omgalgorithm.h>
#include <omgalgorithmset.h>
//
// QT includes
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QResizeEvent>

/** \ingroup gui
 * \brief A manager for customising algorithm profiles.
*/
class OmgAlgorithmManager : public QDialog, private Ui::OmgAlgorithmManager
{
  Q_OBJECT;

public:
  OmgAlgorithmManager(QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~OmgAlgorithmManager();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

  void cellClicked(int theRow, int theColumn);
  void selectAlgorithm(QString theProfileName);
  void on_pbnClone_clicked();
  void on_pbnDelete_clicked();
  void on_pbnApply_clicked();
  void on_pbnDefaults_clicked();
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
  void resizeEvent ( QResizeEvent * theEvent );
signals:


private:
  /** Save the alg (passed by ref) to disk into the users alg profiles area */
  bool saveAlgorithm(OmgAlgorithm &theAlgorithm);

  void setParametersGui( OmgAlgorithm &theAlgorithm);
  //!a layout for the parameters frame on the form
  QVBoxLayout * mpLayout2;
  //!a scrollarea to go inside the layout
  QScrollArea * mpScrollArea3;
  //!a frame to go into the scroll area
  QFrame * mpQFrame4;
  //!a grid layout to apply to the frame - alg parameters will be added to this
  QGridLayout * mpLayout5;

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

  //! A container for all the algorithm profiles we are working with
  OmgAlgorithmSet mAlgorithmSet;
};

#endif

