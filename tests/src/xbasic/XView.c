/* XView.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <X11/Xatom.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/canvas.h>
#include <xview/xv_xrect.h>
#include <phigs/phigs.h>

/* PHIGS Identifiers */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1
#define INTERIOR_STYLE_LABEL	(Pint) 1

/* Window layout information */
#define WS_WIDTH		250
#define WS_HEIGHT		250

/* Callback function to set the interior style. */
static int
set_interior_style( menu, item )
    Menu	menu;
    Menu_item	item;
{
    Pint_style	style;

    pset_edit_mode( PEDIT_REPLACE );
    popen_struct( STRUCTURE_ID );
    pset_elem_ptr( (Pint) 0 );
    pset_elem_ptr_label( INTERIOR_STYLE_LABEL );
    poffset_elem_ptr( (Pint) 1 );
    switch ( xv_get( item, MENU_VALUE ) ) {
	case 1: style = PSTYLE_HOLLOW; break;
	case 2: style = PSTYLE_SOLID; break;
	case 3: style = PSTYLE_PAT; break;
	case 4: style = PSTYLE_HATCH; break;
	case 5: style = PSTYLE_EMPTY; break;
    }
    pset_int_style( style );
    pclose_struct();
    pupd_ws( WS_ID, PFLAG_PERFORM );

    return XV_OK;
}

/* Callback function to redisplay the PHIGS output. */
static void
redraw( canvas, paint_window, display, xwindow, area )
    Canvas		canvas;
    Xv_Window		paint_window;
    Display		*display;
    Window		xwindow;
    Xv_xrectlist	*area;
{
    /* Redraw the PHIGS workstation. */
    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
}

/* Callback function to quit the program. */
static Notify_value
quit_proc( frame, status )
    Frame		frame;
    Destroy_status	status;
{
    if ( status == DESTROY_CLEANUP ) {
	/* Close the workstation and PHIGS. */
	pclose_ws( WS_ID );
	pclose_phigs();
    }
	
    return notify_next_destroy_func( frame, status );
}

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Frame		frame;
    Panel		panel;
    Canvas		canvas;
    Menu		menu;
    Window		paint_xwindow;
    Display		*display;
    Pconnid_x_drawable	conn_id;
    Pxphigs_info	xphigs_info;
    unsigned long	mask;

    static	Ppoint		points[] = {{.2,.2},{.8,.2},{.5,.8}};
    static	Ppoint_list	triangle = {3, points};

    /* Initialize XView and create the container for the PHIGS
     * window and control panel.
     */
    xv_init( XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0 );
    frame = xv_create( NULL, FRAME, FRAME_LABEL, argv[0], NULL );

    /* Create the menu bar. */
    panel = xv_create( frame, PANEL,
	PANEL_LAYOUT, PANEL_HORIZONTAL,
	0 );
    menu = (Menu)xv_create( NULL, MENU,
	MENU_NOTIFY_PROC, set_interior_style,
	MENU_STRINGS,
	    "hollow", "solid", "pattern", "hatch", "empty", 0,
	0 );
    (void)xv_create( panel, PANEL_BUTTON,
	PANEL_LABEL_STRING, "Interior",
	PANEL_ITEM_MENU, menu,
	0 );
    window_fit_height( panel );

    /* Create the canvas we'll use as a drawing window for PHIGS. */
    canvas = xv_create( frame, CANVAS,
	CANVAS_RETAINED, FALSE,
	CANVAS_REPAINT_PROC, redraw, CANVAS_X_PAINT_WINDOW, TRUE,
	CANVAS_WIDTH, WS_WIDTH, CANVAS_HEIGHT, WS_HEIGHT,
	CANVAS_AUTO_CLEAR, FALSE,
	CANVAS_AUTO_SHRINK, FALSE, CANVAS_AUTO_EXPAND, FALSE,
	XV_WIDTH, WS_WIDTH+1, XV_HEIGHT, WS_HEIGHT+1,
	WIN_BELOW, panel, XV_X, 0,
	0 );
    xv_set( panel, WIN_WIDTH, xv_get( canvas, WIN_WIDTH ), NULL );
    window_fit( frame );

    /* Set up to close the workstation when the user selects "Quit"
     * from the frame menu.
     */
    notify_interpose_destroy_func( frame, quit_proc );

    /* Tell the window manager to keep track of the canvas' colormap. */
    display = (Display *)xv_get( canvas, XV_DISPLAY );
    paint_xwindow = (Window)
	xv_get( (Xv_Window ) canvas_paint_window( canvas ), XV_XID );
    XSetWMColormapWindows( display, (Window) xv_get( frame, XV_XID ),
	&paint_xwindow, 1 );

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

    /* Open the workstation. */
    conn_id.display = display;
    conn_id.drawable_id = paint_xwindow;
    popen_ws( WS_ID, (void *)&conn_id, phigs_ws_type_x_drawable );

    /* Set the update mode to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Post the structure created earlier. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );

    /* Turn control over to XView. */
    xv_main_loop( frame );
    return 0;
}
