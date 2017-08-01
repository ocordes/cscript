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
  changed by: Oliver Cordes 2017-08-01

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#include "cache.h"
#include "config.h"
#include "compile.h"
#include "configfile.h"
#include "file.h"
#include "helpers.h"
#include "output.h"

#define normal_action 0
#define cache_action  1

char         *executable = NULL;
config_table *conftab    = NULL;
_file_info   *file_info  = NULL;

int           action     = normal_action;
char         *task       = NULL;

static struct option longopts[] = {
  { "debug",          required_argument, NULL, 'd' },
  { "version",        0,                 NULL, 'v' },
  { "cache",          required_argument, NULL, 'l' },
  { NULL,             0,                 NULL, 0   }
};

#define options "vc:d:f:l:"


/* option parser */

void parse_options( int *argc, char **argv[] )
{
  int   ch;
  int   dl;
  char *dummy;

  int   is_loop = 1;

  while ( ( is_loop == 1 ) && ( ( ch = getopt_long( (*argc), (*argv),
                                    options,  longopts, NULL  ) ) != -1 ) )
  {
    switch( ch )
    {
      case 'v':
        printf( "%s: Version %s (build %s)\n", PROJECT_NAME, VERSION, BUILD );
        exit( 0 );
        break;
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
        is_loop = 0;
        break;
      case 'l':
        task = strdup( optarg );
        action = cache_action;
        break;
      /*default:
        printf( "Unknown parameter -%c detected! Abort program!\n", ch );
        exit( -1 ); */
    }

  }

  if ( conftab == NULL )
  {
    dummy = search_file( ".cscript", ".:$HOME:");
    conftab = config_read( dummy, 0 );
    free( dummy );
  }

  (*argc) -= optind;
  (*argv) += optind;
}


int main( int argc, char* argv[] )
{
  int ret_val = 0;

  parse_options( &argc, &argv );

  init_cache( conftab );
  init_compile( conftab );

  switch( action )
  {
    case normal_action:
      if ( executable != NULL )
      {
        file_info = get_file_info( executable );

        check_cache( file_info );

        ret_val = 0;   /* everything is okay */
        switch( file_info->state )
        {
          case state_not_exist:
          case state_outdated:
            ret_val = compile_file( file_info );
          break;
        }

        if ( ret_val == 0 )
        {
          /* run only, if everything is okay! */
          ret_val = cache_execute( file_info, argc, argv );
        }

        /* finished executing */
        free_file_info( file_info );
      }
      break;
    case cache_action:
      cache_task( task );
      break;
  }

  done_compile();
  done_cache();

  if ( conftab != NULL )
  {
    config_done( conftab );
  }
  exit( ret_val );
  return 0;
}
