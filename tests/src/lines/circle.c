/* circle.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1
#define HALF_SQRT2	0.7071 /* sqrt(2)/2 */

main()
{
    static Ppoint4 	control_points[] = {
	{.5,.3,0,1}, {.7,.3,0,HALF_SQRT2},
	{.7,.5,0,1}, {.7,.7,0,HALF_SQRT2},
	{.5,.7,0,1}, {.3,.7,0,HALF_SQRT2},
	{.3,.5,0,1}, {.3,.3,0,HALF_SQRT2},
	{.5,.3,0,1} };
    static Pfloat	knot_vals[] = {0,0,0,1,1,2,2,3,3,4,4,4};

    /* The same points, but without the homogeneous coordinate. */
    static Ppoint3 	pgon_points[] = {
	{.5,.3,0}, {.7,.3,0}, {.7,.5,0}, {.7,.7,0},
	{.5,.7,0}, {.3,.7,0}, {.3,.5,0}, {.3,.3,0},
	{.5,.3,0} };

    Pint		order, i;
    Pfloat		min, max;
    Pfloat_list		knots;
    Ppoint_list3	pgon;
    Ppoint_list34	cpts;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Define the curve. */
    order = 3;
    cpts.num_points = 9;
    cpts.points.point4d = control_points;
    knots.floats = knot_vals;
    knots.num_floats = cpts.num_points + order;
    min = knot_vals[0];
    max = knot_vals[knots.num_floats - 1];

    /* Apply the weights to the x, y, and z control point coords. */
    for ( i = 0; i < cpts.num_points; i++ ) {
	control_points[i].x *= control_points[i].w;
	control_points[i].y *= control_points[i].w;
	control_points[i].z *= control_points[i].w;
    }

    /* Points to show the control polygon. */
    pgon.num_points = 9;
    pgon.points = pgon_points;

    popen_struct( STRUCTURE_ID );
	pset_curve_approx( PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS,
	    (Pfloat) 20.0 );
	pnuni_bsp_curv( order, &knots, PRATIONAL, &cpts, min, max );
	pset_linetype( PLINE_DOT );
	/* The polyline to show the control polygon. */
	ppolyline3( &pgon );
    pclose_struct();

    ppost_struct( WS_ID, STRUCTURE_ID, 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep (30);

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
