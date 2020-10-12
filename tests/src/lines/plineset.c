/* plineset.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

/* Cube coordinates. */
static Ppoint3	top[] = {{.3,.7,.3}, {.3,.7,.7}, {.7,.7,.7},
			{.7,.7,.3}, {.3,.7,.3}};
static Ppoint3	bottom[] = {{.3,.3,.3}, {.3,.3,.7}, {.7,.3,.7},
			{.7,.3,.3}, {.3,.3,.3}};
static Ppoint3	front[] = {{.3,.7,.7}, {.3,.3,.7}, {.7,.3,.7},
			{.7,.7,.7}, {.3,.7,.7}};
static Ppoint3	back[] = {{.3,.7,.3}, {.3,.3,.3}, {.7,.3,.3},
			{.7,.7,.3}, {.3,.7,.3}};

static int set_view();

main()
{
    Pptco3		x_axis[2], y_axis[2], z_axis[2];
    Pline_vdata_list3	axes[3], cube[4];

    /* Set the axes coordinates and colors. */
    x_axis[0].point.x = x_axis[0].point.y = x_axis[0].point.z = 0.0;
    x_axis[1].point.x = 0.2;
    x_axis[1].point.y = 0.0;
    x_axis[1].point.z = 0.0;
    x_axis[0].colr.direct.rgb.red = 1.0;
    x_axis[0].colr.direct.rgb.green = 0.0;
    x_axis[0].colr.direct.rgb.blue = 0.0;
    x_axis[1].colr.direct.rgb = x_axis[0].colr.direct.rgb;

    y_axis[0].point.x = y_axis[0].point.y = y_axis[0].point.z = 0.0;
    y_axis[1].point.x = 0.0;
    y_axis[1].point.y = 0.2;
    y_axis[1].point.z = 0.0;
    y_axis[0].colr.direct.rgb.red = 0.0;
    y_axis[0].colr.direct.rgb.green = 1.0;
    y_axis[0].colr.direct.rgb.blue = 0.0;
    y_axis[1].colr.direct.rgb = y_axis[0].colr.direct.rgb;

    z_axis[0].point.x = z_axis[0].point.y = z_axis[0].point.z = 0.0;
    z_axis[1].point.x = 0.0;
    z_axis[1].point.y = 0.0;
    z_axis[1].point.z = 0.2;
    z_axis[0].colr.direct.rgb.red = 0.0;
    z_axis[0].colr.direct.rgb.green = 0.0;
    z_axis[0].colr.direct.rgb.blue = 1.0;
    z_axis[1].colr.direct.rgb = z_axis[0].colr.direct.rgb;

    /* Build the polyline set for the axes. */
    axes[0].vertex_data.ptcolrs = x_axis; axes[0].num_vertices = 2;
    axes[1].vertex_data.ptcolrs = y_axis; axes[1].num_vertices = 2;
    axes[2].vertex_data.ptcolrs = z_axis; axes[2].num_vertices = 2;

    /* Build the polyline set for the cube. */
    cube[0].vertex_data.points = top;    cube[0].num_vertices = 5;
    cube[1].vertex_data.points = bottom; cube[1].num_vertices = 5;
    cube[2].vertex_data.points = front;	 cube[2].num_vertices = 5;
    cube[3].vertex_data.points = back;	 cube[3].num_vertices = 5;
    
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* Set the line attributes. */
	pset_linewidth( (Pfloat) 2.0 );	/* linewidth scale factor */
	pset_line_colr_ind( (Pint) 1 );	/* line color */
	pset_linetype( PLINE_SOLID );	/* linetype */
	pset_line_shad_meth( PSD_NONE );/* polyline shading method */

	/* Set the view index to the view we'll use. */
	pset_view_ind( (Pint) 1 );

	/* The polyline set for the axes. */
	ppolyline_set3_data( PVERT_COORD_COLOUR, PMODEL_RGB,
	    (Pint) 3, axes );

	/* The polyline set for the cube. */
	ppolyline_set3_data( PVERT_COORD, (Pint) 0, (Pint) 4, cube );
    pclose_struct();

    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    set_view();	/* This function is defined below. */

    /* Post the structure. */
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure the structures are traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 20 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static int
set_view()
{
    Pview_rep3		view_rep;
    Pview_map3		view_map;
    Pint		err;

    /* The view orientation parameters. */
    static Ppoint3	view_ref_pt = {0.5,0.5,0.5};
    static Pvec3	view_plane_normal = {2,0.5,0.5};
    static Pvec3	view_up_vec = {0,1,0};

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &err, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PARAL;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 20.0;
    view_map.win.x_min = -0.5; view_map.win.x_max = 0.5;
    view_map.win.y_min = -0.5; view_map.win.y_max = 0.5;
    view_map.back_plane = -100.0; view_map.front_plane = 100.0;
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
