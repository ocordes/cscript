# cscript

## A tool to use small C/C++ programs like scripts.

The main intention was to convert small C/C++ programs into scripts without developing a new scripting language. The 'scipts' will be compiled with gcc/g++ (or other compilers) and the normally executed. The compiled program are stored in a cache directory, so that successive calls are perfomed faster.

## Installation:

  mkdir build; cd build; cmake ..; make; make install

## Example:

~~~~
#!/usr/bin/env cscript -f

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char* argv[] )
{
  int i;

  puts( "Hello, this is a c-program running as a pseudo script!" );
  for (i=1;i<argc;++i)
    printf( "%i: %s\n", i, argv[i] );

  return 0;
}
~~~~

## Header keywords

cscript is recognizing some header keywords in the script to specify the compile command.
The cscript block starts with '#start' and ends with '#end' . Before compiling this block
will be stripped.

Keyword syntax is: '# keyword value'

Known keywords are:
  CFLAGS
  CPPFLAGS
  LDFLAGS
  mach_CFLAGS
  mach_CPPFLAGS
  mach_LDFLAGS

mach is set for different OSes: DARWIN (MacOS), Linux

e.g:

~~~~
...
#start
# LINUX_LDFLAGS -lm
#end
...
~~~~

This adds the math library for Linux only!
