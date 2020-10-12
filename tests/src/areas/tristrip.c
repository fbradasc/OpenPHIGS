/* tristrip.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define NUM_TRIANGLES		21
#define NUM_VERTICES		(NUM_TRIANGLES + 2)
#define X_START			0.2
#define X_END			0.8
#define Y0			0.7
#define Z0			0.5
#define STRIP_HALF_WIDTH	0.15
#define TWIST_ANGLE		M_PI
#define NO_COLOR		(Pint) 0

main()
{
    Ppoint3		vertices[NUM_TRIANGLES + 2];
    Pptco3		vertices_with_color[NUM_TRIANGLES + 2];
    Pfacet_vdata_arr3	vertex_data;
    Pgcolr		color;
    double		dx, delta, angle;
    int			i;

    /* Generate the points for the band. */
    dx = (X_END - X_START) / (NUM_VERTICES - 1);
    delta = TWIST_ANGLE / (NUM_VERTICES - 1);

    /* The top vertices. */
    for ( i = 0; i < NUM_VERTICES; i += 2 ) {
	vertices[i].x = X_START + i * dx;
	angle = i * delta;
	/* Compute the vertices for the hollow strip. */
	vertices[i].y = Y0 + STRIP_HALF_WIDTH * cos( angle );
	vertices[i].z = Z0 + STRIP_HALF_WIDTH * sin( angle );

	/* Set the coordinates and colors for the shaded strip. */
	vertices_with_color[i].point = vertices[i];
	vertices_with_color[i].point.y -= 0.4;
	vertices_with_color[i].colr.direct.rgb.red = 1.0;
	vertices_with_color[i].colr.direct.rgb.green = 0.0;
	vertices_with_color[i].colr.direct.rgb.blue = 0.0;
    }

    /* The bottom vertices. */
    for ( i = 1; i < NUM_VERTICES; i += 2 ) {
	vertices[i].x = X_START + i * dx;
	angle = i * delta;
	/* Compute the vertices for the hollow strip. */
	vertices[i].y = Y0 - STRIP_HALF_WIDTH * cos( angle );
	vertices[i].z = Z0 - STRIP_HALF_WIDTH * sin( angle );

	/* Set the coordinates and colors for the shaded strip. */
	vertices_with_color[i].point = vertices[i];
	vertices_with_color[i].point.y -= 0.4;
	vertices_with_color[i].colr.direct.rgb.red = 1.0;
	vertices_with_color[i].colr.direct.rgb.green = 1.0;
	vertices_with_color[i].colr.direct.rgb.blue = 1.0;
    }

    /* Make the ends of the band square. */
    vertices[1].x = X_START;
    vertices[NUM_VERTICES - 2].x = vertices[NUM_VERTICES - 1].x;
    vertices_with_color[1].point.x = X_START;
    vertices_with_color[NUM_VERTICES - 2].point.x
	= vertices[NUM_VERTICES - 1].x;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* Set the interior style and color for the hollow strip. */
	pset_int_style( PSTYLE_HOLLOW );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 1.0;
	pset_int_colr( &color );

	/* Distinguish between front and back faces. */
	pset_face_disting_mode( PDISTING_YES );

	/* Set the back interior style and color. */
	pset_back_int_style( PSTYLE_HOLLOW );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0; /* red + green = yellow */
	color.val.general.y = 1.0;
	color.val.general.z = 0.0;
	pset_back_int_colr( &color );

	/* Create the triangle strip element. */
	vertex_data.points = vertices;
	ptri_strip3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    (Pint) NUM_VERTICES, (Pfacet_data_arr3 *)NULL,
	    &vertex_data );


	/* Set the attributes for the shaded strip. */
	pset_int_style( PSTYLE_SOLID );
	pset_int_shad_meth( PSD_COLOUR ); /* shading method */
	/* Stop distinguishing between front and back faces. */
	pset_face_disting_mode( PDISTING_NO );

	/* Create the triangle strip element with vertex colors. */
	vertex_data.ptcolrs = vertices_with_color;
	ptri_strip3_data( PFACET_NONE, PVERT_COORD_COLOUR,
	    PMODEL_RGB, (Pint) NUM_VERTICES,
	    (Pfacet_data_arr3 *)NULL, &vertex_data );
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
