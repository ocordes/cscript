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
  changed by: Oliver Cordes 2017-08-01

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <time.h>

#include "abort.h"
#include "configfile.h"
#include "file.h"
#include "helpers.h"
#include "output.h"

#if defined(__APPLE__)
#define  st_mtim st_mtimespec
#endif

typedef struct{
    const char *name;
    int         val;
} _task_list;

char *cache_dir = NULL;

static _task_list task_list[] = {
    { "list", 1 },
    { NULL, 0 }
};


char * create_arg_list( int argc, char *argv[] )
{
  int    i;
  int   len;
  char *s;

  len = 0;
  for (i=0;i<argc;++i)
  {
    len += strlen( argv[i] ) + 1;  /* add a space or the last \0 */
  }

  s = (char*) malloc( len );
  s[0] = '\0';

  for (i=0;i<argc;++i)
  {
    if ( i > 0 )
    {
      strcat( s, " " );
    }
    strcat( s, argv[i] );
  }

  return s;
}


time_t buf2time( char *s )
{
  if ( s == NULL )
  {
    return 0;
  }

  if ( s[0] == '\0' )
  {
    return 0;
  }

  if ( s[strlen(s)-1] == '\n' )
  {
    s[strlen(s)-1] = '\0';
  }

  return (time_t) strtol( s, (char **)NULL, 10 );
}



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


void check_cache( _file_info *fi )
{
  #define buflen 80
  char    buf[buflen+1];
  FILE   *file;


  /* create all filenames for caching */
  if ( asprintf( &fi->cache_stat, "%s/%s.dat", cache_dir, fi->file_hash ) == -1 )
  {
    fprintf( stderr, "Memory allocation error! Program aborted!\n");
    exit( -1 );
  }
  if ( asprintf( &fi->cache_exe, "%s/%s.exe", cache_dir, fi->file_hash ) == -1 )
  {
    fprintf( stderr, "Memory allocation error! Program aborted!\n");
    exit( -1 );
  }

  /* read cache stats */

  file = fopen( fi->cache_exe, "r" );
  if ( file == NULL )
  {
    output( 10, "No compiled executable in the cache!\n" );
    fi->state = state_not_exist;
    return;
  }
  fclose( file );

  file = fopen( fi->cache_stat, "r" );
  if ( file == NULL )
  {
    output( 10, "No stat file in the cache!\n" );
    fi->state = state_not_exist;
    return;
  }

  /* read the saved times from stat file */
  if ( fgets( buf, buflen, file ) == NULL )
  {
    output( 10, "Can't read the compile time from stat file!\n" );
    fi->compile_time = 0;
  }
  else
  {
    fi->compile_time = buf2time( buf );
  }
  if ( fgets( buf, buflen, file ) == NULL )
  {
    output( 10, "Can't read the access time from stat file!\n" );
    fi->access_time = 0;
  }
  else
  {
    fi->access_time = buf2time( buf );
  }

  fclose( file );

  output( 10, "times: exe=%i a.out=%i\n", fi->file_stat.st_mtim.tv_sec, fi->compile_time );

  if ( fi->file_stat.st_mtim.tv_sec > fi->compile_time )
  {
    fi->state = state_outdated;
  }
}


void cache_update( _file_info *fi )
{
  FILE *file;

  file = fopen( fi->cache_stat, "w" );
  if ( file == NULL )
  {
    output( 1, "Can't write stat information!\n" );
  }
  else
  {
    fprintf( file, "%ld\n", fi->compile_time );
    fprintf( file, "%ld\n", fi->access_time );
    fprintf( file, "%s\n", fi->name );
    fclose( file );
  }
}


int cache_execute( _file_info *fi, int argc, char *argv[] )
{
  char *arglist;
  char *cmd;

  int  ret_val;

  fi->access_time = time( NULL );
  cache_update( fi );

  arglist = create_arg_list( argc, argv );

  if ( asprintf( &cmd, "%s %s", fi->cache_exe, arglist ) == -1 )
  {
    err_abort( -1, "Can't allocate memory for command string!" );
  }

  output( 10, "execute: %s\n", cmd );
  ret_val = system( cmd ) >> 8;

  output( 10, "return value = %i\n", ret_val );

  free( cmd );
  free( arglist );
  return ret_val;
}


int str2task( char *taskname )
{
  char *p = taskname;

  while( (*p) != '\0' )
  {
    (*p) = toupper( (*p) );
    ++p;
  }

  return 0;
}

void cache_task( char *taskname )
{
  int task;

  output( 10, "task: %s\n", taskname );

  task = str2task( taskname );
  
}
