#!/usr/bin/env cscript -f

#

# start
# blah
#end
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char* argv[] )
{
  int i;

  /*printf( "Hello, this is a c-program running as a pseudo script!\n" );*/
  puts( "Hello, this is a c-program running as a pseudo script!" );
  for (i=1;i<argc;++i)
    printf( "%i: %s\n", i, argv[i] );

  /*exit ( 20 ); */
  return 0;
}
