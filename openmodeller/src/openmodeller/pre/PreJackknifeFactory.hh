
/**
* Declaration of class PreJackknifeFactory
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id: PreJackknifeFactory.hh 4470 2008-07-10 12:26:16Z rdg $
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


#ifndef PRE_JACKKNIFEFACTORY_HH
  #define PRE_JACKKNIFEFACTORY_HH

  #include "PreAlgorithmFactory.hh"
  #include "PreParameters.hh"

  class dllexp PreJackknifeFactory : public PreAlgorithmFactory
  {
    public :    

      //Default constructor
      PreJackknifeFactory();      
      
      //Default Destructor
      ~PreJackknifeFactory();
      
    protected :  
      /**
       * Implementation for the abstract PreFactory::build.
       *
       * arg: A const reference to the parameters used by the algorithm.
       * return: A pointer to the new generated algorithm instance.
       */
      PreAlgorithm* build( const PreParameters& arg );
      
  };

  namespace
  {  
    static PreJackknifeFactory PreJackknifeFactory_instance;
  };

#endif

