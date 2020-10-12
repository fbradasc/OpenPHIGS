/* polyline.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

main()
{
    static Ppoint	solid[]	  = {{.3,.8},{.7,.8}};
    static Ppoint	dashed[]  = {{.3,.6},{.7,.6}};
    static Ppoint	dotted[]  = {{.3,.4},{.7,.4}};
    static Ppoint3	dotdash[] = {{.3,.2,.5},{.7,.2,.5}};

    Ppoint_list		point_list;
    Ppoint_list3	point_list3;

    point_list.num_points = 2; /* All the lists have two points. */
    
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	pset_linewidth( (Pfloat) 2.0 );	/* set the line size */
	pset_line_colr_ind( (Pint) 3 );	/* set the line color */

	pset_linetype( PLINE_SOLID );
	point_list.points = solid;
	ppolyline( &point_list );

	pset_linetype( PLINE_DASH );
	point_list.points = dashed;
	ppolyline( &point_list );

	pset_linetype( PLINE_DOT );
	point_list.points = dotted;
	ppolyline( &point_list );

	/* Use a 3D polyline. */
	pset_linetype( PLINE_DASH_DOT );
	point_list3.points = dotdash;
	point_list3.num_points = 2;
	ppolyline3( &point_list3 );
    pclose_struct();

    /* Open the workstation and post the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure that the structures are traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
