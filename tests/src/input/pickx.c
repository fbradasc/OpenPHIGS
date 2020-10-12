/* pickx.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* The file include here has all the geometry and name definitions. */
#include "western_europe.h" /* see */

static void	handle_picking(); /* function defined below */

#define WS_ID		(Pint) 1
#define WS_GEOMETRY	"500x500+50+50"

static Display		*display;
static Window		window;

main()
{
    Pgcolr	color;
    Pint	error, names[2];
    Pint_list	name_list;
    Ppoint	vrp;
    Pvec	vup;
    Pview_map	view_map;
    Pview_rep	view_rep;
    int		i;

    /* Use the utility function we wrote in Chapter 3. */
    open_xphigs_and_xlib_ws( WS_ID, WS_GEOMETRY, &display, &window );

    /* Create the structure for each country. */
    name_list.ints = names;
    name_list.num_ints = 2;
    for ( i = 0; i < NUM_COUNTRIES; i++ ) {
	popen_struct( countries[i].name );
	    /* Add this country's name and group to the name set. */
	    names[0] = countries[i].name;
	    names[1] = countries[i].group;
	    padd_names_set( &name_list );

	    /* Add a pick id. */
	    pset_pick_id( countries[i].name );

	    /* Define the geometry. */
	    pfill_area_set( countries[i].points );
	pclose_struct();
    }

    popen_struct( WESTERN_EUROPE );
	pset_view_ind( (Pint) 1 );
	color.type = PMODEL_RGB;
	color.val.general.x = 1;
	color.val.general.y = 0;
	color.val.general.z = 1;
	pset_int_colr( &color ); /* magenta */

	/* Put all countries in the WESTERN_EUROPE group. */
	names[0] = WESTERN_EUROPE;
	name_list.num_ints = 1;
	padd_names_set( &name_list );

	for ( i = 0; i < NUM_COUNTRIES; i++ )
	    pexec_struct( countries[i].name );
    pclose_struct();

    /* Set the 2D view. */
    vrp.x = 0; vrp.y = 0;
    vup.delta_x = 0; vup.delta_y = 1;
    peval_view_ori_matrix( &vrp, &vup, &error, view_rep.ori_matrix );
    view_map.win.x_min = -0.1; view_map.win.x_max = 1.7;
    view_map.win.y_min = -0.1; view_map.win.y_max = 1.7;
    view_map.proj_vp.x_min = 0; view_map.proj_vp.x_max = 1;
    view_map.proj_vp.y_min = 0; view_map.proj_vp.y_max = 1;
    peval_view_map_matrix( &view_map, &error, view_rep.map_matrix );
    view_rep.clip_limit = view_map.proj_vp;
    view_rep.xy_clip = PIND_CLIP;
    pset_view_rep( WS_ID, (Pint) 1, &view_rep );

    /* Post the structure and update the workstation. */
    ppost_struct( WS_ID, WESTERN_EUROPE, (Pfloat) 0.1 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Go get and respond to input. */
    handle_picking();

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
handle_picking()
{
    char		*name;
    Pint		inclusion[1], error, ws_type;
    Ppick_path		*path;
    Pstore		store;
    Pdisp_space_size3	dc_info;
    Pconnid_x_drawable	*conn_id;
    Pescape_in_data	esc_in;
    Pescape_out_data	*esc_out;
    XEvent		event;
    int			done = 0;

    /* Create a store for the inquiries and the escape. */
    pcreate_store( &error, &store );

    /* Set the escape input parameters. */
    esc_in.escape_in_u4.ws_id = WS_ID;
    esc_in.escape_in_u4.ap_size = 5.0;
    esc_in.escape_in_u4.order = PORDER_BOTTOM_FIRST;
    esc_in.escape_in_u4.depth = 1;
    esc_in.escape_in_u4.pet = 1;
    esc_in.escape_in_u4.echo_switch = PSWITCH_ECHO;

    /* Set the pick filter. */
    esc_in.escape_in_u4.filter.incl_set.num_ints = 1;
    esc_in.escape_in_u4.filter.incl_set.ints = inclusion;
    inclusion[0] = WESTERN_EUROPE;
    esc_in.escape_in_u4.filter.excl_set.num_ints = 0;
    esc_in.escape_in_u4.filter.excl_set.ints = (Pint *)NULL;

    /* Set the echo volume to all of DC. */
    pinq_ws_conn_type( WS_ID, store, &error, (void **)&conn_id, &ws_type );
    pinq_disp_space_size3( ws_type, &error, &dc_info );
    esc_in.escape_in_u4.echo_volume.x_min = 0.0;
    esc_in.escape_in_u4.echo_volume.y_min = 0.0;
    esc_in.escape_in_u4.echo_volume.z_min = 0.0;
    esc_in.escape_in_u4.echo_volume.x_max = dc_info.size_dc.size_x;
    esc_in.escape_in_u4.echo_volume.y_max = dc_info.size_dc.size_y;
    esc_in.escape_in_u4.echo_volume.z_max = dc_info.size_dc.size_z;

    /* Get picks and print each selection. */
    XSelectInput( display, window,
	ButtonPressMask | KeyPressMask | ExposureMask );
    while ( !done ) {
	XNextEvent( display, &event );
	switch ( event.type ) {
	    case ButtonPress:
		/* Call the escape to resolve the pick. */
		esc_in.escape_in_u4.point.x = event.xbutton.x;
		esc_in.escape_in_u4.point.y = event.xbutton.y;
		pescape( PUESC_DRAWABLE_POINT_TO_PICK, &esc_in, store,
		    &esc_out );

		path = &esc_out->escape_out_u4.pick;
		if ( esc_out->escape_out_u4.status == PIN_STATUS_OK ) {
		    switch ( path->path_list[0].pick_id ) {
			case AUSTRIA: name = "AUSTRIA"; break;
			case BELGIUM: name = "BELGIUM"; break;
			case DENMARK: name = "DENMARK"; break;
			case SPAIN: name = "SPAIN"; break;
			case FRANCE: name = "FRANCE"; break;
			case GERMANY: name = "GERMANY"; break;
			case ITALY: name = "ITALY"; break;
			case LUXEMBURG: name = "LUXEMBURG"; break;
			case NETHERLANDS: name = "NETHERLANDS"; break;
			case PORTUGAL:	name = "PORTUGAL"; break;
			case IRELAND:	name = "IRELAND"; break;
			case SWITZERLAND: name = "SWITZERLAND"; break;
			case UNITED_KINGDOM: name = "UNITED_KINGDOM"; break;
		    }
		    printf( "%s: (struct = %d, elem = %d, id = %d)\n", name,
			path->path_list[0].struct_id,
			path->path_list[0].elem_pos,
			path->path_list[0].pick_id );
		}
		break;

	    case KeyPress:
		/* Done picking. */
		pdel_store( store );
		done = 1;
		break;

	    case Expose:
		if ( event.xexpose.count == 0 )
		    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
		break;
	}
    }
}
