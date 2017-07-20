/* config.c

   written: Oliver Cordes 2007-05-20
   updated: Oliver Cordes 2017-04-18


*/

/* history:

	2017-03-30:
	 - animation add_rawoption
	 - add  parameter case-sensitive for the config table

*/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <ctype.h>


#include "configfile.h"
#include "output.h"



/* interal variables */

#define num_errors 8
char errstr[num_errors][100] = { "OK" ,
				 "Section '%s' not found" ,
				 "Option '%s' not found",
				 "Option with an empty value",
				 "Can't convert to int",
				 "Can't convert to float",
				 "Can't convert to boolean",
				 "No such errorcode!" };

char errstr_add[101] = "";


/* internal functions */

#define num_sections 5
#define num_options  5


#define hidden_section "[]"
#define comment_option "##"


void set_error_add( char *s )
{
    strncpy( errstr_add, s, 100 );
    errstr_add[100] = '\0';
}


void _strupper( char *s )
{
	char *p = s;

	while( (*p) != '\0' )
	{
		(*p) = toupper( (*p) );
		++p;
	}
}


_section *get_section( config_table *tab, char *section )
{
  _section *s = NULL;
  int       i;
	char     *sec;


  if ( tab != NULL )
  {
		sec = strdup( section );
		if ( tab->case_sensitive == 0 )
			_strupper( sec );

    for (i=0;i<tab->ncount;i++)
			if ( strcmp( tab->sections[i].section, sec ) == 0 )
	  		s = &tab->sections[i];

		free( sec );
  }

  return s;
}


_option *get_option( config_table *tab, _section *s, char *option )
{
  _option *o = NULL;
  int      i;
	char    *opt;

  if ( s != NULL)
  {
		opt = strdup( option );
		if ( tab->case_sensitive == 0 )
			_strupper( opt );

    for (i=0;i<s->ncount;i++)
			if ( strcmp( s->options[i].option, opt ) == 0 )
	  		o = &s->options[i];

		free( opt );
  }

  return o;
}



void add_rawoption( config_table *tab,
	                  _section* section,
		                char *option,
		                char *value,
		                char *comment )
{
  _option o;
  _option *p;
  int num;

  /* option is not NULL, but value can ... */
  o.option = strdup( option );

	if ( tab->case_sensitive == 0 )
		_strupper( o.option );

  if ( value != NULL )
    o.value  = strdup( value );
  else
    o.value = NULL;

  if ( comment != NULL )
      o.comment = strdup( comment );
  else
      o.comment = NULL;


  if ( section->nfree == 0 )
    {
      /* expand the array */
      num = section->ncount+num_options;
      if ( ( p = (_option*) malloc( num* sizeof( _option ) ) ) != NULL )
	{
	  if ( section->options != NULL )
	    {
	      memcpy( p, section->options,
		      sizeof( _option ) * section->ncount );
	      free( section->options );
	    }
	  section->options = p;
	  section->nfree = num_options;
	}
      else
	/* do nothing */
	return;
    }

  section->options[section->ncount] = o;
  section->ncount++;
  section->nfree--;
}


void add_option( config_table *tab,
		 char *section,
		 char *option,
		 char *value,
		 char *comment )
{
  _section *s;

  s = get_section( tab, section );

  if ( s != NULL )
      add_rawoption( tab, s, option, value, comment );
}



_section *add_section( config_table *tab, char *section, char *comment )
{
  int num;
  _section s;
  _section *p;

  s.section = strdup( section );

	if ( tab->case_sensitive == 0 )
		_strupper( s.section );

  s.ncount = 0;
  s.nfree = 0;
  s.options = NULL;
  if ( comment != NULL )
      s.comment = strdup( comment );
  else
      s.comment = NULL;

  if ( tab->nfree == 0 )
    {
      /* expand the array */
      num = tab->ncount+num_sections;
      if ( ( p = (_section*) malloc( num* sizeof( _section ) ) ) != NULL )
	{
	  if ( tab->sections != NULL )
	    {
	      memcpy( p, tab->sections,
		      sizeof( _section ) * tab->ncount );
	      free( tab->sections );
	    }
	  tab->sections = p;
	  tab->nfree = num_sections;
	}
      else
	/* do nothing */
	return NULL;
    }

  tab->sections[tab->ncount] = s;
  p = &tab->sections[tab->ncount];
  tab->ncount++;
  tab->nfree--;

  return p;
}


void free_section( _section section )
{
  int i;

  for (i=0;i<section.ncount;i++)
  {
      free( section.options[i].option );
      free( section.options[i].value );
      free( section.options[i].comment );
  }
  if ( section.options != NULL )
  {
      free( section.options );
      free( section.comment );
  }
}


void free_tab( config_table *tab )
{
  int i;

  for (i=0;i<tab->ncount;i++)
    free_section( tab->sections[i] );
  if ( tab->sections != NULL )
    free( tab->sections );
  if ( tab->filename != NULL )
    free( tab->filename );
}


