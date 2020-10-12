/* pvt3.c 1.2 */
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
static void	pan_3D(), zoom_3D();

/* Utility macros. */
#define SET_POINT( _p, _x, _y, _z ) \
    (_p.x = _x, _p.y = _y, _p.z = _z)

#define SET_VECTOR( _v, _x, _y, _z ) \
    (_v.delta_x = _x, _v.delta_y = _y, _v.delta_z = _z)

main( argc, argv )
    int		argc;
    char	*argv[];
{
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
    pclose_struct();

    /* Calculate the view orientation matrix. */
    view_ref_pt.x = 0.5;
    view_ref_pt.y = 0.5;
    view_ref_pt.z = 0.5;
    view_up_vec.delta_x = 0;
    view_up_vec.delta_y = 1;
    view_up_vec.delta_z = 0;
    view_plane_normal.delta_x = 1;
    view_plane_normal.delta_y = 0.1;
    view_plane_normal.delta_z = 1;
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &error, view_rep.ori_matrix );

    /* Calculate the view mapping matrix (). */
    view_map.proj_type = PTYPE_PARAL;
    view_map.view_plane = 0.0;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 10;
    view_map.win.x_min = -1; view_map.win.x_max = 1;
    view_map.win.y_min = -1; view_map.win.y_max = 1;
    view_map.back_plane = -1; view_map.front_plane = 1;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;
    peval_view_map_matrix3( &view_map, &error, view_rep.map_matrix );

    /* Store the view in the view table as view 1. */
    view_rep.clip_limit = view_map.proj_vp;
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = view_rep.back_clip = PIND_CLIP;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );

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
    int			done = 0;

    XSelectInput( display, window,
	ButtonPressMask | Button1MotionMask | ExposureMask );
    while ( !done ) {
	XNextEvent( display, &event );
	if ( event.type == ButtonPress ) {
	    switch ( event.xbutton.button ) {
		case Button1:
		    pan_3D( WS_ID, (Pint) 1, &event, &view_ref_pt,
			&view_plane_normal, &view_up_vec );
		    break;
		case Button2:
		    zoom_3D( WS_ID, (Pint) 1, 0.1, &event, &view_map );
		    break;
		default:
		    done = 1;
		    break;
	    }

	} else if ( event.type == MotionNotify ) {
		pan_3D( WS_ID, (Pint) 1, &event, &view_ref_pt,
		    &view_plane_normal, &view_up_vec );
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
#define PAN_OVER_SCALE	2.0/400

static void
pan_3D( ws_id, index, event, vrp, vpn, vup )
    Pint	ws_id;
    Pint	index;
    XEvent	*event;
    Ppoint3	*vrp;
    Pvec3	*vpn;
    Pvec3	*vup;
{
    int		i, j;
    Pint	error;
    Pupd_st	upd_st;
    Pview_rep3	cur_view, req_view;
    Ppoint3	movement, shift;

    static int		lastx, lasty;
    static float	theta, phi;
    static Pmatrix3	inv_ori;

    switch ( event->type ) {

	case ButtonPress:
	    /* Initialize */
	    lastx = event->xbutton.x;
	    lasty = event->xbutton.y;

	    /* Compute the current view plane normal angles. */
	    theta = atan2( vpn->delta_x, vpn->delta_z );
	    phi = atan2( vpn->delta_y, hypot(vpn->delta_x, vpn->delta_z) );
	    break;

	case MotionNotify:
	    /* Get the current view definition. */
	    pinq_view_rep( ws_id, index, &error, &upd_st,
		&cur_view, &req_view);
	    if ( error ) return;

	    if ( event->xbutton.state & ShiftMask ) {
		/* Pan over the view plane.  See the text for a description
		 * of how the new view reference point is determined.
		 */
		inv_ori[3][3] = 1;
		for ( i = 0; i < 3; i++ )
		    for ( j = 0; j < 3; j++ )
			inv_ori[i][j] = req_view.ori_matrix[j][i];
			movement.x = PAN_OVER_SCALE * (event->xmotion.x - lastx);
			movement.y = PAN_OVER_SCALE * (lasty - event->xmotion.y);
			movement.z = 0;
			ptran_point3( &movement, inv_ori, &error, &shift );
			vrp->x += shift.x; vrp->y += shift.y; vrp->z += shift.z;
		
	    } else {
		/* Pan around by moving the view plane normal. */
		/* Increment the longitude and latitude. */
		theta += PAN_SCALE * (event->xmotion.x - lastx);
		phi += PAN_SCALE * (lasty - event->xmotion.y);

		/* Reset the view plane normal. */
		vpn->delta_x = cos( phi ) * sin( theta );
		vpn->delta_y = sin( phi );
		vpn->delta_z = cos( phi ) * cos( theta );
	    }

	    /* Calculate the new orientation matrix. */
	    peval_view_ori_matrix3( vrp, vpn, vup, &error,
	    req_view.ori_matrix );
	    if ( error ) return;

	    /* Set the view. */
	    pset_view_rep3( WS_ID, (Pint) 1, &req_view );

	    lastx = event->xmotion.x;
	    lasty = event->xmotion.y;
	    break;
    }
}

static void
zoom_3D( ws_id, index, zoom_factor, event, view_map )
    Pint	ws_id;
    Pint	index;
    float	zoom_factor;
    XEvent	*event;
    Pview_map3	*view_map;
{
    Pint	error;
    Pupd_st	upd_st;
    Pview_rep3	cur_view, req_view;
    float	vw_width, vw_height;

    /* Compute the new view window. */
    vw_width = view_map->win.x_max - view_map->win.x_min;
    vw_height = view_map->win.y_max - view_map->win.y_min;
    zoom_factor *= 0.5; /* divide the change evenly between sides. */
    if ( event->xbutton.state & ShiftMask ) {
	/* Zoom out. */
	view_map->win.x_min -= zoom_factor * vw_width;
	view_map->win.x_max += zoom_factor * vw_width;
	view_map->win.y_min -= zoom_factor * vw_height;
	view_map->win.y_max += zoom_factor * vw_height;
    } else {
	/* Zoom in. */
	view_map->win.x_min += zoom_factor * vw_width;
	view_map->win.x_max -= zoom_factor * vw_width;
	view_map->win.y_min += zoom_factor * vw_height;
	view_map->win.y_max -= zoom_factor * vw_height;
    }

    /* Get the current view definition. */
    pinq_view_rep( ws_id, index, &error, &upd_st,
	&cur_view, &req_view);
    if ( error ) return;

    /* Calculate the new mapping matrix. */
    peval_view_map_matrix3( view_map, &error, req_view.map_matrix );
    if ( error ) return;

    /* Set the view.  */
    pset_view_rep3( WS_ID, (Pint) 1, &req_view );
}
