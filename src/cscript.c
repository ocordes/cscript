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


/* cscript.c

  written by: Oliver Cordes 2017-07-20
  changed by: Oliver Cordes 2017-07-22

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#include "cache.h"
#include "configfile.h"
#include "file.h"
#include "helpers.h"
#include "output.h"


char         *executable = NULL;
config_table *conftab    = NULL;
_file_info   *file_info  = NULL;

static struct option longopts[] = {
  { "debug",          required_argument, NULL, 'd' },
  { NULL,             0,                 NULL, 0   }
};

#define options "c:d:f:"


/* option parser */

void parse_options( int argc, char *argv[] )
{
  int   ch;
  int   dl;
  char *dummy;

  while ( ( ch = getopt_long( argc, argv,
                             options,  longopts, NULL  ) ) != -1 )
  {
    switch( ch )
    {
      case 'c':
        conftab = config_read( optarg, 0 );
        break;
      case 'd':
        dl = atoi( optarg );
        if ( dl < 1 )
        {
          fprintf( stderr, "Ingnoring debug parameter '%s'!\n", optarg );
        }
        else
        {
          debug_init( dl );
        }
        break;
      case 'f':
        executable = strdup( optarg );
        break;
      default:
        printf( "Unknown parameter -%c detected! Abort program!\n", ch );
        exit( -1 );
    }

  }

  if ( conftab == NULL )
  {
    dummy = search_file( ".cscript", ".:$HOME:");
    conftab = config_read( dummy, 0 );
    free( dummy );
  }

  argc -= optind;
  argv += optind;
}


int main( int argc, char* argv[] )
{
  int i;

  parse_options( argc, argv );

  init_cache( conftab );

  printf( "Hello c-script!\n" );

  for (i=0;i<argc;++i)
  {
    printf( "%i: %s\n", i, argv[i] );
  }

  output( 1, "executable=%s\n", executable );

  file_info = get_file_info( executable );

  check_cache( file_info );


  /* finished executing */
  free_file_info( file_info );
  done_cache();

  if ( conftab != NULL )
  {
    config_done( conftab );
  }
  return 0;
}
