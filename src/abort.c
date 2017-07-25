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


/* abort.c

  written by: Oliver Cordes 2017-07-25
  changed by: Oliver Cordes 2017-07-25

*/

#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>

void err_abort( int exitcode, char *fmt, ... )
{
  va_list ap;

  va_start( ap, fmt );
  vfprintf( stderr, fmt, ap );
  va_end( ap );
  fprintf( stderr, " Program aborted!\n" );

  exit( exitcode );
}
