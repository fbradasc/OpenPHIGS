/* houses.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WORKSTATION_ID	(Pint) 1
#define HOUSE_A		(Pint) 1
#define HOUSE_B		(Pint) 2
#define BACKGROUND	(Pint) 4
#define TREE		(Pint) 5

static Ppoint
	grass_pts[] = {{.05,.05},{.95,.05},{.95,.3},{.05,.3}},
	sky_pts[] = {{.05,.3},{.95,.3},{.95,.95},{.05,.95}},
	tree_pts[] = {{.3,.2},{.4,.2},{.35,.4}},
	house_A_pts[] = {{.6,.15},{.8,.15},{.8,.4}, {.7,.5}, {.6,.4}},
	chimney_A_pts[] = {{.75,.45},{.8,.4},{.8,.53}, {.75,.53}},
	house_B_pts[] = {{.5,.15},{.8,.15},{.8,.3}, {.65,.35}, {.5,.4}},
	chimney_B_pts[] = {{.7,.333},{.75,.317},{.75,.4}, {.7,.4}};

static Ppoint_list
	grass		= {4, grass_pts},
	sky		= {4, sky_pts},
	tree		= {3, tree_pts},
	house_A		= {5, house_A_pts},
	chimney_A	= {4, chimney_A_pts},
	house_B		= {5, house_B_pts},
	chimney_B	= {4, chimney_B_pts};

/* Define a macro to set a color conveniently. */
#define SET_COLOR( colr, r, g, b ) { \
	    colr.type = PMODEL_RGB;	\
	    colr.val.general.x = r;	\
	    colr.val.general.y = g;	\
	    colr.val.general.z = b;	\
	}

main()
{
    Pint	current_house;
    Pgcolr	red, green, blue, yellow, dark_green;

    /* Define the colors we'll use. */
    SET_COLOR( red, 1, 0, 0 )
    SET_COLOR( green, 0, 1, 0 )
    SET_COLOR( blue, 0, 0, 1 )
    SET_COLOR( dark_green, 0, .7, 0 )
    SET_COLOR( yellow, 1, 1, 0 )

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( BACKGROUND );
	pset_int_style( PSTYLE_SOLID );
	pset_int_colr( &blue );
	pfill_area( &sky );
	pset_int_colr( &dark_green );
	pfill_area( &grass );
    pclose_struct();

    popen_struct( TREE );
	pset_int_style( PSTYLE_SOLID );
	pset_int_colr( &green );
	pfill_area( &tree );
    pclose_struct();

    popen_struct( HOUSE_A );
	pset_int_style( PSTYLE_SOLID );
	pset_int_colr( &yellow );
	pfill_area( &house_A );
	pset_int_colr( &red );
	pfill_area( &chimney_A );
    pclose_struct();

    popen_struct( HOUSE_B );
	pset_int_style( PSTYLE_SOLID );
	pset_int_colr( &yellow );
	pfill_area( &house_B );
	pset_int_colr( &red );
	pfill_area( &chimney_B );
    pclose_struct();

    popen_ws( WORKSTATION_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Set the update state to wait for explicit update calls. */
    pset_disp_upd_st( WORKSTATION_ID, PDEFER_WAIT, PMODE_NIVE );

    ppost_struct( WORKSTATION_ID, BACKGROUND, (Pfloat) 0.1 );
    ppost_struct( WORKSTATION_ID, TREE, (Pfloat) 1.0 );

    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );

    current_house = 0;
    printf( "carriage return shows alternate houses...\\n" );
    while ( getchar() != EOF ) {
	punpost_struct( WORKSTATION_ID, current_house );
	current_house = ( current_house == HOUSE_A ? HOUSE_B : HOUSE_A );
	ppost_struct( WORKSTATION_ID, current_house, (Pfloat) 1.0 );
	pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );
    }

    pclose_ws( WORKSTATION_ID );
    pclose_phigs();
    return 0;
}
