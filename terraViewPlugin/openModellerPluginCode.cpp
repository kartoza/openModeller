#include "openModellerPluginCode.h"

#include <openModellerPluginWindow.h>
//#include <PluginBase.h>
#include <spl.h>

#include <TeAgnostic.h>
#include <TeSharedPtr.h>

#include <qmessagebox.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qwidget.h>

#include <TeDatabaseFactory.h>

//
// Implement the plugin run method. 
//
SPL_PLUGIN_API bool SPL_RUN_NAME_CODE( slcPluginArgs* a_pPluginArgs )
{
  TEAGN_TRUE_OR_RETURN( ( a_pPluginArgs != 0 ), 
    "Invalid plugin args pointer" );
  TEAGN_TRUE_OR_RETURN( ( a_pPluginArgs->GetCount() == 1 ), 
    "Invalid plugin args number" );
  
  void* arg_ptrs[ 1 ];
  TEAGN_TRUE_OR_RETURN( ( a_pPluginArgs->GetArg( 0, arg_ptrs ) == 0 ),
    "Unable to get plugin arguments" );
  TEAGN_TRUE_OR_RETURN( ( arg_ptrs[ 0 ] != 0 ),
    "Invalid plugin args" );
    
  PluginParameters* plug_pars_ptr = ( PluginParameters* ) arg_ptrs[ 0 ];
  
  /* This is needed for windows - TeSingletons doesn't work with DLLs */
  TeProgress::setProgressInterf( plug_pars_ptr->teprogressbase_ptr_ );
  
  /* Creating the plugin form instance */
  static TeSharedPtr< openModellerPluginWindow > openModellerPluginWindow_instance (
	  new openModellerPluginWindow( plug_pars_ptr->qtmain_widget_ptr_ ) );
  openModellerPluginWindow_instance->plugin_params_ = *plug_pars_ptr;
   
  if( !openModellerPluginWindow_instance->loadData() )
  {
	  QMessageBox::information ( plug_pars_ptr->qtmain_widget_ptr_,
		  "Open Modeller Plugin", "Terraview is not connected to a database." );
      return false;/**/
  }
  openModellerPluginWindow_instance->show();

  return true;
}