config_table *init_tab ( int case_sensitive )
{
  config_table *tab;

  if ( ( tab = (config_table*) malloc( sizeof( config_table ) ) ) != NULL )
    {
      tab->ncount         = 0;
      tab->nfree          = 0;
      tab->sections       = NULL;
      tab->filename       = NULL;
			tab->case_sensitive = case_sensitive;
    }
  return tab;
}

/* all export functions */


int config_has_section( config_table *tab, char *section )
{
  _section *s;

  s = get_section( tab, section );

  return ( s != NULL );
}


int config_has_option( config_table *tab, char *section, char *option)
{
  int res = 1;

  _section *s;
  _option  *o;

  s = get_section( tab, section );

  if ( s != NULL )
    {
      o = get_option( tab, s, option );
      res = ( o != NULL );
    }

  return res;
}



char *config_get( config_table *tab, char *section, char *option, int *err )
{
  _section *s;
  _option  *o;


  if ( err == NULL )
    return NULL;


  s = get_section( tab, section );

  if ( s == NULL )
    {
      /* section not found */
      (*err) = config_error_wrong_section;
      set_error_add( section );
      return NULL;
    }

  o = get_option( tab, s, option );
  if ( o == NULL )
    {
      /* option not found */
      (*err) = config_error_wrong_option;
      set_error_add( option );
      return NULL;
    }

  if ( o->value == NULL )
    {
      /* no value set */
      (*err) = config_error_empty;
      return NULL;
    }

  (*err) = config_error_ok;
  /* return a copy of the value */
  return strdup( o->value );
}


int config_getint( config_table *tab,
		   char *section, char *option,
		   int *err )
{
  char *p;

  int   val;


  p = config_get( tab, section, option, err );

  if ( p == NULL )
    return 0;


  errno = 0;

  val = strtol( p, NULL, 0);

  if ( errno != 0 )
    (*err) = config_error_int;

  /* p is not needed anymore */
  free( p );

  return val;
}


double  config_getfloat( config_table *tab,
			 char *section, char *option,
			 int *err )
{
  char *p;

  double val;

  p = config_get( tab, section, option, err );

  if ( p == NULL )
    return 0.0;

  errno = 0;

  val = strtod( p, NULL );

  if ( errno != 0 )
    (*err) = config_error_float;

  /* p is not needed anymore */
  free( p );

  return val;
}


int config_getboolean( config_table *tab,
		       char *section, char *option,
		       int *err )
{
  char *p;
  int   i;

  int   val = 1;

  p = config_get( tab, section, option, err );

  if ( p == NULL )
    return 1;

  /* better change all to uppercase letters */
  for (i=0;i<strlen(p);i++) p[i] = toupper( p[i] );

  if ( strcmp( p, "TRUE" ) == 0 )
    val = 1;
  else
    if ( strcmp( p, "FALSE" ) == 0 )
      val = 0;
    else
      if ( strcmp( p, "YES" ) == 0 )
	val = 1;
      else
	if ( strcmp( p, "NO" ) == 0 )
	  val = 0;
	else
	  {
	    /* so normal test failed, testing for 0 or 1 */
	    if ( strcmp( p, "0" ) == 0 )
	      val = 0;
	    else
	      if ( strcmp( p, "1" ) == 0 )
		val = 1;
	      else
		{
		  (*err) = config_error_boolean;
		  val = 1;
		}
	  }

  /* p is not needed anymore */
  free( p );

  return val;
}


config_table *config_read( char *filename, int case_sensitive )
{
  config_table *tab;

  char          buf[1000];
  FILE         *file;

  char         *p, *q, *r;
  char         *c, *ci;

  _section     *last_section = NULL;

	if ( filename == NULL )     /* open only vaid filenames */
		return NULL;

  tab = init_tab( case_sensitive );

  if ( tab != NULL )
  {
    tab->filename = strdup( filename );

    if ( ( file = fopen( filename, "r" ) ) != NULL )
		{
	    /* create a hidden section */
	    last_section = add_section( tab, hidden_section, NULL );
	    while ( feof( file ) == 0 )
			{
				if ( fgets( buf, 1000, file) != NULL )
				{
		    	p = buf;

		    	/* manikuere */

		    	while ( ( p[0] == ' ' ) || ( p[0] == '\t' ) ) p++;
		    	if ( ( strlen(p) > 0 ) && ( p[strlen(p)-1] == '\n' ))
						p[strlen(p)-1] = '\0';
		    	if ( ( strlen(p) > 0 ) && ( p[strlen(p)-1] == '\r' ))
						p[strlen(p)-1] = '\0';


		    	/* find the last comment */
		    	if ( p[0] != '#' )
		    	{
						ci = rindex( p, '#' );
						if ( ci != NULL )
						{
			    		c = strdup( ci );
			    		ci[0] = '\0';
						}
						else
			    		c = NULL;
		    	}
		    	else
						c = NULL;


		    	while ( ( strlen(p) > 1 ) && ( p[strlen(p)-1] == ' ' ) )
						p[strlen(p)-1] = '\0';


		    	switch( p[0] )
		    	{
						case '#':
			    		add_rawoption( tab, last_section, comment_option, p, NULL );
			    		continue;
						case '\0':
			    		/* next section */
			    		break;
						case '[':
			    		/* start a new section */
			    		q = strtok( p, "[]\0" );
			    		last_section = add_section( tab, q, c );
			    		break;
						default:
			    		/* check if '=' is present */
			    		r = index( p, '=' );
			    		if ( r != NULL )
			    		{
								/* new option*/
								q = strtok( p, "=\0" );
								r = strtok( NULL, "\0" );
								add_rawoption( tab, last_section, q, r, c );
			    		}
			    		else
								/* everything else, thanx to Henning for his
				   			rubbish in filter.ini! */
								add_rawoption( tab, last_section, comment_option, p, c );
		    	}
				} /* end of fgets( ... */
	    }
	    fclose( file );
		}  /* end of fopen( ...  */
    else
    {
			/* file not opened correctly */
	  	free_tab( tab );
	  	tab = NULL;
    }
  }

  return tab;
}


