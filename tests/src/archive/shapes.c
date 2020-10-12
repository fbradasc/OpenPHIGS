/* shapes.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */

#include <phigs/phigs.h>
#include <math.h>
#include <stdlib.h>

/* Miscellaneous constants we use. */
#define WS_ID		(Pint) 1
#define ARCHIVE_ID	(Pint) 1
#define CUBE		(Pint) 1
#define CYLINDER	(Pint) 2
#define SPHERE		(Pint) 3

/* Functions defined below that build the objects. */
static void	build_cube(), build_cylinder(), build_sphere();

static Pint		shapes[] = {CUBE, CYLINDER, SPHERE};
static Pint_list	root_ids = {3, shapes};

static int ar_file_open( Pint archive_id );

main()
{
    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    build_cube();
    build_cylinder();
    build_sphere();

    /* Open the archive file and check the open. */
    popen_ar_file( ARCHIVE_ID, "shapes.phar" );
    if ( !ar_file_open( ARCHIVE_ID ) ) {
	fprintf( stderr, "Cannot open archive file\n" );
	exit( 1 );
    }

    /* Archive the shapes. */
    par_struct_nets( ARCHIVE_ID, &root_ids );

    /* Close the archive file. */
    pclose_ar_file( ARCHIVE_ID );

    pclose_phigs();
    return 0;
}

static int
ar_file_open( archive_id )
    Pint	archive_id;
{
    int			i, status = 0;
    Pint		error;
    Pstore		store;
    Par_file_list	*ar_files;

    pcreate_store( &error, &store );
    if ( error ) {
	fprintf( stderr, "Cannot create store object\n" );
	return 0;
    }

    pinq_ar_files( store, &error, &ar_files );
    if ( error ) {
	fprintf( stderr, "Error %d from pinq_ar_files\n", error );

    } else {
	/* Search the list of open archive files. */
	for ( i = 0; i < ar_files->num_ar_files; i++ ) {
	    if ( ar_files->ar_files[i].id == archive_id ) {
		status = 1;
		break; /* out of for loop */
	    }
	}
    }

    pdel_store( store );
    return status;
}

#define SET_VERTEX_COORD( v, _x, _y, _z ) \
    (v).x = (_x), (v).y = (_y), (v).z = (_z)

#define SET_VERTEX_COORD_COLOR( v, _x, _y, _z, _r, _g, _b ) \
    (v).point.x = (_x), \
    (v).point.y = (_y), \
    (v).point.z = (_z), \
    (v).colr.direct.rgb.red = (_r), \
    (v).colr.direct.rgb.green = (_g), \
    (v).colr.direct.rgb.blue = (_b)

#define NO_COLOR	(Pint) -1

static void
build_cube()
{
    Ppoint3		vertex_data[8];   /* eight vertices */
    Pfacet_vdata_list3	vertices;

    static Pint		indices[6][4] = {
    {3, 7, 6, 2}, {0, 4, 7, 3}, {0, 3, 2, 1},
    {4, 5, 6, 7}, {1, 2, 6, 5}, {0, 1, 5, 4}};

    static Pint_list	contours[6] = {
    {4, indices[0]}, {4, indices[1]}, {4, indices[2]},
    {4, indices[3]}, {4, indices[4]}, {4, indices[5]}};

    static Pint_list_list	contour_lists[6] = {
    {1, &contours[0]}, {1, &contours[1]}, {1, &contours[2]},
    {1, &contours[3]}, {1, &contours[4]}, {1, &contours[5]}};

    /* Build the vertex list. */
    vertices.num_vertices = 8;
    vertices.vertex_data.points = vertex_data;
    SET_VERTEX_COORD( vertex_data[0], 0, 0, 1 );
    SET_VERTEX_COORD( vertex_data[1], 1, 0, 1 );
    SET_VERTEX_COORD( vertex_data[2], 1, 0, 0 );
    SET_VERTEX_COORD( vertex_data[3], 0, 0, 0 );
    SET_VERTEX_COORD( vertex_data[4], 0, 1, 1 );
    SET_VERTEX_COORD( vertex_data[5], 1, 1, 1 );
    SET_VERTEX_COORD( vertex_data[6], 1, 1, 1 );
    SET_VERTEX_COORD( vertex_data[7], 0, 1, 0 );

    popen_struct( CUBE );
    pset_of_fill_area_set3_data( PFACET_NONE, PEDGE_NONE, PVERT_COORD,
	NO_COLOR, (Pint) 6, (Pfacet_data_arr3 *) NULL,
	(Pedge_data_list *)NULL, contour_lists, &vertices );
    pclose_struct();
}

#define NUM_FACETS	16
#define NUM_POINTS	(NUM_FACETS + 1)
#define RADIUS		0.5

static void
build_cylinder()
{
    Ppoint3		vertex_data[2 * NUM_POINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    double		theta = 0;
    int			i;

    /* Compute the bottom row of points. */
    for ( i = 0; i < NUM_FACETS; i++ ) {
	vertex_data[i].x = RADIUS * (1 + cos( theta ));
	vertex_data[i].y = 0;
	vertex_data[i].z = RADIUS * (1 + sin( theta ));
	theta += (2 * M_PI) / NUM_FACETS;
    }

    /* Close the cylinder. */
    vertex_data[NUM_FACETS].x = vertex_data[0].x;
    vertex_data[NUM_FACETS].y = vertex_data[0].y;
    vertex_data[NUM_FACETS].z = vertex_data[0].z;

    /* Compute the top row of points. */
    for ( i = 0; i < NUM_POINTS; i++ ) {
	vertex_data[NUM_POINTS + i].x = vertex_data[i].x;
	vertex_data[NUM_POINTS + i].y = 1.0;
	vertex_data[NUM_POINTS + i].z = vertex_data[i].z;
    }

    popen_struct( CYLINDER );
	vertices.points = vertex_data;
	array_dims.size_x = NUM_POINTS;
	array_dims.size_y = 2;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertices );
    pclose_struct();
}

#define NUM_LONG_FACETS		32
#define NUM_LONG_POINTS		(NUM_LONG_FACETS + 1)
#define NUM_LAT_FACETS		16
#define NUM_LAT_POINTS		(NUM_LAT_FACETS + 1)

static void
build_sphere()
{
    Ppoint3		vertex_data[NUM_LONG_POINTS * NUM_LAT_POINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    double		theta = 0, phi = 0;
    int			i, j, index;

    for ( i = 0; i < NUM_LAT_POINTS; i++ ) {
	for ( j = 0; j < NUM_LONG_FACETS; j++ ) {
	    index = i * NUM_LONG_POINTS + j;
	    vertex_data[index].x = RADIUS * (1 + sin(phi) * cos(theta));
	    vertex_data[index].y = RADIUS * (1 + cos(phi));
	    vertex_data[index].z = RADIUS * (1 + sin(phi) * sin(theta));
	    theta += (2 * M_PI) / NUM_LONG_FACETS;
	}
	phi += M_PI / NUM_LAT_FACETS;

	/* Close the circle. */
	index = i * NUM_LONG_POINTS + NUM_LONG_FACETS;
	vertex_data[index].x = vertex_data[index - NUM_LONG_FACETS].x;
	vertex_data[index].y = vertex_data[index - NUM_LONG_FACETS].y;
	vertex_data[index].z = vertex_data[index - NUM_LONG_FACETS].z;
    }

    popen_struct( SPHERE );
	vertices.points = vertex_data;
	array_dims.size_x = NUM_LONG_POINTS;
	array_dims.size_y = NUM_LAT_POINTS;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertices );
    pclose_struct();
}
