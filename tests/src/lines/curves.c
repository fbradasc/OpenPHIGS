/* curves.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

main()
{
    static Ppoint3 	control_points[] = {
	{.1,.8,0},{.35,.9,0},{.6,.9,0},{.4,.7,0},{.7,.7,0},{.9,.88,0}};
    static Pfloat	knots_2ord[] = {1,1,2,3,4,5,6,6};
    static Pfloat	knots_3ord[] = {1,1,1,2,3,4,5,5,5};
    static Pfloat	knots_6ord[] = {1,1,1,1,1,1,2,2,2,2,2,2};

    Pint		order, i;
    Pfloat		min, max;
    Pfloat_list		knots;
    Ppoint_list34	cpts;
    Ppoint_list3	markers;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Assign the control-point list. */
    cpts.num_points = 6;
    cpts.points.point3d = control_points;

    /* Use markers to show the control points. */
    markers.num_points = 6;
    markers.points = control_points;

    popen_struct( STRUCTURE_ID );
	/* Second order curve. */
	order = 2;
	knots.floats = knots_2ord;
	knots.num_floats = cpts.num_points + order;
	min = knots_2ord[order-1];
	max = knots_2ord[knots.num_floats - order];
	pnuni_bsp_curv( order, &knots, PNON_RATIONAL, &cpts, min, max );
	ppolymarker3( &markers );

	/* Set the approximation criteria for the other curves. */
	pset_curve_approx( PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS,
	    (Pfloat) 20.0 );

	/* Third order curve. */
	order = 3;
	knots.floats = knots_3ord;
	knots.num_floats = cpts.num_points + order;
	min = knots_3ord[order-1];
	max = knots_3ord[knots.num_floats - order];
	for ( i = 0; i < 6; i++ ) /* shift it down a bit */
	    control_points[i].y -= 0.3;
	pnuni_bsp_curv( order, &knots, PNON_RATIONAL, &cpts, min, max );
	ppolymarker3( &markers );

	/* Sixth order curve. */
	order = 6;
	knots.floats = knots_6ord;
	knots.num_floats = cpts.num_points + order;
	min = knots_6ord[order-1];
	max = knots_6ord[knots.num_floats - order];
	for ( i = 0; i < 6; i++ ) /* shift it down a bit */
	    control_points[i].y -= 0.3;
	pnuni_bsp_curv( order, &knots, PNON_RATIONAL, &cpts, min, max );
	ppolymarker3( &markers );
    pclose_struct();

    ppost_struct( WS_ID, STRUCTURE_ID, 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep (30);

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
