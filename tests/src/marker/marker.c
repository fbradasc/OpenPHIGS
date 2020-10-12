/* marker.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

main()
{
    static Ppoint	dots[]	  = {{.3,.9},{.5,.9},{.7,.9}};
    static Ppoint	pluses[]  = {{.3,.7},{.5,.7},{.7,.7}};
    static Ppoint	asters[]  = {{.3,.5},{.5,.5},{.7,.5}};
    static Ppoint	circles[] = {{.3,.3},{.5,.3},{.7,.3}};
    static Ppoint3	crosses[] = {{.3,.1,.5},{.5,.1,.5},{.7,.1,.5}};

    Ppoint_list		point_list;
    Ppoint_list3	point_list3;

    point_list.num_points = 3; /* All the lists have three points. */
    
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	pset_marker_size( (Pfloat) 5.0 );	/* set the marker size */
	pset_marker_colr_ind( (Pint) 3 );	/* set the marker color */

	pset_marker_type( PMARKER_DOT );
	point_list.points = dots;
	ppolymarker( &point_list );

	pset_marker_type( PMARKER_PLUS );
	point_list.points = pluses;
	ppolymarker( &point_list );

	pset_marker_type( PMARKER_ASTERISK );
	point_list.points = asters;
	ppolymarker( &point_list );

	pset_marker_type( PMARKER_CIRCLE );
	point_list.points = circles;
	ppolymarker( &point_list );

	/* Use a 3D polymarker. */
	pset_marker_type( PMARKER_CROSS );
	point_list3.points = crosses;
	point_list3.num_points = 3;
	ppolymarker3( &point_list3 );
    pclose_struct();

    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
