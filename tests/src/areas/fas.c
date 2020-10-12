/* fas.c 1.3 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define NUM_CIRCLE_SEGMENTS	40

/* This function builds an array of circle points. */
static void
circle( x_center, y_center, radius, num_segments, points )
    float	x_center;
    float	y_center;
    float	radius;
    int		num_segments;
    Ppoint	*points;
{
    double	delta, angle;
    int		i;

    angle = 0.0;
    delta = 2 * M_PI / num_segments;
    for ( i = 0; i < num_segments; i++, angle += delta ) {
	points[i].x = x_center + radius * cos( angle );
	points[i].y = y_center + radius * sin( angle );
    }
}

main()
{
    Ppoint		large[NUM_CIRCLE_SEGMENTS];
    Ppoint		small[NUM_CIRCLE_SEGMENTS];
    Ppoint_list		contours[2];
    Ppoint_list_list	sets;
    Pgcolr		color;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Build the contour list -- 2 contours. */
    contours[0].num_points = NUM_CIRCLE_SEGMENTS;
    contours[0].points = large;
    contours[1].num_points = NUM_CIRCLE_SEGMENTS;
    contours[1].points = small;

    sets.num_point_lists = 2; sets.point_lists = contours;

    popen_struct( STRUCTURE_ID );
	pset_int_style( PSTYLE_SOLID ); /* set the interior style */
	pset_edge_flag( PEDGE_ON );	/* turn edges on */
	color.type = PMODEL_RGB;
	color.val.general.x = 0.0;
	color.val.general.y = 1.0;
	color.val.general.z = 0.0;
	pset_edge_colr( &color );	/* set the edge color */

	/* Generate the two circles fully overlapping. */
	circle( 0.15, 0.5, 0.1, NUM_CIRCLE_SEGMENTS, large );
	circle( 0.15, 0.5, 0.04, NUM_CIRCLE_SEGMENTS, small );
	pfill_area_set( &sets ); /* create the area element */

	/* Generate the two circles partially overlapping. */
	circle( 0.45, 0.5, 0.1, NUM_CIRCLE_SEGMENTS, large );
	circle( 0.55, 0.5, 0.04, NUM_CIRCLE_SEGMENTS, small );
	pfill_area_set( &sets ); /* create the area element */

	/* Generate the two circles not overlapping. */
	circle( 0.75, 0.5, 0.1, NUM_CIRCLE_SEGMENTS, large );
	circle( 0.92, 0.5, 0.04, NUM_CIRCLE_SEGMENTS, small );
	pfill_area_set( &sets ); /* create the area element */
    pclose_struct();

    /* Open the workstation and post the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure that the structure is traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
