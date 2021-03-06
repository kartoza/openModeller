
/**
* Declaration of class PreJackknifeFactory
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id: PreJackknifeFactory.cpp 4470 2008-07-10 12:26:16Z rdg $
*
* LICENSE INFORMATION
* 
* Copyright(c) 2008 by INPE -
* Instituto Nacional de Pesquisas Espaciais
*
* http://www.inpe.br
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details:
* 
* http://www.gnu.org/copyleft/gpl.html
*/

#include "PreJackknifeFactory.hh"
#include "PreJackknife.hh"

PreJackknifeFactory::PreJackknifeFactory()
: PreAlgorithmFactory( std::string( "PreJackknife" ) )
{
};      

PreJackknifeFactory::~PreJackknifeFactory()
{
};


PreAlgorithm* PreJackknifeFactory::build ( const PreParameters& arg )
{
  PreAlgorithm* instance_ptr = new PreJackknife();

  if(!instance_ptr->reset( arg ))
  {
     std::string msg = "PreJackknifeFactory::build: Invalid parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
  return instance_ptr;
}

