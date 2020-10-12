/* map.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <string.h>

/* The file include here has all the geometry and name definitions. */
#include "western_europe.h" /* see */

#define WS_ID		(Pint) 1

static void	handle_highlighting_requests(); /* defined below */

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
	for ( i = 0; i < NUM_COUNTRIES; i++ ) {
	    pexec_struct( countries[i].name );
	}
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
    handle_highlighting_requests();

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

#define INCLUDE 1
#define EXCLUDE 2

static void
handle_highlighting_requests()
{
    int		c, op = INCLUDE;
    Pint	name;
    Pfilter	filter;
    Pint	inclusion[NUM_COUNTRIES], exclusion[NUM_COUNTRIES];

    filter.incl_set.ints = inclusion;
    filter.excl_set.ints = exclusion;

    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );
    filter.incl_set.num_ints = filter.excl_set.num_ints = 0;
    while ( (c = getchar()) != EOF ) {
	if ( strrchr( "abdefgilnprsuxEF", c ) ) {
	    /* Determine which country to highlight or exclude. */
	    switch ( c ) {
		case 'a': name = AUSTRIA; break;
		case 'b': name = BELGIUM; break;
		case 'd': name = DENMARK; break;
		case 'e': name = SPAIN; break;
		case 'f': name = FRANCE; break;
		case 'g': name = GERMANY; break;
		case 'i': name = ITALY; break;
		case 'l': name = LUXEMBURG; break;
		case 'n': name = NETHERLANDS; break;
		case 'p': name = PORTUGAL; break;
		case 'r': name = IRELAND; break;
		case 's': name = SWITZERLAND; break;
		case 'u': name = UNITED_KINGDOM; break;
		case 'E': name = EC; break;
		case 'F': name = EFTA; break;
		case 'x': op = EXCLUDE; /* change the operation */
		    continue;
	    }

	    /* Set it to be highlighted or excluded. */
	    if ( op == INCLUDE )
		inclusion[filter.incl_set.num_ints++] = name;
	    else if ( op == EXCLUDE )
		exclusion[filter.excl_set.num_ints++] = name;

	} else if ( c = 10 ) /* carriage return */ {
		/* Set the highlight filter with the new info. */
		pset_highl_filter( WS_ID, &filter );
		pupd_ws( WS_ID, PFLAG_PERFORM );

		/* Reset for next input. */
		filter.incl_set.num_ints = filter.excl_set.num_ints = 0;
		op = INCLUDE;
	}
    }
}
