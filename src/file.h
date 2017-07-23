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


/* file.h

  written by: Oliver Cordes 2017-07-22
  changed by: Oliver Cordes 2017-07-23

*/

#ifndef __file_h

#define __file_h 1

#include <sys/stat.h>

#define state_unknown   -1
#define state_ok        0
#define state_not_exist 1
#define state_outdated  2

typedef struct {
  char       *name;
  struct stat file_stat;
  char       *file_hash;
  int         state;
  char       *cache_stat;
  char       *cache_exe;
} _file_info;


_file_info *get_file_info( char * );

void free_file_info( _file_info * );

#endif
