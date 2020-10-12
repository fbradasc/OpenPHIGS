/* sphere.c 1.3 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_ID		(Pint) 1
#define MODEL		(Pint) 1
#define SPHERE		(Pint) 2
#define PLANE		(Pint) 3

static void	normalize(), build_plane(), build_sphere();

main()
{
    Plight_src_bundle	light_def;
    Pint		lights[4];
    Pint_list		lights_on, lights_off;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Enable HLHSR on the workstation. */
    pset_hlhsr_mode( WS_ID, PHIGS_HLHSR_MODE_ZBUFF );

    /* Define the lights. */
    light_def.type = PLIGHT_AMBIENT;
    light_def.rec.ambient.colr.type = PMODEL_RGB;
    light_def.rec.ambient.colr.val.general.x = .3;
    light_def.rec.ambient.colr.val.general.y = .3;
    light_def.rec.ambient.colr.val.general.z = .3;
    pset_light_src_rep( WS_ID, (Pint) 1, &light_def );

    light_def.type = PLIGHT_DIRECTIONAL;
    light_def.rec.directional.dir.delta_x = 1;
    light_def.rec.directional.dir.delta_y = 0;
    light_def.rec.directional.dir.delta_z = 0;
    normalize( &light_def.rec.directional.dir );
    light_def.rec.directional.colr.type = PMODEL_RGB;
    light_def.rec.directional.colr.val.general.x = 0.4;
    light_def.rec.directional.colr.val.general.y = 0.4;
    light_def.rec.directional.colr.val.general.z = 0.4;
    pset_light_src_rep( WS_ID, (Pint) 2, &light_def );

    light_def.type = PLIGHT_POSITIONAL;
    light_def.rec.positional.pos.x = 0.5;
    light_def.rec.positional.pos.y = 0.5;
    light_def.rec.positional.pos.z = 0.5;
    light_def.rec.positional.colr.type = PMODEL_RGB;
    light_def.rec.positional.colr.val.general.x = 0.4;
    light_def.rec.positional.colr.val.general.y = 0.4;
    light_def.rec.positional.colr.val.general.z = 0.0;
    light_def.rec.positional.coef[0] = 1;
    light_def.rec.positional.coef[1] = 1;
    pset_light_src_rep( WS_ID, (Pint) 3, &light_def );

    light_def.type = PLIGHT_SPOT;
    light_def.rec.spot.pos.x = 1;
    light_def.rec.spot.pos.y = 1;
    light_def.rec.spot.pos.z = 1;
    light_def.rec.spot.dir.delta_x = -1;
    light_def.rec.spot.dir.delta_y = -1;
    light_def.rec.spot.dir.delta_z = -1;
    normalize( &light_def.rec.spot.dir );
    light_def.rec.spot.exp = 75;
    light_def.rec.spot.angle = M_PI/4;
    light_def.rec.spot.colr.type = PMODEL_RGB;
    light_def.rec.spot.colr.val.general.x = 0.7;
    light_def.rec.spot.colr.val.general.y = 0.7;
    light_def.rec.spot.colr.val.general.z = 0.7;
    light_def.rec.spot.coef[0] = 1;
    light_def.rec.spot.coef[1] = 0.2;
    pset_light_src_rep( WS_ID, (Pint) 4, &light_def );

    popen_struct( MODEL );
	/* Set the interior style, reflectance equation, shading
	 * method, face culling mode, and turn HLHSR on.
	 */
	pset_int_style( PSTYLE_SOLID );
	pset_refl_eqn( PREFL_AMB_DIFF_SPEC );
	pset_int_shad_meth( PSD_COLOUR );
	pset_face_cull_mode( PCULL_BACKFACE );
	pset_hlhsr_id( PHIGS_HLHSR_ID_ON );

	/* Turn the lights on. */
	lights_off.num_ints = 0;
	lights_on.num_ints = 4;
	lights_on.ints = lights;
	lights[0] = 1; lights[1] = 2; lights[2] = 3; lights[3] = 4;
	pset_light_src_state( &lights_on, &lights_off );

	pexec_struct( PLANE );
	pexec_struct( SPHERE );
    pclose_struct();

    build_plane();
    build_sphere();

    ppost_struct( WS_ID, MODEL, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
normalize( vector )
    Pvec3	*vector;
{
    float	r;

    /* Transform a vector to a unit vector. */
    r = vector->delta_x * vector->delta_x
	+ vector->delta_y * vector->delta_y
	+ vector->delta_z * vector->delta_z;
    if ( r > 0 ) {
	r = 1/sqrt( r );
	vector->delta_x *= r;
	vector->delta_y *= r;
	vector->delta_z *= r;
    }
}

#define NUM_X_POINTS	30
#define NUM_Y_POINTS	30
#define NO_COLOR	(Pint) -1

static void
build_plane()
{
    int			i, j, index;
    Ppoint3		pts[NUM_X_POINTS * NUM_Y_POINTS];
    Pfacet_vdata_arr3	plane;
    Pint_size		array_dims;

    array_dims.size_x = NUM_X_POINTS;
    array_dims.size_y = NUM_Y_POINTS;
    plane.points = pts;
    for ( j = 0; j < NUM_Y_POINTS; j++ ) {
	for ( i = 0; i < NUM_X_POINTS; i++ ) {
	    index = j * NUM_X_POINTS + i;
	    pts[index].x = 0.1 + i * 0.8/(NUM_X_POINTS - 1);
#ifdef SunPHIGS2.0
	    pts[index].y = 0.1 + j * 0.8/(NUM_Y_POINTS - 1);
#else
	    pts[index].y = 0.9 - j * 0.8/(NUM_Y_POINTS - 1);
#endif
	    pts[index].z = 0;
	}
    }

    popen_struct( PLANE );
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &plane );
    pclose_struct();
}

