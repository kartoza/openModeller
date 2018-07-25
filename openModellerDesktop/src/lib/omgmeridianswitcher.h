#ifndef _OMGMERIDIANSWITCHER_H_
#define _OMGMERIDIANSWITCHER_H_
#include <QObject>
class QString;

/** \ingroup library
 * \brief This class will shift the meridian (where greenwich meridian is found in the leftmost column)
 * of aarcinfo grid file.....later maybe any gdal file that supports creation.
 */
class OMG_LIB_EXPORT  OmgMeridianSwitcher : public QObject
{

Q_OBJECT;

public:

  /** Default constructor */
  OmgMeridianSwitcher();
  /** Destructor  */
   ~OmgMeridianSwitcher();
   /** Dis waar die kak aangejaag word.... */
  bool doSwitch(QString theInputFileString, QString theOutputFileString);

signals: 
  void error(QString theError);
  void message(QString theMessage);

private:
  //
  //   Private attributes
  //

};

#endif  //OMGMERIDIANSWITCHER

