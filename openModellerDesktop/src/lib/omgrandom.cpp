/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This program is free software{} you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation{} either version 2 of the License, or    *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY{} without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program{} if not, write to the                        *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/****************************************************************/
/*********************** Random Generation **********************/

/**
 * Generates a pseudo-random seed and initializes the system
 * random sequence generator. The seed is based in the
 * micro-seconds of the current machine time.
 * @return Not zero if the generator was initiated.
 */
#if WIN32 
  #include <time.h>
#else
 #include <sys/time.h>
 #include <sys/resource.h>
#endif

#include <stdlib.h>

/*******************/
/*** init Random ***/
int initRandom()
{
  unsigned int seed;

#ifndef WIN32
  struct timeval time;
  gettimeofday( &time, (struct timezone *)NULL );
  seed = time.tv_usec;
#else
  seed = (unsigned int) time( NULL );
#endif
    
// This is an workaround to get some things working with GCC. Unfortunately we don't
// know if the implementation of functions like random_shuffle use rand or lrand48
// internally so that we can seed it accordingly. Apparently, the test below is the
// same one used by the function. A more portable solution would be to develop our
// custom random number generator and pass it to random_shuffle or use it directly 
// in other parts of the code.
#ifdef _GLIBCPP_HAVE_DRAND48
  srand48( seed );
#else
  srand( seed );
#endif

  return 1;
}

