/* (C) Copyright 2010 by Oliver Cordes
        - ocordes ( at ) astro ( dot ) uni-bonn ( dot ) de


    This file is part of cscript.

    animation is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    animation is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cscript.  If not, see <http://www.gnu.org/licenses/>.

*/


/* cache.c

  written by: Oliver Cordes 2017-07-21
  changed by: Oliver Cordes 2017-07-21

*/

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "configfile.h"
#include "helpers.h"
#include "output.h"


char *cache_dir = NULL;

void init_cache( config_table *conftab )
{
  char *s;
  int   err;

  s = config_get_default( conftab, "main", "cache_dir", "$HOME/.cscript_cache" );
  cache_dir = expand_environment_variable( s );
  free( s );
  output( 10, "cache_dir = %s\n", cache_dir );

  if ( mkdir( cache_dir, 0000755 ) == -1 ) /* -rwxrx-rx- */
  {
    err = errno;
    if ( err != EEXIST )
    {
      fprintf( stderr, "Can't create cache directory (%s)! Program aborted!\n",
            strerror( err ) );
      exit( -1 );
    }
  }
}


void done_cache( void )
{
  if ( cache_dir != NULL )
    free( cache_dir );
}
