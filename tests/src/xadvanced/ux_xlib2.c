/* ux_xlib2.c 1.3 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* PHIGS Identifiers */
#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

/* Window layout information */
#define WS_X		50
#define WS_Y		50
#define WS_WIDTH	300
#define WS_HEIGHT	300
#define WS_BORDER_WIDTH	1

static void
resize( display, ws_id, event )
    Display	*display;
    Pint	ws_id;
    XEvent	*event;
{
    Pint	error;
    Pupd_st	upd_st;
    Plimit	req_win, req_vp, cur_win, cur_vp, new_vp;

    /* Determine the new workstation viewport from the window size. */
    new_vp.x_min = 0; new_vp.y_min = 0;
    new_vp.x_max = event->xconfigure.width;
    new_vp.y_max = event->xconfigure.height;

    /* Inquire the current WS transform, then set the viewport. */
    pinq_ws_tran( ws_id, &error, &upd_st, &req_win, &cur_win,
	&req_vp, &cur_vp );
    if ( new_vp.x_max != cur_vp.x_max || new_vp.y_max != cur_vp.y_max ) {
#ifdef SunPHIGS2.0
	{
	    Pescape_in_data	in;
	    in.escape_in_u8.ws_id = ws_id;
	    pescape( PUESC_RASTER_RESIZE, &in, (Pstore)NULL,
		(Pescape_out_data **)NULL );
	}
#endif
	pset_ws_vp( ws_id, &new_vp );
    }
}

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Display			*display;
    Window			window;
    XEvent			event;
    int				done;
    XSetWindowAttributes	win_attrs;
    Pconnid_x_drawable		conn_id;
    Pxphigs_info		xphigs_info;
    unsigned long		mask;
    Pint			ws_type;

    static Ppoint	points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static Ppoint_list	triangle = {3, points};

    /* Connect to the server and create a window. */
    if ( !(display = XOpenDisplay( NULL )) ) {
	fprintf( stderr, "Can't open default display\n" );
	exit( -1 );
    }
    win_attrs.backing_store = NotUseful;
    win_attrs.border_pixel = win_attrs.background_pixel =
	BlackPixel(display, DefaultScreen(display));
    window = XCreateWindow( display,
	RootWindow( display, DefaultScreen( display ) ),
	WS_X, WS_Y, WS_WIDTH, WS_HEIGHT, WS_BORDER_WIDTH,
	CopyFromParent, InputOutput, CopyFromParent,
	CWBackingStore | CWBorderPixel | CWBackPixel, &win_attrs );
    XMapWindow( display, window );

    /* Wait for the window to become visible. */
    XSelectInput( display, window, ExposureMask );
    XWindowEvent( display, window, ExposureMask, &event );

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    /* Build the structure that we'll display. */
    popen_struct( STRUCTURE_ID );
	pset_int_style( PSTYLE_SOLID );
	pfill_area( &triangle );
    pclose_struct();

    /* Create a new workstation type whose DC limits adjust with window
     * size changes.
     */
    ws_type = phigs_ws_type_create( phigs_ws_type_x_drawable,
	PHIGS_DC_MODEL, PHIGS_DC_LIMITS_ADJUST_TO_WINDOW,
	NULL );

    /* Open a PHIGS workstation that uses the window we've created. */
    conn_id.display = display;
    conn_id.drawable_id = window;
    popen_ws( WS_ID, (void *)&conn_id, ws_type );

    /* Post and display the structure. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Read and respond to X events. */
    XSelectInput( display, window,
	ExposureMask | ButtonPressMask | StructureNotifyMask );
    done = 0;
    while ( !done ) {
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		break;
	    case ConfigureNotify:
		resize( display, WS_ID, &event );
		break;
	    case ButtonPress:
		done = 1;
		break;
	}
    }

    pclose_ws( WS_ID );
    pclose_phigs();
    XCloseDisplay( display );
    return 0;
}
