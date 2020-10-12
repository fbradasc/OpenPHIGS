/* first.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WORKSTATION_ID	(Pint) 1
#define STRUCTURE_ID	(Pint) 1

main()
{
    Ppoint_list3	point_list;

    static Ppoint3	points[] = {{.2,.2,0},{.4,.7,0},{.6,.3,0},{.8,.8,0}};

    point_list.num_points = 4;
    point_list.points = points;
    
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	pset_linetype( PLINE_DASH );
	ppolyline3( &point_list );
    pclose_struct();

    popen_ws( WORKSTATION_ID, NULL, phigs_ws_type_x_tool );
    ppost_struct( WORKSTATION_ID, STRUCTURE_ID, (Pfloat) 1.0 );
    pupd_ws( WORKSTATION_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WORKSTATION_ID );
    pclose_phigs();
}
