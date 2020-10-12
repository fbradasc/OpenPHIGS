/* extent.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* PHIGS Identifiers */
#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

/* Window Configuration Information */
#define WS_GEOMETRY	"300x300+50+50"

/* Text positioning and other info */
#define X_LABEL			"X position: "
#define Y_LABEL			"Y position: "
#define UPPER_LABEL_X		0.05
#define UPPER_LABEL_Y		0.95
#define READOUT_FORMAT		"%4d"
#define READOUT_MARKER		(Pint) 1

/* Text attribute values */
#define FONT			(Pint) 1
#define CHARACTER_HEIGHT	(Pfloat) 0.05
#define EXPANSION		(Pfloat) 1.0
#define SPACING			(Pfloat) 0.0

/* Utility defined in Chapter 3. */
extern void	open_xphigs_and_xlib_ws();

/* Utility function defined below. */
static void report_error_and_exit( Pint	error, char *function);

main( )
{
    /* X-related variables */
    Display			*display;
    Window			window;
    XEvent			event;
    int				done;

    /* PHIGS-related variables */
    void		*inquired_conn;
    char		buffer[20];
    float		height;
    Ppoint		upper_offset, lower_offset, upper_label,
			upper_readout, lower_label, lower_readout;
    Prect		upper_extent, lower_extent;
    Pint		error = 0, ws_type;
    Pstore		store;
    Ptext_align		alignment;

    /* Open PHIGS and create a workstation.  Use the utility function
     * open_xphigs_and_xlib_ws() that we wrote in Chapter 3.
     */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Get the specific workstation type to pass to the extent inquiry. */
    pcreate_store( &error, &store );
    if ( error != 0 )
	report_error_and_exit( error, "pcreate_store" );
    pinq_ws_conn_type( WS_ID, store, &error, &inquired_conn, &ws_type );
    if ( error != 0 )
	report_error_and_exit( error, "pinq_ws_conn_type" );

    /* Inquire the extents of the label strings. */
    pinq_text_extent( ws_type, FONT, EXPANSION, SPACING,
	CHARACTER_HEIGHT, PPATH_RIGHT, PHOR_LEFT, PVERT_TOP,
	X_LABEL, &error, &upper_extent, &upper_offset );
    if ( error != 0 )
	report_error_and_exit( error, "pinq_text_extent" );

    pinq_text_extent( ws_type, FONT, EXPANSION, SPACING,
	CHARACTER_HEIGHT, PPATH_RIGHT, PHOR_LEFT, PVERT_TOP,
	Y_LABEL, &error, &lower_extent, &lower_offset );
    if ( error != 0 )
	report_error_and_exit( error, "pinq_text_extent" );
    
    /* Compute the font height. It's the same for all characters. */
    height = upper_extent.q.y - upper_extent.p.y;
    
    /* Compute the lower label position and the readout positions. */
    upper_label.x = UPPER_LABEL_X;
    upper_label.y = UPPER_LABEL_Y;
    lower_label.x = upper_label.x;
    lower_label.y = upper_label.y - height;

    upper_readout.x = upper_label.x + upper_offset.x;
    upper_readout.y = upper_label.y + upper_offset.y;

    lower_readout.x = lower_label.x + lower_offset.x;
    lower_readout.y = lower_label.y + lower_offset.y;
    
    /* Build the structure. */
    popen_struct( STRUCTURE_ID );
	/* Create the attribute elements. */
	pset_text_prec( PPREC_STROKE );
	pset_text_font( FONT );
	pset_char_ht( CHARACTER_HEIGHT );
	pset_char_space( SPACING );
	pset_char_expan( EXPANSION );
	alignment.hor = PHOR_LEFT;
	alignment.vert = PVERT_TOP;
	pset_text_align( &alignment );

	/* Create the labels. */
	ptext( &upper_label, X_LABEL );
	ptext( &lower_label, Y_LABEL );

	/* Create the readouts. Use empty strings for now. */
	plabel( READOUT_MARKER );	/* mark them to find later */
	ptext( &upper_readout, "" );
	ptext( &lower_readout, "" );
    pclose_struct();

    /* Post and display the structure. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Tell the workstation to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Open the structure and set the edit mode to REPLACE. */
    popen_struct( STRUCTURE_ID );
    pset_edit_mode( PEDIT_REPLACE );

    /* Read and respond to X events. */
    XSelectInput( display, window,
	PointerMotionMask | ButtonPressMask | ExposureMask );
    done = 0;
    while ( !done ) {
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case MotionNotify:
		/* Replace the text element for the upper readout. */
		pset_elem_ptr( 0 );
		pset_elem_ptr_label( READOUT_MARKER );
		poffset_elem_ptr( 1 );
		sprintf( buffer, READOUT_FORMAT, event.xmotion.x );
		ptext( &upper_readout, buffer );

		/* Replace the text element for the lower readout. */
		poffset_elem_ptr( 1 );
		sprintf( buffer, READOUT_FORMAT, event.xmotion.y );
		ptext( &lower_readout, buffer );

		/* Update the display. */
		pupd_ws( WS_ID, PFLAG_PERFORM );
		break;

	    case ButtonPress:
		done = 1;
		break;

	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		break;
	}
    }

    pclose_struct();
    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
report_error_and_exit( error, function )
    Pint	error;
    char	*function;
{
    fprintf( stderr, "error %d from %s\n", error, function );
    exit( 1 );
}
