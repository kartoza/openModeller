/**
 * Declaration of FileParser class.
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
 * @date 2003-09-25
 * $Id: file_parser.hh 4393 2008-06-24 18:17:03Z rdg $
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
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

#ifndef _FILE_PARSERHH_
#define _FILE_PARSERHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/ignorecase_traits.hh>

#include <vector>
#include <string>
#include <utility>   // for std::pair

/**
 * Read key/value pairs from a configuration file.
 * 
 */
class FileParser
{
public:

  FileParser( std::string const file );

  ~FileParser();

  int load( std::string const file );

  /**
  * Get the value of a key (case insensitive).
  */
  std::string get( std::string const key ) const;

  /**
  * Return the number of times that a certain key appears in the file.
  */
  int count( std::string const key ) const;

  /**
   * Get a set of values with the same key specified in the argument.
   */
  std::vector<std::string> getAll( std::string const key ) const;

  /**
   * Returns the number of lines in file.
   */
  int length() const
  {
    return f_lst.size();
  }
  
private:

  typedef std::pair<icstring,std::string> Item;
  typedef std::vector<Item> ItemList;

  ItemList f_lst;
};

#endif
