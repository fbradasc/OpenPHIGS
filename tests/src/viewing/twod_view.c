/* twod_view.c 1.2 */
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
#define ZOOM_FACTOR	0.05
#define PAN_SCALE	0.0025

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Display	*display;
    Window	window;
    XEvent	event;
    Pint	error, font;
    Pvec	vup;
    Ppoint	text_pt, vrp;
    Pview_map	vmap;
    Pview_rep	view;
    int		lastx, lasty, done = 0;
    float	vw_size, pan_factor = PAN_SCALE * 400;
    char	letter[2];

    /* Open PHIGS and create a workstation.  Use the utility function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    font = argc > 1 ? atoi(argv[1]) : 1;

    popen_struct( STRUCT_ID );
	pset_view_ind( (Pint) 1 );
	pset_text_font( font );
	pset_char_ht( (Pfloat) 10 );
	text_pt.x = 150; text_pt.y = 300;
	letter[1] = '\0';
	for ( letter[0] = '!'; letter[0] < '~'; letter[0] ++ ) {
	    ptext( &text_pt, letter );
	    text_pt.x += 15;
	    if ( text_pt.x >= 300 )
	    text_pt.x = 100, text_pt.y -= 20;
	}
    pclose_struct();

    /* Compute the view orientation matrix. */
    vrp.x = vrp.y = 0;
    vup.delta_x = 0; vup.delta_y = 1;
    peval_view_ori_matrix( &vrp, &vup, &error, view.ori_matrix );

    /* Compute the view mapping matrix. */
    vmap.win.x_min = 0; vmap.win.x_max = 400;
    vmap.win.y_min = 0; vmap.win.y_max = 400;
    vmap.proj_vp.x_min = 0; vmap.proj_vp.x_max = 1;
    vmap.proj_vp.y_min = 0; vmap.proj_vp.y_max = 1;
    peval_view_map_matrix( &vmap, &error, view.map_matrix );

    /* Store the view in the view table. */
    view.xy_clip = PIND_CLIP;
    view.clip_limit = vmap.proj_vp;
    pset_view_rep( WS_ID, (Pint) 1, &view );

    /* Set the update state for automatic updates. */
    pset_disp_upd_st( WS_ID, PDEFER_ASAP, PMODE_UQUM );

    /* Post the top-level structure. */
    ppost_struct( WS_ID , STRUCT_ID, (Pfloat) 1.0 );

    /* Read and respond to X events. */
    XSelectInput( display, window,
	ButtonPressMask | Button1MotionMask | ExposureMask );
    while ( !done ) {
	XNextEvent( display, &event );
	if ( event.type == ButtonPress ) {
	    switch ( event.xbutton.button ) {
		case Button1:
		    if ( event.xbutton.state & ControlMask ) {
			/* Reset to center. */
			vrp.x = 0; vrp.y = 0;
			vmap.win.x_min = 0; vmap.win.x_max = 400;
			vmap.win.y_min = 0; vmap.win.y_max = 400;
			peval_view_map_matrix( &vmap, &error,
			    view.map_matrix );
			peval_view_ori_matrix( &vrp, &vup, &error,
			    view.ori_matrix );
			pset_view_rep( WS_ID, (Pint) 1, &view );
		    } else {
			/* Initialize for panning. */
			lastx = event.xbutton.x;
			lasty = event.xbutton.y;
		    }
		    break;

		case Button2:
		    /* ZOOM -- Compute the new view window. */
		    vw_size = vmap.win.x_max - vmap.win.x_min;
		    if ( vw_size <= 0 ) vw_size = 1;
			if ( event.xbutton.state & ShiftMask ) {
			    /* Zoom out. */
			    vmap.win.x_min -= ZOOM_FACTOR * vw_size;
			    vmap.win.x_max += ZOOM_FACTOR * vw_size;
			    vmap.win.y_min -= ZOOM_FACTOR * vw_size;
			    vmap.win.y_max += ZOOM_FACTOR * vw_size;
			} else {
			    /* Zoom in. */
			    vmap.win.x_min += ZOOM_FACTOR * vw_size;
			    vmap.win.x_max -= ZOOM_FACTOR * vw_size;
			    vmap.win.y_min += ZOOM_FACTOR * vw_size;
			    vmap.win.y_max -= ZOOM_FACTOR * vw_size;
			}
			peval_view_map_matrix( &vmap, &error,
			    view.map_matrix );
			if ( !error )
			    pset_view_rep( WS_ID, (Pint) 1, &view );

			/* Re-compute the panning factor. */
			pan_factor = PAN_SCALE * vw_size;
			break;

		default:
		    done = 1;
		    break;
	    }

	} else if ( event.type == MotionNotify ) {
	    /* PAN -- Compute the new view reference point. */
	    if ( event.xbutton.state & ShiftMask ) {
		vrp.x -= pan_factor * (event.xmotion.x - lastx);
		vrp.y -= pan_factor * (lasty - event.xmotion.y);
	    } else {
		vrp.x += pan_factor * (event.xmotion.x - lastx);
		vrp.y += pan_factor * (lasty - event.xmotion.y);
	    }
	    peval_view_ori_matrix( &vrp, &vup, &error,
		view.ori_matrix );
	    if ( !error )
		pset_view_rep( WS_ID, (Pint) 1, &view );
	    lastx = event.xmotion.x;
	    lasty = event.xmotion.y;
	} else switch ( event.type ) {
	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		break;
	}
    }

    pclose_ws( WS_ID );
    pclose_phigs();
    XCloseDisplay( display );
    return 0;
}
