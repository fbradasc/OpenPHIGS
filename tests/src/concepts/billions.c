/* billions.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WORKSTATION_ID	(Pint) 1
#define STRUCTURE_ID	(Pint) 1

static Ppoint	outer_border[4] = {{0,0},{1,0},{1,1},{0,1}};
static Ppoint	inner_border[4] =
	{{.05,.05},{.95,.05},{.95,.95},{.05,.95}};
static Ppoint	bar_1900_points[] =
	{{.28,.05},{.38,.05},{.38,.19},{.28,.19}};
static Ppoint	bar_2000_points[] =
	{{.61,.05},{.71,.05},{.71,.6},{.61,.6}};

static Ppoint_list	border[2] = {{4,inner_border}, {4, outer_border}};
static Ppoint_list	bar_1900 = {4, bar_1900_points};
static Ppoint_list	bar_2000 = {4, bar_2000_points};

static Ppoint	title_location = {.18, .82};
static Ppoint	subtitle_location = {.38, .75};
static Ppoint	label_location_1900 = {.20, .15};
static Ppoint	label_location_2000 = {.53, .15};
static Ppoint	amount_location_1900 = {.28, .24};
static Ppoint	amount_location_2000 = {.61, .65};

main()
{
    Ppoint_list_list	set;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* The border. */
	pset_edge_flag( PEDGE_ON );
	pset_int_style( PSTYLE_SOLID );
	set.num_point_lists = 2;
	set.point_lists = border;
	pfill_area_set( &set );

	/* The title. */
	pset_char_ht( (Pfloat) .04 );
	ptext( &title_location, "World Population" );
	pset_char_ht( (Pfloat) .02 );
	ptext( &subtitle_location, "(in billions)" );

	/* The 1900 bar. */
	pfill_area( &bar_1900 );

	/* The 1900 bar label. */
	pset_char_ht( (Pfloat) .015 );
	ptext( &label_location_1900, "1900" );
	ptext( &amount_location_1900, "1.6" );

	/* The 2000 bar. */
	pset_int_style( PSTYLE_EMPTY );
	pset_edgetype( PLINE_DASH );
	set.num_point_lists = 1;
	set.point_lists = &bar_2000;
	pfill_area_set( &set );

	/* The 2000 bar label. */
	ptext( &label_location_2000, "2000" );
	ptext( &amount_location_2000, "6.1 (projected)" );
    pclose_struct();

    popen_ws( WORKSTATION_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WORKSTATION_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WORKSTATION_ID );
    pclose_phigs();
    return 0;
}
