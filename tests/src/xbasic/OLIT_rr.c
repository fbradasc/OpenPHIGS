/* OLIT_rr.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <Xol/OpenLook.h>
#include <Xol/OblongButt.h>
#include <Xol/MenuButton.h>
#include <Xol/ControlAre.h>
#include <Xol/Form.h>
#include <Xol/Stub.h>
#include <phigs/phigs.h>

/* PHIGS Identifiers */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define INTERIOR_STYLE_LABEL	(Pint) 2

/* Window layout information */
#define WS_WIDTH		250
#define WS_HEIGHT		250
#define WS_BORDER_WIDTH		1

static Widget	top;
static Atom	delete_window_atom;

/* Callback function to quit the program. */
static void
client_event( w, client_data, event )
    Widget	w;
    caddr_t	client_data;
    XEvent	*event;
{
    XClientMessageEvent	*cm = &event->xclient;

    if ( cm->data.l[0] == delete_window_atom ) {
	pclose_ws( WS_ID );
	pclose_phigs();
	XtDestroyWidget( top );
	exit( 0 );
    }
}

#define MAX_RECTS	10

/* Event handler for Expose events. */
static void
redraw( w, client_data, event )
    Widget	w;
    caddr_t	client_data;
    XEvent	*event;
{
    Pescape_in_data	esc_in;

    static int		num_rects = 0;
    static XRectangle	rects[MAX_RECTS];

    /* Add this exposure to the list. */
    rects[num_rects].x = event->xexpose.x;
    rects[num_rects].y = event->xexpose.y;
    rects[num_rects].width = event->xexpose.width;
    rects[num_rects].height = event->xexpose.height;
    num_rects++;

    /* See if it's the last event or if we've reached our limit. */
    if ( event->xexpose.count == 0 || num_rects == MAX_RECTS ) {
	/* Redraw PHIGS output in those regions. */
	esc_in.escape_in_u6.ws_id = WS_ID;
	esc_in.escape_in_u6.num_regions = num_rects;
	esc_in.escape_in_u6.regions = rects;
	pescape( PUESC_REDRAW_REGIONS, &esc_in, (Pstore)NULL,
	    (Pescape_out_data **)NULL );

	/* Reset. */
	num_rects = 0;
    }
}

/* Callback function to set the interior style. */
static void
set_interior_style( w, client_data, call_data )
    Widget	w;
    caddr_t	client_data;
    caddr_t	call_data;
{
    XSync( XtDisplay(w), False );
    pset_edit_mode( PEDIT_REPLACE );
    popen_struct( STRUCTURE_ID );
	pset_elem_ptr( (Pint) 0 );
	pset_elem_ptr_label( INTERIOR_STYLE_LABEL );
	poffset_elem_ptr( (Pint) 1 );
	pset_int_style( (Pint_style)client_data );
    pclose_struct();
    pupd_ws( WS_ID, PFLAG_PERFORM );
}

/* Macro to simplify creating buttons. */
#define ADD_BUTTON( _label, _w, _f, _val ) \
    (XtAddCallback( XtVaCreateManagedWidget( (_label), \
	oblongButtonWidgetClass, (_w), NULL), \
	XtNselect, (_f), (_val) ))

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Widget			form, canvas, bar, menu;
    Window			xid;
    Pconnid_x_drawable		conn_id;
    XSetWindowAttributes	win_attrs;
    Pxphigs_info		xphigs_info;
    unsigned long		mask;

    static	Ppoint		points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static	Ppoint_list	triangle = {3, points};

    /* Initialize the toolkit and create the container for the PHIGS
     * window and control panel.
     */
    top = OlInitialize( argv[0], NULL, NULL, 0, &argc, argv );
    form = XtVaCreateManagedWidget( argv[0], formWidgetClass, top,
	NULL );

    /* Create the menu bar. */
    bar = XtVaCreateManagedWidget( "menubar",
	controlAreaWidgetClass, form,
	XtNlayoutType, OL_FIXEDROWS, XtNmeasure, 1,
	XtNsameSize, OL_ALL,
	XtNxAttachRight, TRUE, XtNxVaryOffset, FALSE,
	XtNxResizable, TRUE, XtNyResizable, TRUE,
	XtNborderWidth, 1,
	NULL );

    /* Create the pulldown menu. */
    menu = XtVaCreateManagedWidget( "Interior",
	menuButtonWidgetClass, bar,
	XtNlabel, "Interior", XtNtitle, "Interior",
	XtNmenuMark, OL_DOWN, 
	NULL );
    XtVaGetValues( menu, XtNmenuPane, (XtArgVal)&menu, NULL );
    ADD_BUTTON( "hollow",  menu, set_interior_style, PSTYLE_HOLLOW );
    ADD_BUTTON( "solid",   menu, set_interior_style, PSTYLE_SOLID );
    ADD_BUTTON( "pattern", menu, set_interior_style, PSTYLE_PAT );
    ADD_BUTTON( "hatch",   menu, set_interior_style, PSTYLE_HATCH );
    ADD_BUTTON( "empty",   menu, set_interior_style, PSTYLE_EMPTY );

    /* Create the widget we'll use as a drawing window for PHIGS. */
    canvas = XtVaCreateManagedWidget( "canvas", stubWidgetClass, form, 
	XtNwidth, WS_WIDTH, XtNheight, WS_HEIGHT,
	XtNborderWidth, WS_BORDER_WIDTH,
	XtNyRefWidget, bar, XtNyAddHeight, TRUE,
	XtNxAttachRight, TRUE, XtNxVaryOffset, FALSE,
	XtNyAttachBottom, TRUE, XtNyVaryOffset, FALSE,
	XtNxResizable, TRUE, XtNyResizable, TRUE,
	NULL );

    /* Register an event handler for exposure events. */
    XtAddEventHandler( canvas, ExposureMask, FALSE, redraw, 0 );

    /* Realize the widgets. */
    XtRealizeWidget( top );

    /* Set up to use window manager DELETE_WINDOW protocol. */
    delete_window_atom = XInternAtom( XtDisplay(top),
	"WM_DELETE_WINDOW", False );
    XSetWMProtocols( XtDisplay(top), XtWindow(top),
	&delete_window_atom, 1 );
    XtAddEventHandler( top, NoEventMask, TRUE, client_event, NULL );

    /* Tell the window manager to keep track of the colormap. */
    xid = XtWindow( canvas );
    XSetWMColormapWindows( XtDisplay(top), XtWindow(top), &xid, 1 );

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    /* Build the structure that we'll display. */
    popen_struct( STRUCTURE_ID );
    plabel( INTERIOR_STYLE_LABEL );
    pset_int_style( PSTYLE_SOLID );
    pfill_area( &triangle );
    pclose_struct();

    /* Set the backing store attribute. */
    win_attrs.backing_store = NotUseful;
    XChangeWindowAttributes( XtDisplay(canvas), XtWindow(canvas),
	CWBackingStore, &win_attrs );

    /* Open the workstation. */
    conn_id.display = XtDisplay(canvas);
    conn_id.drawable_id = XtWindow(canvas);
    popen_ws( WS_ID, (void *)&conn_id, phigs_ws_type_x_drawable );

    /* Set the update mode to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Post the structure created earlier. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );

    XtMainLoop();
}
