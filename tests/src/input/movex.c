/* movex.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define WS_GEOMETRY	"500x500+50+50"
#define BOX		(Pint) 2
#define MAX_MOVES	10

static Ppoint box_points[4] = {{.1,.1},{.1,-.1},{-.1,-.1},{-.1,.1}};
static Ppoint_list	box = {4, box_points};
static Ppoint_list_list	set = {1, &box};

main()
{
    Pmatrix		xform;
    Pvec		box_position;
    Ppoint3		button_point;
    Pint		err;
    int			done = 0;
    Display		*display;
    Window		window;
    XEvent		event;
    Pstore		store;
    Pescape_in_data	esc_in;
    Pescape_out_data	*esc_out;

    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Build a modelling transform to locate the box in the
     * center of window.
     */
    box_position.delta_x = 0.5; box_position.delta_y = 0.5;
    ptranslate( &box_position, &err, xform );

    /* Create the box. */
    popen_struct( BOX );
	pset_local_tran( xform, PTYPE_REPLACE );
	pset_int_style( PSTYLE_SOLID );
	pfill_area_set( &set );
    pclose_struct();

    /* Post and display the structure. */
    ppost_struct( WS_ID, BOX, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Set the display update state to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Set the edit mode to replace. */
    pset_edit_mode( PEDIT_REPLACE );

    /* Create a store for the escape function. */
    pcreate_store( &err, &store );

    /* Open the structure.  Get input.  Move the box. */
    popen_struct( BOX );
    XSelectInput( display, window,
	ButtonPressMask | KeyPressMask | ExposureMask );
    while ( !done ) {
	printf( "Select a new box position\n" );
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case ButtonPress:
		/* Give the escape input data and call the escape. */
		esc_in.escape_in_u5.ws_id = WS_ID;
		esc_in.escape_in_u5.points.num_points = 1;
		esc_in.escape_in_u5.points.points = &button_point;
		button_point.x = event.xbutton.x;
		button_point.y = event.xbutton.y;
		button_point.z = 0.0;
		pescape( PUESC_DRAWABLE_POINTS_TO_WC, &esc_in, store,
		    &esc_out );

		/* Check to see that the conversion was successful. */
		if ( esc_out->escape_out_u5.points.num_points != 1 )
		    break;

		/* Move the box to the selected position. */
		box_position.delta_x =
		    esc_out->escape_out_u5.points.points[0].x;
		box_position.delta_y =
		    esc_out->escape_out_u5.points.points[0].y;
		ptranslate( &box_position, &err, xform );
		pset_elem_ptr( (Pint) 1 );
		pset_local_tran( xform, PTYPE_REPLACE );

		/* Update the display. */
		pupd_ws( WS_ID, PFLAG_PERFORM );
		break;

	    case KeyPress:
		done = 1;
		break;

	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		break;
	}
    }
    pclose_struct();
    pdel_store( store );

    pclose_ws( WS_ID );
    XCloseDisplay( display );
    pclose_phigs();
    return 0;
}
