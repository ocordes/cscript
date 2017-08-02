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


/* file.c

  written by: Oliver Cordes 2017-07-22
  changed by: Oliver Cordes 2017-08-02

*/
#include <stdlib.h>
#include <stdio.h>

#include <sys/stat.h>

#include <string.h>
#include <errno.h>

#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

#include "file.h"
#include "output.h"


char hex2char( int i )
{
  if ( i < 10 )
  {
    return '0'+i;
  }
  else
  {
    return 'a'+(i-10);
  }
}

void str2hexstr( unsigned char c, char *s, int *pos )
{
  s[(*pos)+1]  = hex2char( c & 0xf );
  s[(*pos)]    = hex2char( c>>4 );
  (*pos) += 2;
}


void generate_file_hash( _file_info *fi )
{
  MD5_CTX       md5;
  unsigned char digest[16];

  int           i, pos;

  MD5_Init( &md5 );
  MD5_Update( &md5, fi->name, strlen( fi->name ) );
  MD5_Final( digest, &md5 );

  fi->file_hash = (char*) malloc( 33 );
  fi->file_hash[32] = '\0';

  pos = 0;
  for (i=0;i<16;++i)
  {
    str2hexstr( digest[i], fi->file_hash, &pos );
  }

  output( 10, "hash: %s\n", fi->file_hash );
}


char *get_real_name( char *executable )
{
  char *s;

  s = realpath( executable, NULL );
  if ( s == NULL )
  {
    return strdup( executable );
  }
  else
  {
    return s;
  }
}


_file_info *get_file_info( char *executable )
{
  _file_info *fi;

  fi = (_file_info*) malloc( sizeof( _file_info ) );

  if ( fi == NULL )
  {
    fprintf( stderr, "Allocation of memory failed! Program aborted!\n" );
    exit( -1 );
  }
  fi->name         = get_real_name( executable );
  fi->state        = state_unknown;
  fi->file_hash    = NULL;
  fi->cache_stat   = NULL;
  fi->cache_exe    = NULL;
  fi->compile_time = 0;
  fi->access_time  = 0;

  if ( stat( executable, &fi->file_stat ) == -1 )
  {
    fprintf( stderr, "Error getting stats for executable (%s)! Program aborted!\n",
          strerror( errno ) );
    exit( -1 );
  }
  generate_file_hash( fi );

  return fi;
}


void free_file_info( _file_info *fi )
{
  if ( fi != NULL )
  {
      free( fi->name );
      free( fi->file_hash );
      free( fi->cache_stat );
      free( fi->cache_exe );
      free( fi );
  }
}
