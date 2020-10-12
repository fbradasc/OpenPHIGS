/* sulfate2.c 1.2 */
#include <math.h>
#include <phigs/phigs.h>

#define DEG_TO_RAD	(M_PI/180)

/* Atom and ion dimensions, all in Angstroms */
#define S_RADIUS	1.04
#define O_RADIUS	0.66
#define SO_BOND_LENGTH	1.44
#define SO_LENGTH	(S_RADIUS + O_RADIUS + SO_BOND_LENGTH)

/* Miscellaneous constants we use. */
#define WS_ID		(Pint) 1
#define MODEL		(Pint) 0
#define SPHERE		(Pint) 1
#define SULFUR		(Pint) 10
#define OXYGEN		(Pint) 11
#define SULFATE		(Pint) 1000

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

static Sphere_point	pos[4] = {
    {SO_LENGTH,                  0, 		 0},
    {SO_LENGTH,                  0, (109 * DEG_TO_RAD)},
    {SO_LENGTH, (120 * DEG_TO_RAD), (109 * DEG_TO_RAD)},
    {SO_LENGTH, (240 * DEG_TO_RAD), (109 * DEG_TO_RAD)}
};

main()
{
    Ppoint_list3	sphere, bond;
    Ppoint3		bond_points[2], fp;
    Pgcolr		color;
    Pmatrix3		translate, scale, rotate, rotate2, global;
    Pvec3		tr, sc;
    Pint		err;
    int			i;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    /* Create the sphere structure. */
    popen_struct( SPHERE );
	build_sphere( 1.0, &sphere );
	ppolyline3( &sphere );
    pclose_struct();

    /* Create the sulfur atom. */
    popen_struct( SULFUR );
	SET_COLOR( 1, 1, 0, color )
	pset_line_colr( &color );

	/* Scale and execute the sphere. */
	sc.delta_x = sc.delta_y = sc.delta_z = S_RADIUS;
	pscale3( &sc, &err, scale );
	pset_local_tran3( scale, PTYPE_REPLACE );
	pexec_struct( SPHERE );
    pclose_struct();

    /* Create the oxygen atom. */
    popen_struct( OXYGEN );
	SET_COLOR( 0, 1, 1, color )
	pset_line_colr( &color );

	/* Scale and execute the sphere. */
	sc.delta_x = sc.delta_y = sc.delta_z = O_RADIUS;
	pscale3( &sc, &err, scale );
	pset_local_tran3( scale, PTYPE_REPLACE );
	pexec_struct( SPHERE );
    pclose_struct();

    popen_struct( SULFATE );
	/* Instance the Sulfur atom. */
	pexec_struct( SULFUR );

	/* Position and instance the oxygen atoms. */
	for ( i = 0; i < 4; i++ ) {
	    /* Convert the position to Cartesion coordinates. */
	    tr.delta_x = pos[i].r * sin( pos[i].t ) * sin( pos[i].p );
	    tr.delta_y = pos[i].r * cos( pos[i].p );
	    tr.delta_z = pos[i].r * cos( pos[i].t ) * sin( pos[i].p );

	    /* Build the matrix and create the transform element. */
	    ptranslate3( &tr, &err, translate );
	    pset_local_tran3( translate, PTYPE_REPLACE );

	    /* Instance the oxygen structure. */
	    pexec_struct( OXYGEN );
	}

	/* Build the bonds. */
	SET_COLOR( 1, 0, 0, color )
	pset_line_colr( &color );
	bond.num_points = 2; bond.points = bond_points;
	bond_points[0].x = bond_points[0].z = 0;
	bond_points[0].y = S_RADIUS;
	bond_points[1].x = bond_points[1].z = 0;
	bond_points[1].y = S_RADIUS + SO_BOND_LENGTH;
	for ( i = 0; i < 4; i++ ) {
	    protate_x( pos[i].p, &err, rotate );
	    protate_y( pos[i].t, &err, rotate2 );
	    pcompose_matrix3( rotate2, rotate, &err, rotate );
	    pset_local_tran3( rotate, PTYPE_REPLACE );
	    ppolyline3( &bond );
	}
    pclose_struct();

    popen_struct( MODEL );
	/* Use the global transform to scale and center the ion. */
	sc.delta_x = sc.delta_y = sc.delta_z = 1 / (3 * SO_LENGTH);
	tr.delta_x = tr.delta_y = tr.delta_z = 0.5;
	fp.x = fp.y = fp.z = 0;
	pbuild_tran_matrix3( &fp, &tr, (Pfloat) 0, (Pfloat) 0,
	    (Pfloat) 0, &sc, &err, global );
	pset_global_tran3( global );

	/* Execute the sulfate ion. */
	pexec_struct( SULFATE );
    pclose_struct();

    /* Open the workstation and display the structure. */
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , MODEL, (Pfloat) 1.0 );
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
    float		radius;
    Ppoint_list3	*sphere;
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
