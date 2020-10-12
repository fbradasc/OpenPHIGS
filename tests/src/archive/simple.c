/* simple.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */

#include <phigs/phigs.h>

/* Miscellaneous constants we use. */
#define STRUCT_ID	(Pint) 1
#define ARCHIVE_ID	(Pint) 1

main()
{
    static Ppoint3	points[] = {{.2,.2, 0}, {.4,.7, 0}, {.6,.3, 0}};
    static Ppoint_list3	point_list = {3, points};

    static Pint		structs[] = {STRUCT_ID};
    static Pint_list	struct_ids = {1, structs};

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCT_ID );
	ppolyline3( &point_list );
    pclose_struct();

    popen_ar_file( ARCHIVE_ID, "simple.phar" );
    par_struct_nets( ARCHIVE_ID, &struct_ids );
    pclose_ar_file( ARCHIVE_ID );

    pclose_phigs();
    return 0;
}
