/* css_probe.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

void	print_all_structure(), print_structure(), print_element();

/* The local store object used by the element inquiry function. */
static Pstore	store = (Pstore) NULL;

void
print_all_structures()
{
    Pint	error, num_structs, i;
    Pint_list	ids;

    /* Determine how many structures there are. */
    pinq_struct_ids( (Pint) 0, (Pint) 0, &error, &ids, &num_structs );

    if ( !error && num_structs > 0 ) {
	/* Allocate space for the list and retrieve all the ids. */
	ids.ints = (Pint *)malloc( num_structs * sizeof(Pint) );
	pinq_struct_ids( num_structs, (Pint) 0, &error, &ids,
	    &num_structs );

	/* Loop through the list and print the structure contents. */
	for ( i = 0; i < ids.num_ints; i++ )
	    print_structure( ids.ints[i] );
    }

    if ( store != (Pstore) NULL )
       pdel_store( store );
}

void
print_structure( struct_id )
    Pint	struct_id;
{
    Pint		error, num_elements, element;
    Pstruct_status	status;
    Pstruct_st		struct_state;

    /* Close the open structure if one is open. */
    pinq_struct_st( &struct_state );
    if ( struct_state == PSTRUCT_ST_STOP )
	pclose_struct();

    /* See if it has any elements in it. */
    pinq_struct_status( struct_id, &error, &status );
    if ( status == PSTRUCT_STATUS_EMPTY )
	printf( "\nStructure %d, (empty)\n", struct_id );

    else if ( status == PSTRUCT_STATUS_NOT_EMPTY ) {
	/* Determine how many elements are in the structure by opening
	 * it and inquiring the element pointer.  See the text ()
	 * for a description of this technique.
	 */
	popen_struct( struct_id );
	pinq_elem_ptr( &error, &num_elements );
	pclose_struct();
	printf( "\nStructure %d, %d elements\n", struct_id, num_elements );

	/* Loop through all the elements and print their contents. */
	for ( element = 1; element <= num_elements; element++ )
	    print_element( struct_id, element );
    }
}

static void	print_polyline3(),
		print_polyline(),
		print_label();

void
print_element( struct_id, element )
    Pint	struct_id;
    Pint	element;
{
    Pint	error;
    Pelem_type	el_type;
    Pelem_data	*el_data;
    size_t	size;

    /* Create the memory buffer if not already created. */
    if ( store == (Pstore) NULL ) {
	pcreate_store( &error, &store );
	if ( error ) {
	    fprintf( stderr, "Cannot create memory buffer\n" );
	    return;
	}
    }

    /* Retrieve the element type and contents. */
    pinq_elem_type_size( struct_id, element, &error, &el_type, &size );
    if ( !error && el_type != PELEM_NIL )
	pinq_elem_content( struct_id, element, store, &error, &el_data );

    /* Were the type and contents successfully retrieved? */
    if ( !error )
	printf( "\tElement %d: ", element );
    else {
	fprintf( stderr, "Error %d inquiring type or contents\n", error );
	return;
    }

    /* Branch on the element type and print the contents. */
    switch ( el_type ) {
	case PELEM_POLYLINE3:
	    print_polyline3( el_data );
	    break;

	case PELEM_POLYLINE:
	    print_polyline( el_data );
	    break;

	case PELEM_LABEL:
	    print_label( el_data );
	    break;
	
	default:
	    fprintf( stderr, "element type not supported yet.\n" );
	    break;
    }
}

static void
print_polyline3( el_data )
    Pelem_data	*el_data;
{
    int		i;

    printf( "type = Polyline3 (3D)\n" );
    printf( "\t\t%d points:\n", el_data->point_list3.num_points );
    for ( i = 0; i < el_data->point_list3.num_points; i++ )
	printf( "\t\t\tpoint %d = ( %g, %g, %g)\n", i+1,
	    el_data->point_list3.points[i].x,
	    el_data->point_list3.points[i].y,
	    el_data->point_list3.points[i].z );
}

static void
print_polyline( el_data )
    Pelem_data	*el_data;
{
    int		i;

    printf( "type = Polyline (2D)\n" );
    printf( "\t\t%d points:\n", el_data->point_list.num_points );
    for ( i = 0; i < el_data->point_list.num_points; i++ )
	printf( "\t\t\tpoint %d = ( %g, %g)\n", i+1,
	    el_data->point_list.points[i].x,
	    el_data->point_list.points[i].y );
}

static void
print_label( el_data )
    Pelem_data	*el_data;
{
    printf( "type = Label\n" );
    printf( "\t\tlabel = %d\n", el_data->int_data );
}
main()
{
    static Ppoint3	points3[] = {{.2,.2,0},{.4,.7,0},{.6,.3,0},{.8,.8,0}};
    static Ppoint_list3	point_list3 = {4, points3};
    static Ppoint	points[] = {{.2,.2},{.4,.7},{.6,.3},{.8,.8}};
    static Ppoint_list	point_list = {4, points};

	point_list3.num_points = 4;
	point_list3.points = points3;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_struct( 9 );
	plabel( 2 );
	ppolyline3( &point_list3 );
	plabel( 4 );
	plabel( 4 );
	plabel( 6 );
	ppolyline( &point_list );
    pclose_struct();

    popen_struct( 22 );
	plabel( 3 );
	plabel( 5 );
	plabel( 9 );
	plabel( 9 );
	ppolyline( &point_list );
	ppolyline3( &point_list3 );
    pclose_struct();

    popen_struct( 69 );
    pclose_struct();

    print_all_structures();

    pclose_phigs();
    return 0;
}
