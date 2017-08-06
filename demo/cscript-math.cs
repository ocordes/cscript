#!/usr/bin/env cscript -f

#

# start
# LINUX_LDFLAGS -lm
#end
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

int main( int argc, char* argv[] )
{
  int i;
  double d;

  /*printf( "Hello, this is a c-program running as a pseudo script!\n" );*/
  puts( "Hello, this is a c-program running as a pseudo script!" );
  for (i=1;i<argc;++i)
    printf( "%i: %s\n", i, argv[i] );

  /*exit ( 20 ); */

  if ( argc > 1 )
  {
    d = atof( argv[1] );
    printf( "sin: %f\n", sin( d ) );
  }

  return 0;
}
