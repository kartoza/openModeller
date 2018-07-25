#ifndef OMGUIABOUT_H
#define OMGUIABOUT_H

#ifdef WIN32
  #include <omguiaboutbase.h>
#else
  #include <omguiaboutbase.uic.h>
#endif

class OmGuiAbout : public OmGuiAboutBase
{
  public:
    OmGuiAbout(QWidget * parent, QString name);
    ~OmGuiAbout(){};
};

#endif //OMGUIABOUT_H
