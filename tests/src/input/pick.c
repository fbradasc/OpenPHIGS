/* pick.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* The file include here has all the geometry and name definitions. */
#include "western_europe.h" /* see */

static void	handle_picking(); /* function defined below */

#define WS_ID		(Pint) 1

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

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

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

    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

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

#define DEVICE	(Pint) 1

#ifndef PICK2
static void
handle_picking()
{
    char		*name;
    Pfilter		filter;
    Pint		inclusion[1];
    Pin_status		status;
    Ppick_path		path;
    Ppick_path_elem	path_elems[2];
    int			i;

    /* Set the pick filter. */
    filter.incl_set.num_ints = 1;
    filter.incl_set.ints = inclusion;
    inclusion[0] = WESTERN_EUROPE;

    filter.excl_set.num_ints = 0;
    filter.excl_set.ints = (Pint *)NULL;

    pset_pick_filter( WS_ID, DEVICE, &filter );

    /* Get ten picks and print each selection. */
    path.path_list = path_elems;
    for ( i = 0; i < 10; i++ ) {
	preq_pick( WS_ID, DEVICE, (Pint) 2, &status, &path );
	if ( status == PIN_STATUS_OK ) {
	    switch ( path.path_list[1].pick_id ) {
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
		path.path_list[1].struct_id,
		path.path_list[1].elem_pos,
		path.path_list[1].pick_id );
	}
    }
}
#else
static void
handle_picking()
{
    char		*name;
    Pfilter		filter, *init_filter;
    Pint		inclusion[1], error, pet;
    Pin_status		status;
    Ppick_path		path, *init_path;
    Ppick_path_elem	path_elems[1];
    int			i;

    Pop_mode		mode;
    Pecho_switch	echo_sw;
    Plimit		echo_area;
    Ppath_order		order;
    Pstore		store;
    Ppick_data		*pick_data;

    /* Set the pick filter. */
    filter.incl_set.num_ints = 1;
    filter.incl_set.ints = inclusion;
    inclusion[0] = WESTERN_EUROPE;

    filter.excl_set.num_ints = 0;
    filter.excl_set.ints = (Pint *)NULL;
    pset_pick_filter( WS_ID, DEVICE, &filter );

    /* Inquire the default pick settings. */
    pcreate_store( &error, &store );
    pinq_pick_st( WS_ID, DEVICE, PINQ_SET, store, &error, &mode,
	&echo_sw, &init_filter, &status, &init_path, &pet, &echo_area,
	&pick_data, &order );

    /* Initialize the device, giving only a new path order. */
    pinit_pick( WS_ID, DEVICE, status, init_path, pet, &echo_area,
	pick_data, PORDER_BOTTOM_FIRST );
    pdel_store( store );

    /* Get ten picks, asking for a depth or 1. */
    path.path_list = path_elems;
    for ( i = 0; i < 10; i++ ) {
	preq_pick( WS_ID, DEVICE,  (Pint) 1, &status, &path );
	if ( status == PIN_STATUS_OK ) {
	    switch ( path.path_list[0].pick_id ) {
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
		path.path_list[0].struct_id,
		path.path_list[0].elem_pos,
		path.path_list[0].pick_id );
	}
    }
}
#endif