void config_done( config_table *tab )
{
  free_tab( tab );
}


char *config_error( int err )
{
    static char dummy[1000];

    if ( ( err < 0 ) || ( err > num_errors ) )
        err =  config_error_error ;

    switch( err )
    {
        case config_error_wrong_section:
        case config_error_wrong_option:
            sprintf( dummy, errstr[err], errstr_add );
            break;
        default:
            strcpy( dummy, errstr[err] );
    }

    return dummy;
}


void config_print_tab( config_table *tab, FILE *file )
{
    int i, j;

    char *o, *v, *c;

    if ( tab == NULL )
	return;

    for (i=0;i<tab->ncount;i++)
    {
	if ( strcmp( tab->sections[i].section, hidden_section ) != 0 )
	{
	    fprintf( file, "[%s]\n", tab->sections[i].section );
	    if ( tab->sections[i].comment != NULL )
		fprintf( file, " %s\n", tab->sections[i].comment );
	    else
		fprintf( file, "\n" );
	}

	for (j=0;j<tab->sections[i].ncount;j++)
	{
	    o = tab->sections[i].options[j].option;
	    v = tab->sections[i].options[j].value;
	    c = tab->sections[i].options[j].comment;

	    if ( strcmp( o, comment_option ) != 0 )
	    {
		if ( v != NULL )
		    fprintf( file, "%s=%s", o, v );
		else
		    fprintf( file, "%s=", o );
		if ( c != NULL )
		    fprintf( file, " %s\n", c );
		else
		    fprintf( file, "\n" );
	    }
	    else
		fprintf( file, "%s\n", v );
	}
	fprintf( file, "\n" );
    }

}


/* some routines with given defaults in the case of errors */

char *config_get_default( config_table *config ,
			   char *section,
			   char *option,
			   char *def )
{
  int err;

  char *s;

  s = config_get( config, section, option, &err );
  if ( err != config_error_ok )
    {
      output( 1, "cfgerr: %s! Using default '%s'\n",
		     config_error( err ), def );
      s = strdup( def );
    }

  return s;
}


int config_getint_default( config_table *config ,
			   char *section,
			   char *option,
			   int def )
{
  int err;

  int i;

  i = config_getint( config, section, option, &err );
  if ( err != config_error_ok )
    {
      output( 1,  "cfgerr: %s! Using default '%d'\n",
		     config_error( err ), def );
      i = def;
    }

  return i;
}


double config_getfloat_default( config_table *config,
				char *section,
				char *option,
				double def)
{
  int err;

  double  d;

  d = config_getfloat( config, section, option, &err );
  if ( err != config_error_ok )
    {
      output( 1,  "cfgerr: %s! Using default '%g'\n",
		     config_error( err ), def );
      d = def;
    }
  return d;
}


int config_getboolean_default( config_table *config,
			       char *section,
			       char *option,
			       int def )
{
  int err;

  int i;

  i = config_getboolean( config, section, option, &err );
  if ( err != config_error_ok )
    {
      output( 1, "cfgerr: %s! Using default '%s'\n",
		     config_error( err ), ((def==1)?"TRUE":"FALSE") );
      i = def;
    }

  return i;
}



/* iterator functions */

char *config_get_sections( config_table *config, int  start, int* saveptr )
{
    if ( start == 1  )
    {
	(*saveptr) = 0;
    }
    else
	(*saveptr)++;

    if (  (*saveptr) >= config->ncount )
	return NULL;

    while ( ( strcmp(  config->sections[(*saveptr)].section, hidden_section ) == 0 ) ||
	    ( strcmp( config->sections[(*saveptr)].section, comment_option ) == 0 ) )
    {
	(*saveptr)++;
	if (  (*saveptr) >= config->ncount )
	    return NULL;
    }
    return config->sections[(*saveptr)].section;
}


char *config_get_options( config_table* config, char* section, int start, int* saveptr )
{
    _section *sec;

    sec = get_section( config, section );

    if ( sec == NULL )
	return NULL;


    if ( start == 1  )
    {
	(*saveptr) = 0;
    }
    else
	(*saveptr)++;

    if (  (*saveptr) >= sec->ncount )
	return NULL;


    return sec->options[(*saveptr)].option;
}
