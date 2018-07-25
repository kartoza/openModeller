/***************************************************************************
 *   Copyright (C) 2006 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef OMGEXPERIMENTSELECTOR_H
#define OMGEXPERIMENTSELECTOR_H


#include <ui_omgexperimentselectorbase.h>
#include <omgexperiment.h>
#include <omgpreanalysis.h>

#include <QAbstractButton>
#include <QWidget>
#include <QStringList> 
#include <QTreeWidget>
#include <QDialog>
#include <QTreeWidgetItem>
#include <QIcon>
class QDomElement;
class QString;
 /** \ingroup gui
  * \brief A dialog to let a user select a previously run experiment
  * @author Tim Sutton
  */
class OmgExperimentSelector : public QDialog, private Ui::OmgExperimentSelectorBase
{
Q_OBJECT
public:
    OmgExperimentSelector( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~OmgExperimentSelector() ;
    
public slots:
    void accept();
    /** Let plugins scanning directories send regular progress updates */
    void refresh();
    void selectDirectory();
    void chooseExperiment();
    void choosePreAnalysis();
    void hidePreAnalysis();
private:
    void parseExperiment(const QDomElement &theElement, QTreeWidgetItem *theParentItem);
    void updateExperimentList();
    void updatePreAnalysisList();
signals:
    void loadExperiment(OmgExperiment * mypExperiment);
    void loadPreAnalysis(OmgPreAnalysis * mypPreAnalysis);
};

#endif //OMGEXPERIMENTSELECTOR_H
