/* move.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define BOX		(Pint) 2
#define TIMEOUT		(Pfloat) 30.0	/* seconds */
#define MAX_MOVES	10

static Ppoint box_points[4] = {{.1,.1},{.1,-.1},{-.1,-.1},{-.1,.1}};
static Ppoint_list	box = {4, box_points};
static Ppoint_list_list	set = {1, &box};

main()
{
    Pmatrix		xform;
    Pvec		box_position;
    Ppoint		locator_position;
    Pint		err, ws_id, device_num, view;
    Pin_class		class;
    int			num_moves;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Build a modelling transform to locate the box in the
     * center of the window.
     */
    box_position.delta_x = 0.5; box_position.delta_y = 0.5;
    ptranslate( &box_position, &err, xform );

    /* Create the box. */
    popen_struct( BOX );
	pset_local_tran( xform, PTYPE_REPLACE );
	pset_int_style( PSTYLE_SOLID );
	pfill_area_set( &set );
    pclose_struct();

    /* Open the workstation. Post and display the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID, BOX, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Enable locator device 1. */
#ifdef EVENT
    pset_loc_mode( WS_ID, (Pint) 1, POP_EVENT, PSWITCH_ECHO );
#endif
#ifdef SAMPLE
    pset_loc_mode( WS_ID, (Pint) 1, POP_SAMPLE, PSWITCH_ECHO );
#endif
#ifdef REQUEST
    pset_loc_mode( WS_ID, (Pint) 1, POP_REQ, PSWITCH_ECHO );
#endif

    /* Set the display update state to wait for explicit updates. */
    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    /* Set the edit mode to replace. */
    pset_edit_mode( PEDIT_REPLACE );

    /* Open the structure.  Get input.  Move the box. */
    popen_struct( BOX );
#ifdef SAMPLE
    box_position.delta_x = box_position.delta_y = 0;
    for ( num_moves = 0; num_moves < MAX_MOVES; ) {
	psample_loc( WS_ID, (Pint) 1, &view, &locator_position );
	if ( locator_position.x != box_position.delta_x
	    || locator_position.y != box_position.delta_y ) {
	    ++num_moves;
	    box_position.delta_x = locator_position.x;
	    box_position.delta_y = locator_position.y;
	    ptranslate( &box_position, &err, xform );
	    pset_elem_ptr( (Pint) 1 );
	    pset_local_tran( xform, PTYPE_REPLACE );

	    /* Update the display. */
	    pupd_ws( WS_ID, PFLAG_PERFORM );
	} else
	    sleep( 1 );
    }
#endif
#ifdef REQUEST
    for ( num_moves = 0; num_moves < MAX_MOVES; num_moves++ ) {
	Pin_status	status;

	preq_loc( WS_ID, (Pint) 1, &status, &view, &locator_position );
	if ( status == PIN_STATUS_OK ) {
	    box_position.delta_x = locator_position.x;
	    box_position.delta_y = locator_position.y;
	    ptranslate( &box_position, &err, xform );
	    pset_elem_ptr( (Pint) 1 );
	    pset_local_tran( xform, PTYPE_REPLACE );

	    /* Update the display. */
	    pupd_ws( WS_ID, PFLAG_PERFORM );
	}
    }
#endif
#ifdef EVENT
    for ( num_moves = 0; num_moves < MAX_MOVES; num_moves++ ) {

	/* Prompt the user and wait for input. */
	printf( "Select a new box position\n" );
	pawait_event( TIMEOUT, &ws_id, &class, &device_num );

	/* See if a locator event was received, react accordingly. */
	if ( class == PIN_LOC ) {
	    /* Get the locator event from the PHIGS input queue. */
	    pget_loc( &view, &locator_position );

	    /* Move the box to the selected position by computing
	     * a new modelling transform and replacing the old one.
	     */
	    box_position.delta_x = locator_position.x;
	    box_position.delta_y = locator_position.y;
	    ptranslate( &box_position, &err, xform );
	    pset_elem_ptr( (Pint) 1 );
	    pset_local_tran( xform, PTYPE_REPLACE );

	    /* Update the display. */
	    pupd_ws( WS_ID, PFLAG_PERFORM );
	}
    }
#endif
    pclose_struct();

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
