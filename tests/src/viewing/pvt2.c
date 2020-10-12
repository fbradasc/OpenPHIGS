/* pvt2.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_GEOMETRY	"300x300+50+50"
#define WS_ID		(Pint) 1
#define MODEL		(Pint) 1
#define LABELS		(Pint) 2
#define SURFACE		(Pint) 3
#define PAN_SCALE	(M_PI/500)

static Display		*display;
static Window		window;
static Pvec3		view_up_vec, view_plane_normal;
static Ppoint3		view_ref_pt;
static Pview_rep3	view_rep;
static Pview_map3	view_map;
static Pint		error;

/* Internal functions defined below. */
static void	create_ideal_gas_surface(), handle_input();
static int	set_view();

/* Utility macros. */
#define SET_POINT( _p, _x, _y, _z ) \
    (_p.x = _x, _p.y = _y, _p.z = _z)

#define SET_VECTOR( _v, _x, _y, _z ) \
    (_v.delta_x = _x, _v.delta_y = _y, _v.delta_z = _z)

/* Define the projection viewports for the four views. */
static Plimit3	vports[] = {
    /*	x_min,	x_max,	y_min,	y_max,	z_min,	z_max */
    {	0.5,	1,	0.5,	1,	0,	1}, /* view 1 */
    {	0.5,	1,	0,	0.5,	0,	1}, /* view 2 */
    {  	0,	0.5,	0,	0.5,	0,	1}, /* view 3 */
    {  	0,	0.5,	0.5,	1,	0,	1}, /* view 4 */
    };

main( argc, argv )
    int		argc;
    char	*argv[];
{
    int			i;
    Ppoint		pts[5];
    Ppoint_list		point_list;

    /* Open PHIGS and create a workstation.  Use the utility function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Build and store the ideal gas surface. */
    create_ideal_gas_surface(); /* defined below */

    /* Create a top-level structure and execute the surface from
     * there, setting the view index first.
     */
    popen_struct( MODEL );
	pset_view_ind( (Pint) 1 );
	pexec_struct( SURFACE );
	pset_view_ind( (Pint) 2 );
	pexec_struct( SURFACE );
	pset_view_ind( (Pint) 3 );
	pexec_struct( SURFACE );
	pset_view_ind( (Pint) 4 );
	pexec_struct( SURFACE );

	/* Draw the view borders. */
	pset_view_ind( (Pint) 0 );
	point_list.num_points = 5;
	point_list.points = pts;
	for ( i = 0; i < 4; i++ ) {
	    pts[0].x = vports[i].x_min; pts[0].y = vports[i].y_min;
	    pts[1].x = vports[i].x_max; pts[1].y = vports[i].y_min;
	    pts[2].x = vports[i].x_max; pts[2].y = vports[i].y_max;
	    pts[3].x = vports[i].x_min; pts[3].y = vports[i].y_max;
	    pts[4].x = vports[i].x_min; pts[4].y = vports[i].y_min;
	    ppolyline( &point_list );
	}
    pclose_struct();

    /* Set the initial view parameters. */
    SET_POINT( view_ref_pt, 0.5, 0.5, 0.5 );
    SET_VECTOR( view_up_vec, 0, 1, 0 );

    view_map.proj_type = PTYPE_PARAL;
    view_map.view_plane = 0.0;
    SET_POINT( view_map.proj_ref_point, 0, 0, 10 );
    view_map.win.x_min = -1; view_map.win.x_max = 1;
    view_map.win.y_min = -1; view_map.win.y_max = 1;
    view_map.back_plane = -1; view_map.front_plane = 1;

    /* Right-side view - view 2. Put it in lower-right corner. */
    SET_VECTOR( view_plane_normal, 1, 0, 0 );
    view_map.proj_vp = vports[1];
    set_view( WS_ID, (Pint) 2, &view_ref_pt,
	&view_plane_normal, &view_up_vec, &view_map );

    /* Front view - view 3. Put it in lower-left corner. */
    SET_VECTOR( view_plane_normal, 0, 0, 1 );
    view_map.proj_vp = vports[2];
    set_view( WS_ID, (Pint) 3, &view_ref_pt,
	&view_plane_normal, &view_up_vec, &view_map );

    /* Top view - view 4. Put it in upper-left corner. */
    SET_VECTOR( view_up_vec, 0, 0, -1 );
    SET_VECTOR( view_plane_normal, 0, 1, 0 );
    view_map.proj_vp = vports[3];
    set_view( WS_ID, (Pint) 4, &view_ref_pt,
	&view_plane_normal, &view_up_vec, &view_map );

    /* Off-axis view - view 1. Put it in upper-right corner. */
    view_map.proj_type = PTYPE_PERSPECT;
    SET_VECTOR( view_up_vec, 0, 1, 0 );
    SET_VECTOR( view_plane_normal, 1, 0.1, 1 );
    view_map.proj_vp = vports[0];
    set_view( WS_ID, (Pint) 1, &view_ref_pt,
	&view_plane_normal, &view_up_vec, &view_map );

    /* Set the update state for automatic updates. */
    pset_disp_upd_st( WS_ID, PDEFER_ASAP, PMODE_UQUM );

    /* Post the top-level structure. */
    ppost_struct( WS_ID, MODEL, (Pfloat) 1.0 );

    /* Read and respond to X events. */
    handle_input();

    pclose_ws( WS_ID );
    pclose_phigs();
    XCloseDisplay( display );
    return 0;
}

