/* (C) Copyright 2010 by Oliver Cordes
        - ocordes ( at ) astro ( dot ) uni-bonn ( dot ) de


    This file is part of animation.

    animation is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    animation is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with animation.  If not, see <http://www.gnu.org/licenses/>.

*/



/* helpers.c


   written by: Oliver Cordes 2010-06-29
   changed by: Oliver Cordes 2017-08-04
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"


/* search files */

char *expand_environment_variable( char *s)
{
  char *p;
  char *r;

  char *pre;
  char *post;
  char *var;
  char  a = '\0';
  int   ret;

  if ( ( p = strchr( s, '$') ) == NULL )
    return strdup( s );

  /* copy the first part of the string */
  if ( s[0] == '$' )
  {
    pre = &a;
    p = strtok_r( s+1, "/\0", &r );
  }
  else
  {
    pre = strtok_r( s, "$\0", &r );
    p = strtok_r( NULL, "/\0", &r );
  }

  var = getenv( p );
  post = strtok_r( NULL, "\0", &r );
  if ( post == NULL )
    ret = asprintf( &p, "%s%s", pre, var );
  else
    ret = asprintf( &p, "%s%s/%s", pre, var, post );

  if ( ret == -1 )
  {
    p = strdup( "error foo" );
  }

  return p;
}


char *search_file( char *name, char *pathes )
{
  char *dup;
  char *p = NULL;
  char *s;

  char *rp;
  char *sname;

  int  err;

  dup = strdup( pathes );
  s = dup;

  do {
    p = strchr(s, ':');
    if (p != NULL) {
        p[0] = 0;
    }

    rp = expand_environment_variable( s );
    err = asprintf( &sname, "%s/%s" , rp, name );
    free( rp );

    err = access( sname, R_OK );
    if ( err == 0 )
      return sname;

    free( sname );

    s = p + 1;
  } while (p != NULL);

  free( dup );

  return NULL;
}



void append_string( char **s, char *val )
{
  int  len;
  char *ss;
  if ( (*s) == NULL )
  {
    (*s) = strdup( val );
    return;
  }

  len = strlen( (*s) ) + strlen( val ) + 2;
  ss = (char*) malloc( len );
  if ( ss == NULL ) return;
  output( 10, "%s\n", (*s) );
  output( 10, "%s\n", val );
  ss[0] = '\0';
  strcat( ss, (*s) );
  strcat( ss, val );
  free( (*s) );
  (*s) = ss;
  output( 10, "%s\n", (*s) );
}
