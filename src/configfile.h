/* config.h

   written: Oliver Cordes 2007-05-20
   updated: Oliver Cordes 2008-10-25


*/

#ifndef __config_h
#define __config_h 1


#include <stdio.h>

#define config_error_ok            0
#define config_error_wrong_section 1
#define config_error_wrong_option  2
#define config_error_empty         3
#define config_error_int           4
#define config_error_float         5
#define config_error_boolean       6
#define config_error_error         7


typedef struct{
  char *option;
  char *value;
  char *comment;
} _option;


typedef struct{
  char    *section;
  _option *options;
  char    *comment;
  int      ncount;
  int      nfree;
} _section;

typedef struct{
  _section *sections;
  int       ncount;
  int       nfree;
  char     *filename;
  int       case_sensitive;
} config_table;



int config_has_section( config_table*, char* );
int config_has_option( config_table*, char*, char* );
char *config_get( config_table*, char*, char*, int* );
int config_getint( config_table*, char*, char*, int* );
double  config_getfloat( config_table*, char*, char*, int* );
int config_getboolean( config_table*, char*, char*, int * );


config_table *config_read( char *filename, int case_sensitive );

void config_done( config_table *tab );

char *config_error( int );


void config_print_tab( config_table *, FILE * );

char *config_get_default( config_table*, char*, char*, char* );
int config_getint_default( config_table*, char*, char*, int );
double config_getfloat_default( config_table*, char*, char*, double);
int config_getboolean_default( config_table*, char*, char*, int );


char *config_get_sections( config_table*, int, int* );
char *config_get_options( config_table*, char*, int, int* );

#endif