static void
handle_input()
{
    XEvent		event;
    int			lastx, lasty, done = 0;
    float		theta = M_PI/4, phi = 0.0706;

    XSelectInput( display, window,
	ButtonPressMask | Button1MotionMask | ExposureMask );
    while ( !done ) {
	XNextEvent( display, &event );
	if ( event.type == ButtonPress ) {
	    switch ( event.xbutton.button ) {
		case Button1:
		    /* Reset last X and last Y. */
		    lastx = event.xbutton.x;
		    lasty = event.xbutton.y;
		    break;
		default:
		    done = 1;
		    break;
	    }

	} else if ( event.type == MotionNotify ) {
	    /* Increment the view plane normal angles. */
	    theta += PAN_SCALE * (event.xmotion.x - lastx);
	    phi += PAN_SCALE * (lasty - event.xmotion.y);

	    /* Reset the view plane normal. */
	    view_plane_normal.delta_x = cos( phi ) * sin( theta );
	    view_plane_normal.delta_y = sin( phi );
	    view_plane_normal.delta_z = cos( phi ) * cos( theta );

	    /* Calculate the new orientation matrix. */
	    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
		&view_up_vec, &error, view_rep.ori_matrix );

	    /* Set the view if orientation is okay. */
	    if ( !error )
		set_view( WS_ID, (Pint) 1, &view_ref_pt,
		    &view_plane_normal, &view_up_vec, &view_map );

		lastx = event.xmotion.x;
		lasty = event.xmotion.y;
	    } else switch ( event.type ) {
		case Expose:
		    if ( event.xexpose.count == 0 )
			predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		    break;
	    }
    }
}

#define NUM_POINTS		13
#define TMAX			1000.0
#define VMAX			1.0
#define BASE_HEIGHT		0.1
#define NO_COLOR		(Pint) -1

static Ppoint3	base_vertices[2][4] = {
	{{0,0,1}, {1,0,1}, {1,BASE_HEIGHT,1}, {0,BASE_HEIGHT,1}},
	{{1,0,1}, {1,0,0}, {1,BASE_HEIGHT,0}, {1, BASE_HEIGHT, 1}}};

