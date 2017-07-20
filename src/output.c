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

/* output.c

   written by: Oliver Cordes 2010-07-20
   changed by: Oliver Cordes 2017-02-01

   $Id$

*/


#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <time.h>


#include "output.h"



/* global variables */

int debug_level = 1;  /* be quiet */


char dummy[1000];


char timestamp[20];


void debug_generate_timestamp( void )
{
    struct tm   *timep;
    time_t       times;

    times = time( NULL );
    timep = localtime( &times );

    snprintf( timestamp, 20, "%02i:%02i.%02i",
	      timep->tm_hour, timep->tm_min, timep->tm_sec );
}


void debug_init( int newlevel )
{
  debug_level = newlevel;
}



void output( int dlevel, char *format, ... )
{
    va_list ap;

    /* be completly quiet */
    if ( debug_level == 0 )
      return;

    if ( dlevel <= debug_level )
      {
	/* generate output */
	va_start( ap, format );
	vsnprintf( dummy, 1000, format, ap );
	va_end( ap );

	if ( debug_level == 1 )
	  printf( "%s", dummy );
	else
	  {
	    debug_generate_timestamp();
	    printf( "%s: %s", timestamp, dummy );
	  }

	fflush( stdout );
    }
}
