//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OMGUIMAIN_H
#define OMGUIMAIN_H

#include <qlabel.h>
#include <qstring.h>
#include <qvaluevector.h>

#ifdef WIN32
  #include <omguimainbase.h>
#else
  #include <omguimainbase.uic.h>
#endif

/**
@author Tim Sutton
*/

struct ModelResult {QString imageFileName; QString logFileName; QString taxonName;};

class OmGuiMain :public OmGuiMainBase
{
  Q_OBJECT
    public:
      OmGuiMain();

      ~OmGuiMain();
      void fileExit();
      void runWizard();
      void saveMapAsImage();
    public slots:
      void drawModelImage (QString theFileName);
      void btnFirst_clicked ();
      void btnLast_clicked ();
      void btnNext_clicked ();
      void btnPrevious_clicked ();
      void setTaxonName();
      void saveProject();
      void loadProject();
      void helpAboutAction_activated();
      
      //! Log emitted from wizard when model is done
      void modelDone(QString theImageFileName, QString theLogFileName, QString theTaxonName);
      void showModel();


    private:
      QLabel * mPictureWidget;
      QValueVector <ModelResult> mModelVector;
      uint mCurrentModel;
};

#endif
