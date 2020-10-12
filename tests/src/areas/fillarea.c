/* fillarea.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define NUM_CURVE_SEGMENTS	40
#define X_START			0.1
#define X_SIZE			0.2
#define Y_START			0.75
#define Y_SIZE			0.05

static Ppoint	chevron[]  = {{.1,.3},{.2,.35},{.3,.3},{.2,.5}};
static Ppoint	octagon[]  = {{.4,.45},{.45,.4},{.55,.4},
		    {.6,.45},{.6,.55},{.55,.6},{.45,.6},{.4,.55}};
static Ppoint	keyhole[]  = {{.7,.3},{.9,.3},{.9,.5},{.7,.5},
		    {.8,.35},{.84,.4},{.8,.45}};
static Ppoint3	bowtie[] = {{.7,.8,.5},{.7,.7,.5},{.9,.8,.5},
		    {.9,.7,.5}};

main()
{
    Ppoint		curve[NUM_CURVE_SEGMENTS + 1];
    Pgcolr		color;
    Ppoint_list		point_list;
    Ppoint_list3	point_list3;
    double		dx, angle;
    int			i;

    /* Generate the points for the sine curve. */
    dx = X_SIZE / NUM_CURVE_SEGMENTS;
    curve[0].x = X_START; curve[0].y = Y_START;
    for ( i = 1; i <= NUM_CURVE_SEGMENTS; i++ ) {
	curve[i].x = curve[i-1].x + dx;
	angle = ((curve[i].x - X_START) / X_SIZE) * 2 * M_PI;
	curve[i].y = Y_START + Y_SIZE * sin( angle );
    }

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* Set the interior style and color. */
	pset_int_style( PSTYLE_SOLID );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 1.0;
	pset_int_colr( &color );

	/* Distinguish between front and back faces. */
	pset_face_disting_mode( PDISTING_YES );

	/* Set the back interior style and color. */
	pset_back_int_style( PSTYLE_SOLID );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 0.0;
	pset_back_int_colr( &color );

	/* Create the fill area element for the curve. */
	point_list.points = curve;
	point_list.num_points = sizeof(curve)/sizeof(curve[0]);
	pfill_area( &point_list );

	/* Create the fill area element for the chevron. */
	point_list.points = chevron;
	point_list.num_points = sizeof(chevron)/sizeof(chevron[0]);
	pfill_area( &point_list );

	/* Create the fill area element for the octagon. */
	point_list.points = octagon;
	point_list.num_points = sizeof(octagon)/sizeof(octagon[0]);
	pfill_area( &point_list );

	/* Create the fill area element for the key hole. */
	point_list.points = keyhole;
	point_list.num_points = sizeof(keyhole)/sizeof(keyhole[0]);
	pfill_area( &point_list );

	/* Create a 3D fill area for the bow tie. */
	point_list3.points = bowtie;
	point_list3.num_points = sizeof(bowtie)/sizeof(bowtie[0]);
	pfill_area3( &point_list3 );
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
