/* buttons.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WORKSTATION_ID	(Pint) 1
#define STRUCTURE_ID	(Pint) 1

/* Define the generic button geometry. */
static Ppoint		button_pts[] = {{0,0},{.15,0},{.15,.15},{0,.15}};
static Ppoint_list	button_list = {4, button_pts};
static Ppoint_list_list	button = {1, &button_list};

/* Define the background for the buttons. */
static Ppoint		bkgd_pts[] = {{.8,0},{1,0},{1,1},{.8,1}};
static Ppoint_list	bkgd_list = {4, bkgd_pts};
static Ppoint_list_list	bkgd = {1, &bkgd_list};

#define SET_COLOR( colr, r, g, b ) { \
	    colr.type = PMODEL_RGB;	\
	    colr.val.general.x = r;	\
	    colr.val.general.y = g;	\
	    colr.val.general.z = b;	\
	}

main()
{
    Pint		err;
    Pgcolr		blue, yellow, black;
    Ppoint		text_pt;
    Ptext_align		text_align;
    Pmatrix		xform;
    Pvec		position;

    /* Define the colors. */
    SET_COLOR( blue, 0, 0, 1 )
    SET_COLOR( yellow, 1, 1, 0 )
    SET_COLOR( black, 0, 0, 0 )

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	pset_int_style( PSTYLE_SOLID );
	pset_edge_flag( PEDGE_ON );

	/* Create the background under the buttons. */
	pset_int_colr( &blue );
	pfill_area_set( &bkgd );

	/* Set the button and text colors. */
	pset_int_colr( &yellow );
	pset_text_colr( &black );

	/* Size and align the text at the center of the buttons. */
	pset_char_ht( (Pfloat) 0.02 );
	text_align.hor = PHOR_CTR;
	text_align.vert = PVERT_HALF;
	pset_text_align( &text_align );
	text_pt.x = 0.5 * (button_pts[0].x + button_pts[1].x);
	text_pt.y = 0.5 * (button_pts[1].y + button_pts[2].y);

	/* Create the buttons, top to bottom. */
	position.delta_x = 0.825; position.delta_y = 0.8;
	ptranslate( &position, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pfill_area_set( &button );
	ptext( &text_pt, "Pan" );

	position.delta_x = 0.825; position.delta_y = 0.55;
	ptranslate( &position, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pfill_area_set( &button );
	ptext( &text_pt, "Zoom" );

	position.delta_x = 0.825; position.delta_y = 0.3;
	ptranslate( &position, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pfill_area_set( &button );
	ptext( &text_pt, "Snap" );

	position.delta_x = 0.825; position.delta_y = 0.05;
	ptranslate( &position, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pfill_area_set( &button );
	ptext( &text_pt, "Cut" );
    pclose_struct();

    popen_ws( WORKSTATION_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WORKSTATION_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WORKSTATION_ID );
    pclose_phigs();
    return 0;
}
