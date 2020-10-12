/* sedit.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WORKSTATION_ID	(Pint) 1
#define STRUCTURE_ID	(Pint) 1
#define WIDTH		(Pint) 1
#define TYPE		(Pint) 2

static Ppoint		points[] = {{.2,.2},{.4,.7},{.6,.3},{.8,.8}};
static Ppoint_list	point_list = {4, points};

main()
{
    float	width;
    int		type, done = 0;
    char	string[20];

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	plabel( WIDTH );
	pset_linewidth( (Pfloat) 1.0 );
	plabel( TYPE );
	pset_linetype( PLINE_SOLID );
	ppolyline( &point_list );
    pclose_struct();

    popen_ws( WORKSTATION_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WORKSTATION_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );

    pset_edit_mode( PEDIT_REPLACE );
    popen_struct( STRUCTURE_ID );
    while ( !done ) {
	printf( " attribute and value> " ); fflush( stdout );
	if ( fgets( string, sizeof( string ), stdin ) != NULL ) {
	    if ( strncmp( "width", string, 5 ) == 0 ) {
		if ( sscanf( &string[5], "%f", &width ) == 1 ) {
		    pset_elem_ptr( (Pint) 0 );
		    pset_elem_ptr_label( WIDTH );
		    poffset_elem_ptr( (Pint) 1 );
		    pset_linewidth( (Pfloat)width );
		}

	    } else if ( strncmp( "type", string, 4 ) == 0 ) {
		if ( sscanf( &string[4], "%d", &type ) == 1 ) {
		    pset_elem_ptr( (Pint) 0 );
		    pset_elem_ptr_label( TYPE );
		    poffset_elem_ptr( (Pint) 1 );
		    pset_linetype( (Pint) type );
		}
	    }
	    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );
	} else {
	    done = 1;
	    printf( "\n" );
	}
    }
    pclose_struct();

    pclose_ws( WORKSTATION_ID );
    pclose_phigs();
    return 0;
}
