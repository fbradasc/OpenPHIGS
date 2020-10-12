/* text.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* Miscellaneous constants we use. */
#define WS_ID			(Pint) 1
#define STRUCTURE_ID		(Pint) 1

/* Utility function defined below. */
static void	set_view();

main()
{
    Ppoint3		text_pt, floor[4];
    Ppoint		text_pt2D;
    Pvec3		dir[2];
    Pgcolr		color;
    Ptext_align		alignment;
    Pvec		up_vector;
    Ppoint_list3	fill_area;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Open a structure and add the text and attribute elements. */
    popen_struct( STRUCTURE_ID );
	/* Set the view index to use the view we build below. */
	pset_view_ind( (Pint) 1 );

	/* First, define a blue floor as a reference frame. */
	color.type = PMODEL_RGB;
	color.val.general.x = 0.0;
	color.val.general.y = 0.0;
	color.val.general.z = 1.0;
	pset_int_colr( &color );	/* set the interior color */
	pset_int_style( PSTYLE_SOLID );	/* set the interior style */
	floor[0].x = 0.0; floor[0].y = 0.0; floor[0].z = 0.0;
	floor[1].x = 0.0; floor[1].y = 0.0; floor[1].z = 1.0;
	floor[2].x = 1.0; floor[2].y = 0.0; floor[2].z = 1.0;
	floor[3].x = 1.0; floor[3].y = 0.0; floor[3].z = 0.0;
	fill_area.num_points = 4;
	fill_area.points = floor;
	pfill_area3( &fill_area );

	/* Set the text precision to STROKE so that all the texti
	 * attributes are used.
	 */
	pset_text_prec( PPREC_STROKE );

	/* ----- First Text Primitive -----
	 * Put a 2D text primitive at the MC origin.  Set the
	 * height to 5% of the width of the floor.
	 */
	/* Set the character height. */
	pset_char_ht( (Pfloat) 0.05 );

	/* Set the text point and create the text element. */
	text_pt2D.x = text_pt2D.y = 0.0;
	ptext( &text_pt2D, "PHIGS Default Text Plane" );

	/* Set the text color to yellow for the rest of the examples. */
	color.type = PMODEL_RGB;
	color.val.general.x = 1.0;
	color.val.general.y = 1.0;
	color.val.general.z = 0.0;
	pset_text_colr( &color );

	/* ----- Second Text Primitive -----
	 * Put a text primitive in the X-Z plane, and raised slightly.
	 * The first direction vector is paralell to the X MC axis.  The
	 * second is parallel to the Z MC axis, but in the opposite
	 * direction.  Use the same character height as the previous
	 * primitive.  Center the text in the unit plane and add some
	 * spacing between characters.
	 */

	/* Center the string using text alignment. */
	alignment.hor = PHOR_CTR;
	alignment.vert = PVERT_HALF;
	pset_text_align( &alignment );

	/* Add some spacing -- 5% of the character height. */
	pset_char_space( (Pfloat) 0.05 );

	/* The first direction vector. */
	dir[0].delta_x = 1.0;
	dir[0].delta_y = 0.0;
	dir[0].delta_z = 0.0;

	/* The second direction vector. */
	dir[1].delta_x = 0.0;
	dir[1].delta_y = 0.0;
	dir[1].delta_z = -1.0;

	/* The text point. */
	text_pt.x = 0.5; text_pt.y = 0.01; text_pt.z = 0.5;

	/* Create the Text 3 structure element. */
	ptext3( &text_pt, dir, "PHIGS - X-Z plane" );

	/* ----- Third Text Primitive -----
	 * Put text parallel to the X-Y plane, but towards the viewer in
	 * Z.  Use a different font.  Don't reset the text alignment, so
	 * that the string will be centered on the text point.
	 */

	pset_text_font( (Pint) 2 );

	/* Reset the character spacing to no added spacing. */
	pset_char_space( (Pfloat) 0.0 );

	dir[0].delta_x = 1.0;
	dir[0].delta_y = 0.0;
	dir[0].delta_z = 0.0;
	dir[1].delta_x = 0.0;
	dir[1].delta_y = 1.0;
	dir[1].delta_z = 0.0;
	text_pt.x = 0.5; text_pt.y = 0.0; text_pt.z = 0.9;
	ptext3( &text_pt, dir, "PHIGS - X-Y plane" );

	/* ----- Fourth Text Primitive -----
	 * Put text in the Y-Z plane, centered.  Distort the characters
	 * with the character expansion factor and print the text
	 * vertically.  Rotate the text with the character up vector.
	 */

	/* Reset the text font. */
	pset_text_font( (Pint) 1 );

	/* Set the character expansion. */
	pset_char_expan( (Pfloat) 1.2 );

	/* Make text vertical. */
	pset_text_path( PPATH_DOWN );

	/* Rotate the text 45 degrees clockwise. */
	up_vector.delta_x = 1.0;
	up_vector.delta_y = 1.0;
	pset_char_up_vec( &up_vector );

	dir[0].delta_x = 0.0;
	dir[0].delta_y = 0.0;
	dir[0].delta_z = -1.0;
	dir[1].delta_x = 0.0;
	dir[1].delta_y = 1.0;
	dir[1].delta_z = 0.0;
	text_pt.x = 0.0; text_pt.y = 0.5; text_pt.z = 0.5;
	ptext3( &text_pt, dir, "PHIGS - Y-Z plane" );

	/* ----- Fifth Text Primitive -----
	 * Put text in a non-primary text plane.
	 */

	/* Reset the text attributes. */
	pset_char_ht( (Pfloat) 0.1 );
	pset_char_expan( (Pfloat) 1.0 );
	pset_text_path( PPATH_RIGHT );
	up_vector.delta_x = 0.0; up_vector.delta_y = 1.0;
	pset_char_up_vec( &up_vector );

	dir[0].delta_x = 1.0;
	dir[0].delta_y = 0.0;
	dir[0].delta_z = 0.0;
	dir[1].delta_x = 0.0;
	dir[1].delta_y = 1.0;
	dir[1].delta_z = -1.0;
	text_pt.x = 0.5; text_pt.y = 0.7; text_pt.z = 0.3;
	ptext3( &text_pt, dir, "PHIGS - Tilted" );
    pclose_struct();

    /* Open the workstation, set a view, and post the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    set_view();	/* This function is defined below. */
    ppost_struct( WS_ID, STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure the structure is traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
set_view()
{
    Pview_rep3		view_rep;
    Pview_map3		view_map;
    Pint		err;

    /* The view orientation parameters. */
    static Ppoint3	view_ref_pt = {0.5,0.5,0.5};
    static Pvec3	view_up_vec = {0,1,0};
    static Pvec3	view_plane_normal = {0.8,1,3};

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &err, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PERSPECT;
    view_map.proj_ref_point.x = 0.5;
    view_map.proj_ref_point.y = 0.5;
    view_map.proj_ref_point.z = 6.0;
    view_map.win.x_min = -1.0; view_map.win.x_max =  1.1;
    view_map.win.y_min = -1.2; view_map.win.y_max =  1.0;
    view_map.back_plane = -1.2; view_map.front_plane = 1.2;
    view_map.view_plane = 0.0;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;

    /* Compute the view mapping matrix. */
    peval_view_map_matrix3( &view_map, &err, view_rep.map_matrix );

    /* Set the view representation. */
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = PIND_CLIP;
    view_rep.back_clip = PIND_CLIP;
    view_rep.clip_limit.x_min = 0.0; view_rep.clip_limit.x_max = 1.0;
    view_rep.clip_limit.y_min = 0.0; view_rep.clip_limit.y_max = 1.0;
    view_rep.clip_limit.z_min = 0.0; view_rep.clip_limit.z_max = 1.0;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
}
