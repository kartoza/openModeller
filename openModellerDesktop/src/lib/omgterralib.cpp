/***************************************************************************
                          omgterralib.cpp  -  description
                             -------------------
    begin                : Sept 2006
    copyright            : (C) 2006 by Tim Sutton
    email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "omgterralib.h"

OmgTerralib::OmgTerralib()
{
  TeDatabaseFactoryParams dbf_params;

  dbf_params.database_ = "terralib";
  dbf_params.dbms_name_ = "PostgreSQL";

  dbf_params.host_ = "localhost"; 
  // change empty host to localhost for MySQL
  if( dbf_params.dbms_name_ == "MySQL" && dbf_params.host_.empty() )
    dbf_params.host_ = "localhost";

  dbf_params.password_ = "";
  dbf_params.port_ = 5432;
  dbf_params.user_ = "testuser";

  db_.reset(TeDatabaseFactory::make( dbf_params ));

  if( ! db_->isConnected() )
  {
    //QMessageBox::critical( this, "TerraView", 
    //    tr( "Unable to connect to database" ) );    
  } 

}
OmgTerralib::~OmgTerralib()
{
}


