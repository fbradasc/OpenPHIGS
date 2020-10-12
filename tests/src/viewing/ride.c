/* ride.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_GEOMETRY	"400x400+50+50"
#define WS_ID		(Pint) 1
#define STRUCT_ID	(Pint) 1
#define HEIGHT		0.35
#define EYE_LEVEL	0.05
#define SURF_PTS	51
#define TRACK_PTS	101
#define NO_COLOR	(Pint) -1

/* Utility macros. */
#define SET_POINT( _p, _x, _y, _z ) \
    (_p.x = _x, _p.y = _y, _p.z = _z)

#define SET_VECTOR( _v, _x, _y, _z ) \
    (_v.delta_x = _x, _v.delta_y = _y, _v.delta_z = _z)

#define TRACK_POINT( _t, _p ) \
    (_p.x =  2 * cos( _t ), _p.z =  2 * sin( _t ), \
	_p.y = HEIGHT * sin( _p.x ) * cos ( _p.z ))

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Display		*display;
    Window		window;
    XEvent		event;
    float		theta, delta, X, Z;
    Pint		error;
    int			i, j, index;
    Pvec3		vup, vpn;
    Pview_rep3		view;
    Pview_map3		view_map;
    Ppoint3		vrp, last_point, track[TRACK_PTS],
			surface[SURF_PTS * (SURF_PTS + 1)];
    Ppoint_list3	point_list;
    Pfacet_vdata_arr3	surface_vdata;
    Pint_size		array_dims;

    /* Open PHIGS and create a workstation.  Use the utility function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Generate the points for the surface. */
    delta = 2 * M_PI / (SURF_PTS - 1);
    for ( i = 0, X = -M_PI; i < SURF_PTS; i++, X += delta ) {
	for ( j = 0, Z = -M_PI; j < SURF_PTS; j++, Z += delta ) {
	    index = i * SURF_PTS + j;
	    surface[index].x = X;
	    surface[index].z = Z;
	    surface[index].y = HEIGHT * sin( X ) * cos ( Z );
	}
    }

    /* Generate the points for the track. */
    delta = 2 * M_PI / (TRACK_PTS - 1);
    for ( i = 0, theta = 0; i < TRACK_PTS; i++, theta += delta )
	TRACK_POINT( theta, track[i] );

    /* Build and store the surface and track. */
    popen_struct( STRUCT_ID );
	pset_view_ind( (Pint) 1 );
	surface_vdata.points = surface;
	array_dims.size_x = array_dims.size_y = SURF_PTS;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &surface_vdata );
	point_list.num_points = TRACK_PTS;
	point_list.points = track;
	pset_line_colr_ind( (Pint) 2 );
	ppolyline3( &point_list );
    pclose_struct();

    /* Set the view parameters that remain unchanged.  They are:
     *	view_up_vector:		points in the Y direction,
     *	projection_ref_point:	at the view reference point (vrp)
     *	back_plane:		5 km ahead of us,
     *	front_plane:		in front of our eyes (10 cm from vrp),
     *	view_plane:		at the front plane,
     *	view_window:		30 cm wide, centered w.r.t. the vrp,
     *	projection viewport:	all of NPC,
     *	projection type:	perspective.
     */
    SET_VECTOR( vup, 0, 1, 0 );
    SET_POINT( view_map.proj_ref_point, 0, 0, 0 );
    view_map.back_plane = -5;
    view_map.view_plane = -0.0001;
    view_map.front_plane = view_map.view_plane;
    view_map.win.x_min = -0.00015; view_map.win.x_max = 0.00015;
    view_map.win.y_min = -0.00015; view_map.win.y_max = 0.00015;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;
    view_map.proj_type = PTYPE_PERSPECT;
    peval_view_map_matrix3( &view_map, &error, view.map_matrix );

    /* Set the clipping parameters. */
    view.xy_clip = view.front_clip = PIND_CLIP;
    view.back_clip = PIND_NO_CLIP;
    view.clip_limit = view_map.proj_vp;

    /* Set the update state for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Post the top-level structure. */
    ppost_struct( WS_ID , STRUCT_ID, (Pfloat) 1.0 );

    /* Request button events to. */
    XSelectInput( display, window, ButtonPressMask );

    /* Start the ride. */
    delta = 2 * M_PI/1000;
    TRACK_POINT( -delta, last_point );
    for ( theta = 0; theta < 20*M_PI; theta += delta ) {
	/* Move the view reference point. */
	TRACK_POINT( theta, vrp );
	vrp.y += EYE_LEVEL;

	/* Update the view plane normal. */
	vpn.delta_x = last_point.x - vrp.x;
	vpn.delta_y = last_point.y - vrp.y;
	vpn.delta_z = last_point.z - vrp.z;
	last_point = vrp;

	/* Compute the orientation matrix and set the view. */
	peval_view_ori_matrix3( &vrp, &vpn, &vup, &error,
	    view.ori_matrix );
	if ( !error ) {
	    pset_view_rep3( WS_ID, (Pint) 1, &view );
	    pupd_ws( WS_ID, PFLAG_PERFORM );
	}

	/* See if we should change the speed. */
	if ( error || XPending( display ) ) {
	    XNextEvent( display, &event );
	    if ( event.type == ButtonPress ) {
		if ( event.xbutton.button == Button3 )
		    break; /* stop the ride */
		else if ( event.xbutton.state & ShiftMask )
		    /* "Stop the ride.  I want to get off." */
		    delta = delta > M_PI/1000 ? delta - M_PI/10000 : 0;
		else
		    delta += M_PI/10000; /* speed up */
	    }
	}
    }

    pclose_ws( WS_ID );
    pclose_phigs();
    XCloseDisplay( display );
    return 0;
}
