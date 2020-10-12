/* mclip.c 1.2 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_GEOMETRY		"500x500+200+200"
#define WS_ID			(Pint) 1
#define MODEL			(Pint) 1
#define AXES			(Pint) 2
#define SPHERE			(Pint) 3
#define ROTATION_XFORM		(Pint) 1
#define LOCAL_MCLIP_VOLUME	(Pint) 2

/* Utility functions defined below. */
static void	build_sphere(), build_axes(), open_xlib_ws(),
		handle_input(), set_view();

static Phalf_space3		half_space;
static Phalf_space_list3	half_space_list;

main()
{
    Ppoint_list3	sphere;
    Ppoint3		center;
    Pmatrix3		transform;
    Pint		error;
    Display		*display;
    Window		window;

    /* Open PHIGS and create a workstation.  Use the function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Create the AXES structure. */
    build_axes( 0.15 );		/* function is defined below */

    popen_struct( MODEL );
	pset_view_ind( (Pint) 1 ); /* select the view we use. */
	pexec_struct( AXES );

	/* Set the global model-clipping volume. One half-space. */
	pset_model_clip_ind( PIND_CLIP );
	half_space.point.x = 0;
	half_space.point.y = 0.6;
	half_space.point.z = 0;
	half_space.norm.delta_x = 0;
	half_space.norm.delta_y = -1;
	half_space.norm.delta_z = 0;
	half_space_list.num_half_spaces = 1;
	half_space_list.half_spaces = &half_space;
	pset_model_clip_vol3( PMC_REPLACE, &half_space_list );

	/* Position the sphere and execute it. */
	center.x = center.y = center.z = 0.5;
	ptranslate3( &center, &error, transform );
	pset_local_tran3( transform, PTYPE_REPLACE );
	pexec_struct( SPHERE );
    pclose_struct();

    popen_struct( SPHERE );
	/* Store a transform element that we can change later to rotate
	 * the sphere.
	 */
	protate_x( (Pfloat) 0, &error, transform );
	plabel( ROTATION_XFORM );
	pset_local_tran3( transform, PTYPE_REPLACE );

	/* Draw the modeling coordinate axes at center of sphere. */
	pexec_struct( AXES );

	/* Set the local model-clipping volume. One half-space. */
	half_space.point.x = 0.15;
	half_space.point.y = 0;
	half_space.point.z = 0;
	half_space.norm.delta_x = -1;
	half_space.norm.delta_y = 0;
	half_space.norm.delta_z = 0;
	half_space_list.num_half_spaces = 1;
	half_space_list.half_spaces = &half_space;
	plabel( LOCAL_MCLIP_VOLUME );
	pset_model_clip_vol3( PMC_INTERSECT, &half_space_list );

	/* Turn model-clipping on for subsequent primitive. */
	pset_model_clip_ind( PIND_CLIP );

	/* Create the sphere. */
	build_sphere( 0.25, &sphere );
	ppolyline3( &sphere );
    pclose_struct();

    set_view();		/* function is defined below. */
    ppost_struct( WS_ID, MODEL, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Go handle X events. */
    handle_input( display, window );

    pclose_ws( WS_ID );
    pclose_phigs();
    XCloseDisplay( display );
    return 0;
}
#define ANGLE_SCALE	(M_PI/200)
#define CLIP_SCALE	(0.01)

static void
handle_input( display, window )
    Display	*display;
    Window	window;
{
    XEvent		event;
    int			lastx, lasty, done = 0;
    Pfloat		x_ang = 0, y_ang = 0;
    Pmatrix3		xxform, yxform, transform;
    Pint		error;

    /* Tell the workstation to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Set the edit mode to REPLACE. */
    pset_edit_mode( PEDIT_REPLACE );

    /* Read and respond to X events. */
    XSelectInput( display, window, ButtonPressMask
	| Button1MotionMask | Button2MotionMask | ExposureMask );
    while ( !done ) {
	XNextEvent( display, &event );
	if ( event.type == ButtonPress ) {
	    switch ( event.xbutton.button ) {
		case Button1:
		case Button2:
		    /* Initialize. */
		    lastx = event.xbutton.x;
		    lasty = event.xbutton.y;
		    break;
		default:
		    done = 1;
		    break;
	    }

	} else if ( event.type == MotionNotify ) {
	    popen_struct( SPHERE );
	    pset_elem_ptr( (Pint) 0 );
	    if ( event.xmotion.state & Button1Mask ) {
		/* Increment the running rotation angles. */
		x_ang += ANGLE_SCALE * (event.xmotion.y - lasty);
		y_ang += ANGLE_SCALE * (event.xmotion.x - lastx);

		/* Build the new rotation transform. */
		protate_x( x_ang, &error, xxform );
		protate_y( y_ang, &error, yxform );
		pcompose_matrix3( xxform, yxform, &error, transform );

		/* Replace the rotation transform. */
		pset_elem_ptr_label( ROTATION_XFORM );
		poffset_elem_ptr( (Pint) 1 );
		pset_local_tran3( transform, PTYPE_REPLACE );

	    } else if ( event.xmotion.state & Button2Mask ) {
		/* Replace the model-clipping volume. */
		pset_elem_ptr_label( LOCAL_MCLIP_VOLUME );
		poffset_elem_ptr( (Pint) 1 );
		half_space.point.x += CLIP_SCALE * (event.xmotion.x - lastx);
		pset_model_clip_vol3( PMC_INTERSECT, &half_space_list );
	    }
	    pclose_struct();

	    /* Update the display. */
	    pupd_ws( WS_ID, PFLAG_PERFORM );

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

static void
set_view()
{
    Pview_rep3		view_rep;
    Pview_map3		view_map;
    Pint		err;

    /* The view orientation parameters. */
    static Ppoint3	view_ref_pt = {0.5,0.5,0.5};
    static Pvec3	view_up_vec = {0,1,0};
    static Pvec3	view_plane_normal = {0,0.2,1};

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &err, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PERSPECT;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 5.0;
    view_map.win.x_min = -0.6; view_map.win.x_max = 0.6;
    view_map.win.y_min = -0.6; view_map.win.y_max = 0.6;
    view_map.back_plane = -0.6; view_map.front_plane = 0.6;
    view_map.view_plane = 0;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;

    /* Compute the view mapping matrix. */
    peval_view_map_matrix3( &view_map, &err, view_rep.map_matrix );

    /* Set the view representation. */
    view_rep.xy_clip = PIND_NO_CLIP;
    view_rep.front_clip = PIND_NO_CLIP;
    view_rep.back_clip = PIND_NO_CLIP;
    view_rep.clip_limit = view_map.proj_vp;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
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
