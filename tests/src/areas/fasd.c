/* fasd.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1
#define NUM_SEGMENTS	40
#define RADIUS		0.10
#define SPACING		0.20

#define SET_COLORV( colrv, r, g, b ) \
    (colrv).direct.rgb.red = (r), \
    (colrv).direct.rgb.green = (g), \
    (colrv).direct.rgb.blue = (b) \

/* This function builds an array of circle points. */
static void
circle( center, points )
    Ppoint	*center;
    Ppoint3	*points;
{
    double	delta, angle;
    int		i;

    delta = 2 * M_PI / NUM_SEGMENTS;
    for ( i = 0, angle = 0; i < NUM_SEGMENTS; i++, angle += delta ) {
	points[i].x = center->x + RADIUS * cos( angle );
	points[i].y = center->y + RADIUS * sin( angle );
	points[i].z = 0.0;
    }
}

main()
{
    Ppoint3		circles[7][NUM_SEGMENTS];
    Pfacet_vdata_list3	contours[7];
    Pfacet_data3	facet_colors[7];
    Ppoint		location;
    float		angle;
    int			i;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Build the contours. */
    for ( i = 0, angle = M_PI/2; i < 6; i++, angle += M_PI/3 ) {
	contours[i].num_vertices = NUM_SEGMENTS;
	contours[i].vertex_data.points = circles[i];
	location.x = 0.5 + (SPACING + RADIUS) * cos( angle );
	location.y = 0.5 + (SPACING + RADIUS) * sin( angle );
	circle( &location, circles[i] );
    }
    /* Build the one in the center. */
    contours[6].num_vertices = NUM_SEGMENTS;
    contours[6].vertex_data.points = circles[6];
    location.x = 0.5; location.y = 0.5;
    circle( &location, circles[i] );

    /* Set the facet colors. */
    SET_COLORV(facet_colors[0].colr, 1, 0, 0); /* red */
    SET_COLORV(facet_colors[1].colr, 1, 1, 0); /* yellow */
    SET_COLORV(facet_colors[2].colr, 0, 1, 0); /* green */
    SET_COLORV(facet_colors[3].colr, 0, 1, 1); /* cyan */
    SET_COLORV(facet_colors[4].colr, 0, 0, 1); /* blue */
    SET_COLORV(facet_colors[5].colr, 1, 0, 1); /* magenta */
    SET_COLORV(facet_colors[6].colr, 1, 1, 1); /* white */

    popen_struct( STRUCTURE_ID );
	/* Set the interior style and shading method. */
	pset_int_style( PSTYLE_SOLID );

	/* Create the fill area set with data elements. */
	for ( i = 0; i < 7; i++ )
	    pfill_area_set3_data( PFACET_COLOUR, PEDGE_NONE,
		PVERT_COORD, PMODEL_RGB, &facet_colors[i],
		(Pint) 1, (Pedge_data_list *)NULL, &contours[i] );
    pclose_struct();

    /* Open the workstation and post the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure the structure is traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
