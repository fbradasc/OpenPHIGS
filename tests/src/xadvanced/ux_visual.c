/* ux_visual.c 1.2 */
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

Window
create_window( display )
    Display	*display;
{
    XVisualInfo			vis_info;
    Colormap			color_map;
    Visual			*visual = (Visual *) CopyFromParent;
    int				screen, depth = (int) CopyFromParent;
    unsigned long		mask;
    XSetWindowAttributes	window_attrs;

    screen = DefaultScreen(display);
    mask = CWBackingStore | CWBorderPixel | CWBackPixel;
    window_attrs.backing_store = NotUseful;
    window_attrs.border_pixel = WhitePixel(display, screen);
    window_attrs.background_pixel = BlackPixel(display, screen);

    /* See if a 24-bit visual is available. */
    vis_info.depth = 24;
    vis_info.screen = screen;
    if ( XMatchVisualInfo( display, DefaultScreen(display),
	    vis_info.depth, TrueColor, &vis_info ) ) {
	/* 24-bit visual is available.  See if it's the not default. */
	if ( vis_info.visual != DefaultVisual(display, screen) ) {
	    mask |= CWColormap;
	    visual = vis_info.visual;
	    depth = vis_info.depth;
	    window_attrs.colormap = XCreateColormap( display,
		DefaultRootWindow(display), visual, AllocNone );
	}
    }

    return XCreateWindow( display, DefaultRootWindow(display),
	WS_X, WS_Y, WS_WIDTH, WS_HEIGHT, WS_BORDER_WIDTH,
	depth, InputOutput, visual, mask, &window_attrs );
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

    static	Ppoint		points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static	Ppoint_list	triangle = {3, points};

    /* Connect to the server and create a window. */
    if ( !(display = XOpenDisplay( NULL )) ) {
	fprintf( stderr, "Can't open default display\n" );
	exit( -1 );
    }

    /* Create and map the window. */
    window = create_window( display );
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

    /* Open a PHIGS workstation that uses the window we've created. */
    conn_id.display = display;
    conn_id.drawable_id = window;
    popen_ws( WS_ID, (void *)&conn_id, phigs_ws_type_x_drawable );

    /* Post and display the structure. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Read and respond to X events. */
    XSelectInput( display, window, ExposureMask | ButtonPressMask );
    done = 0;
    while ( !done ) {
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
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
