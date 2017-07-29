#!/Users/ocordes/git/cscript/build/cscript -d 100 -f

#

# start
# blah
#end
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char* argv[] )
{
  int i;

  for (i=0;i<argc;++i)
    printf( "%i: %s\n", i, argv[i] );

  /*exit ( 20 ); */
  return 0;
}
