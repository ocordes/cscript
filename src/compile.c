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


/* compile.c

  written by: Oliver Cordes 2017-07-24
  changed by: Oliver Cordes 2017-07-25

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "abort.h"
#include "configfile.h"
#include "compile.h"
#include "file.h"
#include "output.h"

#define bufferlen 60000

char *strip_file( _file_info *file_info )
{
  int   fd;
  FILE *out_file;
  FILE *in_file;

  char *templatename;

  char  buf[bufferlen+1];
  char *buf2;
  char *p;
  char *q, *r;
  int   header;

  if ( asprintf( &templatename, "%s/tmp-XXXXXX.c", "/tmp" ) == -1 )
  {
    templatename = strdup( "/tmp/tmp-XXXXXX.c" );
  };
  fd = mkstemps( templatename, 2 );

  if ( fd == -1 )
  {
    err_abort( -1, "Can't create temporary file for compiling (%s)!", strerror( errno) );
  }

  out_file = fdopen( fd, "w" );

  if ( out_file == NULL )
  {
    close( fd );
    err_abort( -1, "Can't open temporary file '%s' for compiling (%s)!",
              templatename, strerror( errno ) );
  }

  in_file = fopen( file_info->name, "r" );

  if ( in_file == NULL )
  {
    err_abort( -1, "Can't open script file for compiling (%s)!", strerror( errno ) );
  }

  header = 0;   /* write all lines to output file */
  /* loop over all script lines */
  while ( feof( in_file ) == 0 )
  {
    if ( fgets( buf, bufferlen, in_file ) != NULL )
    {
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
          output( 10, "extra parameter: key=%s val=%s\n", q, r );
        }
      }

      if ( header == 0 )
      {
        fwrite( buf, strlen( buf ), 1, out_file );
      }
    }
  }
  fclose( in_file );
  fclose( out_file );

  close( fd );

  return templatename;
}


void compile_file( _file_info *file_info )
{
  char *tempname;

  output( 10, "Compiling the executable ...\n" );

  tempname = strip_file( file_info );
  output( 10, "tempname = %s\n", tempname );
  output( 10, "Done.\n" );
}