#define NUM_LONG_FACETS		80
#define NUM_LAT_FACETS		40
#define NUM_LONG_POINTS		(NUM_LONG_FACETS + 1)
#define NUM_LAT_POINTS		(NUM_LAT_FACETS + 1)
#define RADIUS			0.2

static void
build_sphere()
{
    Pptnorm3		vertex_data[NUM_LONG_POINTS * NUM_LAT_POINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    Ppoint3		location;
    Pmatrix3		transform;
    Pint		error;
    double		theta = 0, phi = 0;
    int			i, j, index;

    for ( i = 0; i < NUM_LAT_POINTS; i++ ) {
	for ( j = 0; j < NUM_LONG_FACETS; j++ ) {
	    index = i * NUM_LONG_POINTS + j;
#ifdef SunPHIGS2.0
	    vertex_data[index].point.x = RADIUS * sin(phi) * cos(theta);
	    vertex_data[index].point.y = RADIUS * cos(phi);
	    vertex_data[index].point.z = RADIUS * sin(phi) * sin(theta);
#else
	    vertex_data[index].point.x = RADIUS * sin(phi) * sin(theta);
	    vertex_data[index].point.y = RADIUS * cos(phi);
	    vertex_data[index].point.z = RADIUS * sin(phi) * cos(theta);
#endif
	    /* The normal is just the vector to the point. */
	    vertex_data[index].norm.delta_x = vertex_data[index].point.x;
	    vertex_data[index].norm.delta_y = vertex_data[index].point.y;
	    vertex_data[index].norm.delta_z = vertex_data[index].point.z;
	    normalize( &vertex_data[index].norm );
	    theta += (2 * M_PI) / NUM_LONG_FACETS;
	}
	phi += M_PI / NUM_LAT_FACETS;

	/* Close the circle. */
	index = i * NUM_LONG_POINTS + NUM_LONG_FACETS;
	vertex_data[index].point = vertex_data[index - NUM_LONG_FACETS].point;
	vertex_data[index].norm = vertex_data[index - NUM_LONG_FACETS].norm;
    }

    popen_struct( SPHERE );
	location.x = location.y = location.z = 0.5;
	ptranslate3( &location, &error, transform );
	pset_local_tran3( transform, PTYPE_REPLACE );
	vertices.ptnorms = vertex_data;
	array_dims.size_x = NUM_LONG_POINTS;
	array_dims.size_y = NUM_LAT_POINTS;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD_NORMAL, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertices );
    pclose_struct();
}
