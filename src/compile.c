/* (C) Copyright 2017 by Oliver Cordes
        - ocordes ( at ) astro ( dot ) uni-bonn ( dot ) de


        This file is part of cscript.

        cscript is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        cscript is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with cscript.  If not, see <http://www.gnu.org/licenses/>.

*/


/* compile.c

  written by: Oliver Cordes 2017-07-24
  changed by: Oliver Cordes 2017-08-13

*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <time.h>

#include "abort.h"
#include "cache.h"
#include "configfile.h"
#include "compile.h"
#include "file.h"
#include "helpers.h"
#include "output.h"

#define bufferlen 60000


char *c_compiler   = NULL;
char *cpp_compiler = NULL;

char *CFLAGS       = NULL;
char *CPPFLAGS     = NULL;
char *LDFLAGS      = NULL;





void header_keys( char *key, char* val, int lineno )
{
  char *s;

  if ( ( key == NULL ) || ( val == NULL ) )
  {
    output( 10, "header key wrong in line %i: key=%s val=%s\n", lineno, key, val );
    return;
  }

  output( 10, "extra parameter: key=%s val=%s\n", key, val );

  if ( strcmp( key, "CFLAGS" ) ==  0 )
  {
    append_string( &CFLAGS, val );
    return;
  }

  if ( strcmp( key, "CPPFLAGS" ) ==  0 )
  {
    append_string( &CPPFLAGS, val );
    return;
  }

  if ( strcmp( key, "LDFLAGS" )  == 0 )
  {
    append_string( &LDFLAGS, val );
    return;
  }

  /* now the special MACH keys */
  if ( asprintf( &s, "%s_CFLAGS", get_system_name() ) == -1 )
    return;
  else
  {
    if ( strcmp( key, s ) == 0 )
    {
      append_string( &CFLAGS, val );
      free( s );
      return;
    }
    else
    {
      free( s );
    }
  }

  if ( asprintf( &s, "%s_CPPLAGS", get_system_name() ) == -1 )
  {
    return;
  }
  else
  {
    if ( strcmp( key, s ) == 0 )
    {
      append_string( &CPPFLAGS, val );
      free( s );
      return;
    }
    else
    {
      free( s );
    }
  }

  if ( asprintf( &s, "%s_LDFLAGS", get_system_name() ) == -1 )
  {
    return;
  }
  else
  {
    output( 10, "XFLAGS = %s\n", s );
    if ( strcmp( key, s ) == 0 )
    {
      append_string( &LDFLAGS, val );
      free( s );
      return;
    }
    else
    {
      free( s );
    }
  }
}


char *strip_file( _file_info *file_info )
{
  int    fd;
  FILE  *out_file;
  FILE  *in_file;

  char  *templatename;

  char   buf[bufferlen+1];
  char  *buf2;
  char  *p;
  char  *q, *r;
  int    header;
  int    lineno = 0;

  size_t i;

  if ( asprintf( &templatename, "%s/tmp-XXXXXX.c", "/tmp" ) == -1 )
    templatename = strdup( "/tmp/tmp-XXXXXX.c" );

  fd = mkstemps( templatename, 2 );

  if ( fd == -1 )
    err_abort( -1, "Can't create temporary file for compiling (%s)!", strerror( errno) );

  out_file = fdopen( fd, "w" );

  if ( out_file == NULL )
  {
    close( fd );
    err_abort( -1, "Can't open temporary file '%s' for compiling (%s)!",
              templatename, strerror( errno ) );
  }

  in_file = fopen( file_info->name, "r" );

  if ( in_file == NULL )
    err_abort( -1, "Can't open script file for compiling (%s)!", strerror( errno ) );

  header = 0;   /* write all lines to output file */
  /* loop over all script lines */
  while ( feof( in_file ) == 0 )
  {
    if ( fgets( buf, bufferlen, in_file ) != NULL )
    {
      ++lineno;
      buf2 = strdup( buf );
      buf2[strlen( buf2 )-1] = '\0';
      p = buf2;
      while ( ( p[0] == ' ' ) || ( p[0] == '\t' ) ) ++p;
      if ( p[0] == '#')
      {
        /* the script header */
        if ( p[1] == '!' ) continue;

        ++p;   /* go over the comment sign */

        while ( ( p[0] == ' ' ) || ( p[0] == '\t' ) ) ++p;

        q = strtok( p, " \0" );   /* read the first word */
        if ( q == NULL ) continue;

        if ( strcmp( q, "start") == 0 )
        {
          header = 1;
          continue;
        }
        if ( strcmp( q, "end") == 0 )
        {
          header = 0;
          continue;
        }

        if ( header == 1 )
        {
          r = strtok( NULL, "\0" );
          for (i=0;i<strlen( q );++i)
          {
            q[i] = toupper( q[i] );
          }
          header_keys( q, r, lineno );
        }
      }

      if ( header == 0 )
        fwrite( buf, strlen( buf ), 1, out_file );
    }
  }
  fclose( in_file );
  fclose( out_file );

  close( fd );

  return templatename;
}


void init_compile( config_table *tab )
{
  c_compiler   = config_get_default( tab, "main", "c_compiler", "gcc" );
  CFLAGS       = config_get_default( tab, "main", "CFLAGS", "-O3" );
  cpp_compiler = config_get_default( tab, "main", "cpp_compiler", "g++" );
  CPPFLAGS     = config_get_default( tab, "main", "CPPFLAGS", "-O3" );
  LDFLAGS      = config_get_default( tab, "main", "LDFLAGS", "" );
}

void done_compile( void )
{
  if ( c_compiler != NULL ) free( c_compiler );
  if ( cpp_compiler != NULL ) free( cpp_compiler );

  if ( CFLAGS != NULL ) free( CFLAGS );
  if ( CPPFLAGS != NULL ) free( CPPFLAGS );
  if ( LDFLAGS != NULL ) free( LDFLAGS );
}


int do_compile( char *infile, char *outfile )
{
  char *compile_cmd;
  int   err;

  /* shape some parameters */
  if ( CFLAGS == NULL ) CFLAGS = strdup( "" );
  if ( LDFLAGS == NULL ) LDFLAGS = strdup( "" );

  output( 10, "compiling: %s -> %s\n", infile, outfile );
  output( 10, " CFLAGS : %s\n", CFLAGS );
  output( 10, " LDFLAGS: %s\n", LDFLAGS );

  if ( asprintf( &compile_cmd, "%s %s -o %s %s %s",
       c_compiler, CFLAGS, outfile, infile, LDFLAGS ) == -1 )
       err_abort( -1, "Can't allocate memory for string!" );

  output( 10, "cmd: %s\n", compile_cmd );

  err = system( compile_cmd );

  if ( err != 0 )
    output( 0, "Error during compilation! (err=%i)\n", err );

  free( compile_cmd );

  return err;
}


int compile_file( _file_info *file_info )
{
  char *tempname;

  int   ret_val;

  output( 1, "Compiling the executable ...\n" );

  tempname = strip_file( file_info );
  output( 10, "tempname = %s\n", tempname );

  ret_val = do_compile( tempname, file_info->cache_exe );

  if ( ret_val == 0 )
  {
    /* remeber the compile time */
    file_info->compile_time = time( NULL );
  }

  free( tempname );

  output( 1, "Done.\n" );

  return ret_val;
}
