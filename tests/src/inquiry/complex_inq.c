/* complex_inq.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

void
print_ar_files()
{
    Pint		error, i;
    Pstore		store;
    Par_file_list	*ar_files;

    pcreate_store( &error, &store );
    if ( !error ) {
	pinq_ar_files( store, &error, &ar_files );
	if ( !error ) {
	    for ( i = 0; i < ar_files->num_ar_files; i++ )
		printf( "Archive id = %d, Name is %s\n",
		    ar_files->ar_files[i].id,
		    ar_files->ar_files[i].name );
	}
	pdel_store( store );
    }
}
