#include <stdio.h>

int main( int argc, char* argv[] )
{
  int i;

  printf( "Hello c-script!\n" );

  for (i=0;i<argc;++i)
  {
    printf( "%i: %s\n", i, argv[i] );
  }
  return 0;
}
