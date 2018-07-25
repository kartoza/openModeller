/**
 * Main program for all GARP unit tests
 * 
 * @file   test_main.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-07-23
 * $Id: test_main.cpp 247 2004-07-23 21:50:35Z scachett $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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
 * 
 */

#include "../../../test/CppUnitLite/TestHarness.h"

#include <om_log.hh>
#include <string.h>

extern Log g_log;

int main(int argc, char **argv)
{
  Log::Level level = Log::Error;

  if (argc == 2)
    {
      if (!strcmp(argv[1], "debug"))
	level = Log::Debug;
    }

  // Reconfigure the global logger.
  g_log.set( level, stdout, "GarpTests" );

  TestResult tr;
  return TestRegistry::runAllTests(tr);
}
