/* ux_xlib_rr.c 1.2 */
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

#define MAX_REGIONS	10

static void
expose( display, workstation_id, first_event )
    Display	*display;
    Pint	workstation_id;
    XEvent	*first_event;	/* first exposure event */
{
    XRectangle		rects[MAX_REGIONS];
    XEvent		event, *evp = first_event;
    Pescape_in_data	esc_in;
    int			i = 0;

    /* Collect all the exposed regions. */
    do {
	rects[i].x = evp->xexpose.x;
	rects[i].y = evp->xexpose.y;
	rects[i].width = evp->xexpose.width;
	rects[i].height = evp->xexpose.height;
	i++;
    } while ( i < MAX_REGIONS && XCheckTypedWindowEvent( display,
	evp->xexpose.window, Expose, evp = &event) );

    /* Redraw PHIGS output in those regions. */
    esc_in.escape_in_u6.ws_id = workstation_id;
    esc_in.escape_in_u6.num_regions = i;
    esc_in.escape_in_u6.regions = rects;
    pescape( PUESC_REDRAW_REGIONS, &esc_in, (Pstore)NULL,
	(Pescape_out_data **)NULL );
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

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    /* Build the structure that we'll display. */
    popen_struct( STRUCTURE_ID );
	pset_int_style( PSTYLE_SOLID );
	pfill_area( &triangle );
    pclose_struct();

    /* Open a PHIGS workstation that uses the window we've created. */
    conn_id.display = display;
    conn_id.drawable_id = window;
    popen_ws( WS_ID, (void *)&conn_id, phigs_ws_type_x_drawable );

    /* Post the structure. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );

    /* Read and respond to X events. */
    XSelectInput( display, window, ExposureMask | ButtonPressMask );
    done = 0;
    while ( !done ) {
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case Expose:
		expose( display, WS_ID, event );
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
