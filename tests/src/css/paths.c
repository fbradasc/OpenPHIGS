/* paths.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

void print_ancestors(), print_descendants();

void
print_ancestors( struct_id, flag )
    Pint	struct_id;
    int		flag;
{
    Pint			error, i, j;
    Pstore			store;
    Pelem_ref_list_list		*paths;

    pcreate_store( &error, &store );
    if ( !error )
	pinq_paths_ances( struct_id, PORDER_TOP_FIRST, (Pint) 0,
	    store, &error, &paths );

    if ( error ) {
	fprintf( stderr, "Error %d inquiring ancestors\n", error );
	return;
    }

    printf( "%d ancestor paths for structure %d:\n",
	paths->num_elem_ref_lists, struct_id );
    for ( i = 0; i < paths->num_elem_ref_lists; i++ ) {
	printf( "\tpath %d:", i+1 );
	for( j = 0; j < paths->elem_ref_lists[i].num_elem_refs; j++ )
	    printf( " (%d, %d)",
		paths->elem_ref_lists[i].elem_refs[j].struct_id,
		paths->elem_ref_lists[i].elem_refs[j].elem_pos );
	printf( "\n" );
    }

    pdel_store( store );
}
void
print_descendants( struct_id, flag )
    Pint	struct_id;
    int		flag;
{
    Pint			error, i, j;
    Pstore			store;
    Pelem_ref_list_list		*paths;

    pcreate_store( &error, &store );
    if ( !error )
	pinq_paths_descs( struct_id, PORDER_TOP_FIRST, (Pint) 0,
	    store, &error, &paths );

    if ( error ) {
	fprintf( stderr, "Error %d inquiring descendants\n", error );
	return;
    }

    printf( "%d descendant paths for structure %d:\n",
	paths->num_elem_ref_lists, struct_id );
    for ( i = 0; i < paths->num_elem_ref_lists; i++ ) {
	printf( "\tpath %d:", i+1 );
	for( j = 0; j < paths->elem_ref_lists[i].num_elem_refs; j++ )
	    printf( " (%d, %d)",
		paths->elem_ref_lists[i].elem_refs[j].struct_id,
		paths->elem_ref_lists[i].elem_refs[j].elem_pos );
	printf( "\n" );
    }

    pdel_store( store );
}

int
structure_is_executed( struct_id )
    Pint	struct_id;
{
    Pint			error;
    Pstore			store;
    Pelem_ref_list_list		*paths;

    pcreate_store( &error, &store );
    pinq_paths_ances( struct_id, PORDER_BOTTOM_FIRST, (Pint) 1, store,
	&error, &paths );
    pdel_store( store );

    return ( paths->num_elem_ref_lists > 0 ) ? 1 : 0;
}

#define STRUCT_A	(Pint) 1
#define STRUCT_B	(Pint) 2
#define STRUCT_C	(Pint) 3
#define STRUCT_D	(Pint) 4

main()
{
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Inquire Ancestors example. */
    popen_struct( STRUCT_A );
	plabel( (Pint) 0 );
	plabel( (Pint) 0 );
	pexec_struct( STRUCT_D );
	plabel( (Pint) 0 );
    pclose_struct();

    popen_struct( STRUCT_B );
	pexec_struct( STRUCT_C );
	plabel( (Pint) 0 );
	plabel( (Pint) 0 );
    pclose_struct();

    popen_struct( STRUCT_C );
	plabel( (Pint) 0 );
	plabel( (Pint) 0 );
	plabel( (Pint) 0 );
	pexec_struct( STRUCT_D );
    pclose_struct();

    print_ancestors( STRUCT_D );
    pdel_struct( STRUCT_A );
    pdel_struct( STRUCT_B );
    pdel_struct( STRUCT_C );
    pdel_struct( STRUCT_D );

    /* Inquire Descendants example. */
    popen_struct( STRUCT_A );
	plabel( (Pint) 0 );
	pexec_struct( STRUCT_B );
	plabel( (Pint) 0 );
	pexec_struct( STRUCT_C );
    pclose_struct();

    popen_struct( STRUCT_B );
	plabel( (Pint) 0 );
	plabel( (Pint) 0 );
	pexec_struct( STRUCT_D );
	plabel( (Pint) 0 );
    pclose_struct();

    print_descendants( STRUCT_A );

    if ( structure_is_executed( STRUCT_B ) )
	printf( "Structure B is executed\n" );
    else
	printf( "Structure B is not executed\n" );

    pclose_phigs();
    return 0;
}
