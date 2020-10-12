/* sulfate.c 1.2 */
#include <math.h>
#include <phigs/phigs.h>

#define DEG_TO_RAD	(M_PI/180)

/* Atom and ion dimensions, all in Angstroms */
#define S_RADIUS	1.04
#define O_RADIUS	0.66
#define SO_BOND_LENGTH	1.44
#define SO_LENGTH	(S_RADIUS + SO_BOND_LENGTH + O_RADIUS)

/* Miscellaneous constants we use. */
#define WS_ID		(Pint) 1
#define SULFATE		(Pint) 1

#define SET_COLOR( r, g, b, colr ) { \
	    (colr).type = PMODEL_RGB;	\
	    (colr).val.general.x = r;	\
	    (colr).val.general.y = g;	\
	    (colr).val.general.z = b;	\
	}

/* Utility function defined below. */
static void	build_sphere();

/* The atom locations are in spherical coordinates. */
typedef struct {
    float	r, t, p; /* radius, theta, phi */
} Sphere_point;

static Sphere_point	pos[] = {
    {SO_LENGTH,			 0,		     0},
    {SO_LENGTH,                  0, (109 * DEG_TO_RAD)},
    {SO_LENGTH, (120 * DEG_TO_RAD), (109 * DEG_TO_RAD)},
    {SO_LENGTH, (240 * DEG_TO_RAD), (109 * DEG_TO_RAD)}
};

main()
{
    Ppoint_list3	sulfur, oxygen, bond;
    Ppoint3		bond_points[2];
    Pgcolr		color;
    Pmatrix3		translate, scale, rotate, global;
    Pvec3		tr, sc;
    Pint		err;
    int			i;

    /* Build the spheres for the sulfur and oxygen atoms. */
    build_sphere( S_RADIUS, &sulfur );
    build_sphere( O_RADIUS, &oxygen );

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( SULFATE );
	/* Compute the global scaling transform. */
	sc.delta_x = sc.delta_y = sc.delta_z = 1 / (3 * SO_LENGTH);
	pscale3( &sc, &err, scale );

	/* Compute the global translation transform. */
	tr.delta_x = tr.delta_y = tr.delta_z = 0.5;
	ptranslate3( &tr, &err, translate );

	/* Compose them and create the Set Global Transform element. */
	pcompose_matrix3( translate, scale, &err, global );
	pset_global_tran3( global );

	/* Create the yellow sulfur atom. */
	SET_COLOR( 1, 1, 0, color )
	pset_line_colr( &color );
	ppolyline3( &sulfur );

	/* Create the cyan oxygen atoms and position them with local
	 * modeling transforms.
	 */
	SET_COLOR( 0, 1, 1, color )
	pset_line_colr( &color );
	for ( i = 0; i < 4; i++ ) {
	    /* Convert each position to Cartesion coordinates. */
	    tr.delta_x = pos[i].r * sin( pos[i].t ) * sin( pos[i].p );
	    tr.delta_y = pos[i].r * cos( pos[i].p );
	    tr.delta_z = pos[i].r * cos( pos[i].t ) * sin( pos[i].p );

	    /* Build the matrix and create the transform element. */
	    ptranslate3( &tr, &err, translate );
	    pset_local_tran3( translate, PTYPE_REPLACE );

	    ppolyline3( &oxygen );
	}

	/* Build the bonds, which are red. */
	SET_COLOR( 1, 0, 0, color )
	pset_line_colr( &color );

	/* Define the polyline along the Y axis. */
	bond.num_points = 2; bond.points = bond_points;
	bond_points[0].x = bond_points[0].z = 0;
	bond_points[0].y = S_RADIUS;
	bond_points[1].x = bond_points[1].z = 0;
	bond_points[1].y = S_RADIUS + SO_BOND_LENGTH;

	/* Rotate the bond polyline into the correct positions. */
	for ( i = 0; i < 4; i++ ) {
	    protate_y( pos[i].t, &err, rotate );
	    pset_local_tran3( rotate, PTYPE_REPLACE );
	    protate_x( pos[i].p, &err, rotate );
	    pset_local_tran3( rotate, PTYPE_PRECONCAT );
	    ppolyline3( &bond );
	}
    pclose_struct();

    /* Open the workstation and display the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , SULFATE, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    /* Stop to look at the picture. */
    sleep( 10 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

#define NUM_LAT_SEGS	20
#define NUM_LON_SEGS	30

static void
build_sphere( radius, sphere )
    float		radius;	/* IN: The radius of the sphere. */
    Ppoint_list3	*sphere;	/* OUT: The polyline. */
{
    /* Build a "wire" sphere, using a single spiral polyline. */

    double	phi, delta_phi, theta, delta_theta;
    int		i;

    sphere->num_points = NUM_LAT_SEGS * NUM_LON_SEGS + 1;
    sphere->points = (Ppoint3 *)
	malloc( (unsigned) (sphere->num_points * sizeof(Ppoint3)) );

    phi = 0; /* start at the North pole */
    delta_phi = M_PI / (NUM_LAT_SEGS * NUM_LON_SEGS);

    theta = 0;
    delta_theta = (2 * M_PI) / NUM_LON_SEGS;

    for ( i = 0; i < sphere->num_points; i++ ) {
	sphere->points[i].x = radius * sin( phi ) * sin( theta );
	sphere->points[i].y = radius * cos( phi );
	sphere->points[i].z = radius * sin( phi ) * cos( theta );
	theta += delta_theta;
	phi += delta_phi;
    }
}