static void
create_ideal_gas_surface()
{
    Pint		error;
    Pint_size		array_dims;
    Ppoint3		surface[NUM_POINTS * (NUM_POINTS + 1)], text_pt;
    Pvec		anno_up_vec;
    Pvec3		scale, translate, text_dir[2], offset;
    Pmatrix3		xscale, xtranslate, xform;
    Ptext_align		text_align;
    Ppoint_list3	point_list;
    Pfacet_vdata_arr3	surface_vdata;
    float		V, dV, T, dT, maxP;
    int			i, j, index;

    /* Generate the points for the surface: P = T/V. */
    dV = VMAX / (NUM_POINTS - 1);
    dT = TMAX / (NUM_POINTS - 1);
    maxP = 4 * TMAX / VMAX;
    for ( i = 0, V = 0; i < NUM_POINTS + 1; i++, V += dV ) {
	for ( j = 0, T = 0; j < NUM_POINTS; j++, T += dT ) {
	    index = i * NUM_POINTS + j;
	    surface[index].x = V;
	    surface[index].z = TMAX - T;
	    if ( V == 0 ) {
		/* Clamp 0 volume to maximum pressure. */
		surface[index].y = maxP;
	    } else if ( i == NUM_POINTS ) {
		/* Truncate last row to 0 pressure. */
		surface[index].x = VMAX;
		surface[index].y = 0;
	    } else {
		/* Compute the pressure. */
		surface[index].y = T / V;
		if ( surface[index].y > maxP )
		    surface[index].y = maxP;
	    }
	}
    }

    popen_struct( LABELS );
	/* Border the volume and temperature labels. */
	point_list.num_points = 4;
	for ( i = 0; i < 2; i++ ) {
	    point_list.points = base_vertices[i];
	    pfill_area3( &point_list );
	}

	/* Create the labels. */
	pset_char_ht( (Pfloat) 0.05 );
	text_align.hor = PHOR_CTR; text_align.vert = PVERT_HALF;
	pset_text_align( &text_align );

	SET_POINT( text_pt, 0.5, 0.05, 1 );
	SET_VECTOR( text_dir[0], 1, 0, 0 );
	SET_VECTOR( text_dir[1], 0, 1, 0 );
	ptext3( &text_pt, text_dir, "Volume" );

	SET_POINT( text_pt, 1, 0.05, 0.5 );
	SET_VECTOR( text_dir[0], 0, 0, -1 );
	ptext3( &text_pt, text_dir, "Temperature" );

	/* Use annotation text for the "Pressure" label. */
	pset_anno_char_ht( 0.02 );
	anno_up_vec.delta_x = -1; anno_up_vec.delta_y = 0;
	pset_anno_align( &text_align );
	pset_anno_char_up_vec( &anno_up_vec );
	SET_POINT( text_pt, 0.05, 0.5, 0.95 );
	offset.delta_x = offset.delta_y = offset.delta_z = 0;
	panno_text_rel3( &text_pt, &offset, "Pressure" );
    pclose_struct();

    popen_struct( SURFACE );
	pexec_struct( LABELS );

	/* Scale the surface into a reasonable volume and center it. */
	SET_VECTOR( scale, 0.8/VMAX, 0.8/maxP, 0.8/TMAX );
	pscale3( &scale, &error, xscale );
	SET_VECTOR( translate, 0.1, 0.1, 0.1 );
	ptranslate3( &translate, &error, xtranslate );
	pcompose_matrix3( xtranslate, xscale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );

	/* Create the quadrilateral mesh element. */
	surface_vdata.points = surface;
	array_dims.size_x = NUM_POINTS;
	array_dims.size_y = NUM_POINTS + 1;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &surface_vdata );
    pclose_struct();
}

static int
set_view( ws_id, index, vrp, vpn, vup, view_map )
    Pint	ws_id;		/* workstation identifier */
    Pint	index;		/* view index */
    Ppoint3	*vrp;		/* view reference point */
    Pvec3	*vpn;		/* view plane normal */
    Pvec3	*vup;		/* view up vector */
    Pview_map3	*view_map;	/* view mapping parameters */
{
    Pint	error;
    Pview_rep3	view;

    /* Calculate the view orientation matrix. */
    peval_view_ori_matrix3( vrp, vpn, vup, &error, view.ori_matrix );
    if ( error ) return error;

    /* Calculate the view mapping matrix. */
    peval_view_map_matrix3( view_map, &error, view.map_matrix );
    if ( error ) return error;

    view.xy_clip = PIND_CLIP;
    view.front_clip = view.back_clip = PIND_CLIP;
    view.clip_limit = view_map->proj_vp;
    pset_view_rep3( ws_id, index, &view );
    return error;
}
