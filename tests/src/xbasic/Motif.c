/* Motif.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/Protocols.h>
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
quit( w, client_data, call_data )
    Widget			w;
    caddr_t			client_data;
    XmAnyCallbackStruct		*call_data;
{
    XClientMessageEvent		*cm = &call_data->event->xclient;

    if ( cm->data.l[0] == delete_window_atom ) {
	pclose_ws( WS_ID );
	pclose_phigs();
	XtDestroyWidget( top );
	exit( 0 );
    }
}

/* Callback function to redisplay the PHIGS output. */
static void
redraw( w, client_data, event )
    Widget	w;
    caddr_t	client_data;
    XEvent	*event;
{
    /* Redraw the PHIGS workstation. */
    if ( event->xexpose.count == 0 ) /* last Expose event */
	predraw_all_structs( WS_ID, PFLAG_ALWAYS );
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
	xmPushButtonWidgetClass, (_w), NULL), \
	XmNactivateCallback, (_f), (_val) ))

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Widget			form, canvas, bar, menu;
    Window			xid;
    Pconnid_x_drawable		conn_id;
    XSetWindowAttributes	win_attrs;
    XtAppContext		app;
    Pxphigs_info		xphigs_info;
    unsigned long		mask;

    static	Ppoint		points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static	Ppoint_list	triangle = {3, points};

    /* Initialize the toolkit and create the container for the PHIGS
     * window and control panel.
     */
    top = XtVaAppInitialize( &app, "Motif", NULL, 0,
	&argc, argv, NULL, NULL );
    form = XtVaCreateManagedWidget( NULL, xmFormWidgetClass, top,
	NULL );

    /* Create the menu bar. */
    bar = XmCreateMenuBar( form, "menubar", NULL, 0 );
    XtVaSetValues( bar, 
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL );
    XtManageChild( bar );

    /* Create the pulldown menu. */
    menu = XmCreatePulldownMenu( bar, "interior", NULL, 0 );
    XtVaCreateManagedWidget( "Interior", xmCascadeButtonWidgetClass,
	bar, XmNsubMenuId, menu,
	NULL );
    ADD_BUTTON( "hollow",  menu, set_interior_style, PSTYLE_HOLLOW );
    ADD_BUTTON( "solid",   menu, set_interior_style, PSTYLE_SOLID );
    ADD_BUTTON( "pattern", menu, set_interior_style, PSTYLE_PAT );
    ADD_BUTTON( "hatch",   menu, set_interior_style, PSTYLE_HATCH );
    ADD_BUTTON( "empty",   menu, set_interior_style, PSTYLE_EMPTY );


    /* Create the widget we'll use as a drawing window for PHIGS. */
    canvas = XtVaCreateManagedWidget( "canvas",
	xmDrawingAreaWidgetClass, form, 
	XtNwidth, WS_WIDTH, XtNheight, WS_HEIGHT,
	XtNborderWidth, WS_BORDER_WIDTH,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, bar,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL );

    /* Register an event handler for Expose events. */
    XtAddEventHandler( canvas, ExposureMask, FALSE, redraw, 0 );

    /* Realize the widgets. */
    XtRealizeWidget( top );

    /* Set up to use window manager DELETE_WINDOW protocol. */
    delete_window_atom = XInternAtom( XtDisplay(top),
	"WM_DELETE_WINDOW", TRUE );
    XmAddWMProtocols( top, &delete_window_atom, 1 );
    XmSetWMProtocolHooks( top, delete_window_atom,
	NULL, NULL, quit, NULL );

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
	{Pgcolr		white;
	white.type = PMODEL_RGB;
	white.val.general.x = 1.0;
	white.val.general.y = 1.0;
	white.val.general.z = 1.0;
	pset_int_colr( &white );
	}
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

    XtAppMainLoop( app );
}
