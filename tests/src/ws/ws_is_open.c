/* ws_is_open.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* PHIGS Identifiers */
#define STRUCTURE_ID	(Pint) 1

/* Window size information */
#define WS_WIDTH	300
#define WS_HEIGHT	300

static int
ws_is_open( ws_id )
    Pint	ws_id;
{
    Pint	open_ws_id[1], start, error, num_wss;
    Pint_list	ws_list;

    ws_list.ints = open_ws_id;
    start = 0;
    do {
	pinq_open_wss( (Pint) 1, start, &error, &ws_list, &num_wss );
	if ( error ) /* PHIGS is not open */
	    break;
	if ( ws_list.ints[0] == ws_id )
	    return 1; /* found */
    } while ( ++start < num_wss );

    return 0; /* not found */
}

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Window			window[4];
    XEvent			event;
    int				i, done;
    XSetWindowAttributes	win_attrs;
    Pconnid_x_drawable		conn_id, *ret_conn_id;
    Pxphigs_info		xphigs_info;
    unsigned long		mask;
    Pint			ws_type, error;
    Pstore			store;

    static	Ppoint		points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static	Ppoint_list	triangle = {3, points};
    static	int		x[4] = {100, 200, 200, 100};
    static	int		y[4] = {100, 100, 200, 200};

    /* Connect to the server and create a window. */
    if ( !(conn_id.display = XOpenDisplay( NULL )) ) {
	fprintf( stderr, "Can't open default display\n" );
	exit( -1 );
    }

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    /* Set the window attributes. */
    win_attrs.backing_store = NotUseful;
    win_attrs.border_pixel = win_attrs.background_pixel =
	BlackPixel(conn_id.display, DefaultScreen(conn_id.display));
    win_attrs.event_mask = ExposureMask | ButtonPressMask;

    /* Create the windows. */
    for ( i = 0; i < 4; i++ ) {
	window[i] = XCreateWindow( conn_id.display,
	    RootWindow( conn_id.display, DefaultScreen( conn_id.display ) ),
	    x[i], y[i], WS_WIDTH, WS_HEIGHT, 0,
	    CopyFromParent, InputOutput, CopyFromParent,
	    CWBackingStore | CWBorderPixel | CWBackPixel | CWEventMask,
	    &win_attrs );
	XMapWindow( conn_id.display, window[i] );
    }

    /* Open the workstations. */
    for ( i = 0; i < 4; i++ ) {
	conn_id.drawable_id = window[i];
	popen_ws( (Pint) i, (void *)&conn_id, phigs_ws_type_x_drawable );
    }
    for ( i = 0; i < 5; i++ ) {
	if ( !ws_is_open( i ) )
	    fprintf( stderr, "workstation %d not open\n", i );
	else
	    fprintf( stderr, "workstation %d is open\n", i );
    }

    /* Get the specific workstation type. */
    pcreate_store( &error, &store );
    for ( i = 0; i < 5; i++ ) {
	Pdisp_space_size3	dc_info;

	pinq_ws_conn_type( (Pint)i, store, &error, &ret_conn_id, &ws_type);
	pinq_disp_space_size3( ws_type, &error, &dc_info );
    }

    /* Build the structure that we'll display. */
    popen_struct( STRUCTURE_ID );
	pset_int_style( PSTYLE_SOLID );
	pfill_area( &triangle );
    pclose_struct();

    /* Open a PHIGS workstation that uses the window we've created. */

    /* Post and display the structure. */
    for ( i = 0; i < 4; i++ ) {
	ppost_struct( i, STRUCTURE_ID, (Pfloat) 1.0 );
	pupd_ws( i, PFLAG_PERFORM );
    }

    /* Read and respond to X events. */
    done = 0;
    while ( !done ) {
	XNextEvent( conn_id.display, &event );
	switch ( event.type ) {
	    case Expose:
		if ( event.xexpose.count == 0 )
		    for ( i = 0; i < 4; i++ )
			if ( event.xexpose.window == window[i] )
			    predraw_all_structs( i, PFLAG_ALWAYS );
		break;
	    case ButtonPress:
		done = 1;
		break;
	}
    }

    for ( i = 0; i < 4; i++ )
	pclose_ws( i );
    pclose_phigs();
    XCloseDisplay( conn_id.display );
    return 0;
}
