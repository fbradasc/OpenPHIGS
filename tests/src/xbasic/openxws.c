/* openxws.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

void
open_xphigs_and_xlib_ws( ws_id, geometry, display, window )
    Pint	ws_id;	
    char	*geometry;
    Display	**display;
    Window	*window;
{
    XEvent			event;
    XSetWindowAttributes	win_attrs;
    int				x = 50, y = 50;
    unsigned int		width = 400, height = 400;
    Pconnid_x_drawable		conn_id;
    Pxphigs_info		xphigs_info;
    unsigned long		mask;

    /* Connect to the server. */
    if ( !(*display = XOpenDisplay( NULL )) ) {
	(void)fprintf( stderr, "Can't open default display\n" );
	exit( -1 );
    }

    /* Create the window. */
    if ( geometry )
	(void)XParseGeometry( geometry, &x, &y, &width, &height );
    win_attrs.backing_store = NotUseful;
    win_attrs.border_pixel = win_attrs.background_pixel =
	BlackPixel(*display, DefaultScreen(*display));
    *window = XCreateWindow( *display,
	RootWindow( *display, DefaultScreen( *display ) ),
	x, y, width, height, 1 /* border width */,
	CopyFromParent, InputOutput, CopyFromParent,
	CWBackingStore | CWBorderPixel | CWBackPixel, &win_attrs );
    XMapWindow( *display, *window );

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    /* Open a PHIGS workstation that uses the window we created. */
    conn_id.display = *display;
    conn_id.drawable_id = *window;
    popen_ws( ws_id, (void *)&conn_id, phigs_ws_type_x_drawable );
}
