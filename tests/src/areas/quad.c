/* quad.c 1.3 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* Miscellaneous constants we use. */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define NUM_X_INTERVALS		20
#define NUM_Z_INTERVALS		20
#define NUM_X_POINTS		(NUM_X_INTERVALS + 1)
#define NUM_Z_POINTS		(NUM_Z_INTERVALS + 1)
#define X0			0.5
#define Z0			0.5
#define NO_COLOR		(Pint) 0

/* Utility functions defined below. */
static void	set_view(), build_axes();

main()
{
    Ppoint3		vertices[NUM_X_POINTS * NUM_Z_POINTS];
    Pfacet_vdata_arr3	vertex_data;
    Pint_size		array_dims;
    Pgcolr		color;
    float		x, dx, z, dz;
    int			i, j, index;

    /* Generate the points for the surface. */
    dx = 1.0 / (NUM_X_INTERVALS);
    dz = 1.0 / (NUM_Z_INTERVALS);
#ifdef SunPHIGS2.0
    for ( i = 0, x = 0.0; i < NUM_X_POINTS; i++, x += dx ) {
	for ( j = 0, z = 0.0; j < NUM_Z_POINTS; j++, z += dz ) {
	    index = i * NUM_X_POINTS + j;
#else
    for ( j = 0, z = 0.0; j < NUM_Z_POINTS; j++, z += dz ) {
	for ( i = 0, x = 0.0; i < NUM_X_POINTS; i++, x += dx ) {
	    index = j * NUM_X_POINTS + i;
#endif
	    vertices[index].x = x;
	    vertices[index].y = (x - X0) * (x - X0) + (z - Z0) * (z - Z0);
	    vertices[index].z = z;
	}
    }

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* Set the view index to the view we build below. */
	pset_view_ind( (Pint) 1 );

	/* Create the axes. */
	build_axes( 0.2 ); /* this function defined below */

	/* Distinguish between front and back faces. */
	pset_face_disting_mode( PDISTING_YES );

	/* Set the interior style and color for the front faces. */
	pset_int_style( PSTYLE_HOLLOW );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 1.0;
	pset_int_colr( &color );

	/* Set the interior style and color for the back faces. */
	pset_back_int_style( PSTYLE_SOLID );
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 1.0;
	pset_back_int_colr( &color );

	/* Create the quadrilateral mesh element. */
	vertex_data.points = vertices;
	array_dims.size_x = NUM_X_POINTS;
	array_dims.size_y = NUM_Z_POINTS;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertex_data );
    pclose_struct();

    /* Open the workstation, set a view, and post the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    set_view();	/* This function is defined below. */
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure the structure is traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
set_view()
{
    Pview_rep3		view_rep;
    Pview_map3		view_map;
    Pint		err;

    /* The view orientation parameters. */
    static Ppoint3	view_ref_pt = {0.5,0.5,0.5};
    static Pvec3	view_up_vec = {0,1,0};
    static Pvec3	view_plane_normal = {-0.5,0.5,1};

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &err, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PERSPECT;
    view_map.proj_ref_point.x = 1.0;
    view_map.proj_ref_point.y = 0.5;
    view_map.proj_ref_point.z = 10.0;
    view_map.win.x_min = -0.8; view_map.win.x_max = 0.8;
    view_map.win.y_min = -0.9; view_map.win.y_max = 0.7;
    view_map.back_plane = -1.0; view_map.front_plane = 5.0;
    view_map.view_plane = 0.0;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;

    /* Compute the view mapping matrix. */
    peval_view_map_matrix3( &view_map, &err, view_rep.map_matrix );

    /* Set the view representation. */
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = PIND_CLIP;
    view_rep.back_clip = PIND_CLIP;
    view_rep.clip_limit.x_min = 0.0; view_rep.clip_limit.x_max = 1.0;
    view_rep.clip_limit.y_min = 0.0; view_rep.clip_limit.y_max = 1.0;
    view_rep.clip_limit.z_min = 0.0; view_rep.clip_limit.z_max = 1.0;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
}

static void
build_axes( length )
    float		length;
{
    Pptco3		x_axis[2], y_axis[2], z_axis[2];
    Pline_vdata_list3	axes[3];

    /* Set the axes coordinates and colors. */
    x_axis[0].point.x = x_axis[0].point.y = x_axis[0].point.z = 0;
    x_axis[1].point.x = length;
    x_axis[1].point.y = 0;
    x_axis[1].point.z = 0;
    x_axis[0].colr.direct.rgb.red = 1;
    x_axis[0].colr.direct.rgb.green = 0;
    x_axis[0].colr.direct.rgb.blue = 0;
    x_axis[1].colr.direct.rgb = x_axis[0].colr.direct.rgb;

    y_axis[0].point.x = y_axis[0].point.y = y_axis[0].point.z = 0;
    y_axis[1].point.x = 0;
    y_axis[1].point.y = length;
    y_axis[1].point.z = 0;
    y_axis[0].colr.direct.rgb.red = 0;
    y_axis[0].colr.direct.rgb.green = 1;
    y_axis[0].colr.direct.rgb.blue = 0;
    y_axis[1].colr.direct.rgb = y_axis[0].colr.direct.rgb;

    z_axis[0].point.x = z_axis[0].point.y = z_axis[0].point.z = 0;
    z_axis[1].point.x = 0;
    z_axis[1].point.y = 0;
    z_axis[1].point.z = length;
    z_axis[0].colr.direct.rgb.red = 0;
    z_axis[0].colr.direct.rgb.green = 0;
    z_axis[0].colr.direct.rgb.blue = 1;
    z_axis[1].colr.direct.rgb = z_axis[0].colr.direct.rgb;

    /* Build a polyline set for the axes. */
    axes[0].num_vertices = 2;
    axes[1].num_vertices = 2;
    axes[2].num_vertices = 2;
    axes[0].vertex_data.ptcolrs = x_axis;
    axes[1].vertex_data.ptcolrs = y_axis;
    axes[2].vertex_data.ptcolrs = z_axis;
    ppolyline_set3_data( PVERT_COORD_COLOUR, PMODEL_RGB, (Pint) 3, axes );
}
