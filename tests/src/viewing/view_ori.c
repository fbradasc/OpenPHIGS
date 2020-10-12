/* view_ori.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_GEOMETRY	"500x500+50+50"
#define WS_ID		(Pint) 1
#define SPHERE		(Pint) 1
#define AXES		(Pint) 2
#define SPHERE_RADIUS	(Pfloat) 1.0
#define PAN_SCALE	(M_PI/500)

static Display		*display;
static Window		window;
static Pvec3		view_up_vec, view_plane_normal;
static Ppoint3		view_ref_pt;
static Pview_rep3	view_rep;
static Pview_map3	view_map;
static Pint		error;

/* Internal functions. */
static void	build_sphere(), handle_input(), build_axes();

main()
{
    Ppoint_list3	sphere;

    /* Open PHIGS and create a workstation.  Use the utility function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Create the AXES structure. (see for build_axes definition */
    build_axes( 0.4 );

    /* Build the sphere. (See for definition of build_sphere. */
    build_sphere( SPHERE_RADIUS, &sphere );
    popen_struct( SPHERE );
	pset_view_ind( (Pint) 1 );
	pexec_struct( AXES );
	ppolyline3( &sphere );
    pclose_struct();

    /* Calculate the initial view orientation matrix. */
    view_ref_pt.x = 0;
    view_ref_pt.y = 0;
    view_ref_pt.z = 0;
    view_up_vec.delta_x = 0;
    view_up_vec.delta_y = 1;
    view_up_vec.delta_z = 0;
    view_plane_normal.delta_x = 1;
    view_plane_normal.delta_y = 1;
    view_plane_normal.delta_z = 1;
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &error, view_rep.ori_matrix );

    /* Define the view mapping. */
    view_map.proj_type = PTYPE_PERSPECT;
    view_map.view_plane = 0.0;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 10;
    view_map.win.x_min = -1.5; view_map.win.x_max = 1.5;
    view_map.win.y_min = -1.5; view_map.win.y_max = 1.5;
    view_map.back_plane = -1.5; view_map.front_plane = 1.5;
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

    /* Post the structure. */
    ppost_struct( WS_ID, SPHERE, (Pfloat) 1.0 );

    /* Read and respond to X events. */
    handle_input();

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
handle_input()
{
    XEvent		event;
    int			lastx, lasty, done = 0;
    float		theta = M_PI/4, phi = M_PI/4;

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
		pset_view_rep3( WS_ID, (Pint) 1, &view_rep );

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

#define NUM_LAT_SEGS	20
#define NUM_LON_SEGS	30

static void
build_sphere( radius, sphere )
    float		radius;
    Ppoint_list3	*sphere;
{
    /* Build a "wire" sphere, using a single spiral polyline. */

    double	phi, delta_phi, theta, delta_theta;
    int		i;

    sphere->num_points = NUM_LAT_SEGS * NUM_LON_SEGS + 1;
    sphere->points = (Ppoint3 *)
	malloc( (unsigned) (sphere->num_points * sizeof(Ppoint3)) );

    phi = 0; /* start at the North pole */
    delta_phi = M_PI / (NUM_LAT_SEGS * NUM_LON_SEGS);

    theta = 0;
    delta_theta = (2 * M_PI) / NUM_LON_SEGS;

    for ( i = 0; i < sphere->num_points; i++ ) {
	sphere->points[i].x = radius * sin( phi ) * sin( theta );
	sphere->points[i].y = radius * cos( phi );
	sphere->points[i].z = radius * sin( phi ) * cos( theta );
	theta += delta_theta;
	phi += delta_phi;
    }
}

#define SET_COORD( _p, _x, _y, _z ) \
    (_p).point.x = (_x), (_p).point.y = (_y), (_p).point.z = (_z)

#define SET_COLOR( _p, _r, _g, _b ) \
    (_p).colr.direct.rgb.red = (_r), \
    (_p).colr.direct.rgb.green = (_g), \
    (_p).colr.direct.rgb.blue = (_b)

static void
build_axes( d )
    float	d;	/* axis length */
{
    Pline_vdata_list3	axes[3];
    Pptco3		x_axis[2], y_axis[2], z_axis[2];

    /* Set the axes coordinates and colors. */
    SET_COORD( x_axis[0], 0, 0, 0 ); SET_COORD( x_axis[1], d, 0, 0 );
    SET_COORD( y_axis[0], 0, 0, 0 ); SET_COORD( y_axis[1], 0, d, 0 );
    SET_COORD( z_axis[0], 0, 0, 0 ); SET_COORD( z_axis[1], 0, 0, d );

    SET_COLOR( x_axis[0], 1, 0, 0 );
    SET_COLOR( y_axis[0], 0, 1, 0 );
    SET_COLOR( z_axis[0], 0, 0, 1 );

    axes[0].num_vertices = 2; axes[0].vertex_data.ptcolrs = x_axis;
    axes[1].num_vertices = 2; axes[1].vertex_data.ptcolrs = y_axis;
    axes[2].num_vertices = 2; axes[2].vertex_data.ptcolrs = z_axis;
    popen_struct( AXES );
	ppolyline_set3_data( PVERT_COORD_COLOUR, PMODEL_RGB,
			(Pint) 3, axes );
    pclose_struct();
}
