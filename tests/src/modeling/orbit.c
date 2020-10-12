/* orbit.c 1.2 */
#include <math.h>
#include <phigs/phigs.h>

#define WS_ID	1
#define CIRCLE	1
#define SUN	2
#define PLANET	3
#define MOON	4
#define SYSTEM	5

#define SUN_RADIUS	0.12
#define PLANET_RADIUS	0.05
#define MOON_RADIUS	0.02
#define PLANET_ORBIT	(2.5 * SUN_RADIUS + PLANET_RADIUS)
#define MOON_ORBIT	(1.25 * PLANET_RADIUS + MOON_RADIUS)

#define NUM_CIRCLE_FACETS	8

static void rotate();

main()
{
    Ppoint_list	circle;
    Ppoint	circle_points[NUM_CIRCLE_FACETS + 1];
    double	angle, incr;
    Pmatrix	xform;
    Pint	err;
    Pvec	shift, scale;
    int		i;

    /* Make the unit circle for the generic heavenly body. */
    circle.num_points = NUM_CIRCLE_FACETS + 1;
    circle.points = circle_points;
    angle = 0; incr = (2*M_PI) / NUM_CIRCLE_FACETS;
    for ( i = 0; i <= NUM_CIRCLE_FACETS; i++ ) {
	circle_points[i].x = cos( angle );
	circle_points[i].y = sin( angle );
	angle += incr;
    }

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( CIRCLE );
	pfill_area( &circle );
    pclose_struct();

    popen_struct( SUN );
	/* Scale and instance the circle for the sun. */
	scale.delta_x = scale.delta_y = SUN_RADIUS;
	pscale( &scale, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pexec_struct( CIRCLE );

	/* Position and instance the planet. */
	protate( (Pfloat) 0, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	shift.delta_x = PLANET_ORBIT; shift.delta_y = 0.0;
	ptranslate( &shift, &err, xform );
	pset_local_tran( xform, PTYPE_PRECONCAT );
	pexec_struct( PLANET );
    pclose_struct();

    popen_struct( PLANET );
	/* Scale and instance the circle for the planet. */
	scale.delta_x = scale.delta_y = PLANET_RADIUS;
	pscale( &scale, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	pexec_struct( CIRCLE );

	/* Position and instance the moon. */
	protate( (Pfloat) 0, &err, xform );
	pset_local_tran( xform, PTYPE_REPLACE );
	shift.delta_x = MOON_ORBIT; shift.delta_y = 0.0;
	ptranslate( &shift, &err, xform );
	pset_local_tran( xform, PTYPE_PRECONCAT );
	pexec_struct( MOON );
    pclose_struct();

    popen_struct( MOON );
	/* Scale and instance the circle for the moon. */
	scale.delta_x = scale.delta_y = MOON_RADIUS;
	pscale( &scale, &err, xform );
	pset_local_tran( xform, PTYPE_PRECONCAT );
	pexec_struct( CIRCLE );
    pclose_struct();

    popen_struct( SYSTEM );
	/* Center the whole system in the default viewing plane. */
	shift.delta_x = shift.delta_y = 0.5;
	ptranslate( &shift, &err, xform );
	pset_global_tran( xform );
	pexec_struct( SUN );
    pclose_struct();

    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    ppost_struct( WS_ID , SYSTEM, (Pfloat) 1 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 2 );
    rotate();
    sleep( 2 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

#define NUM_INCREMENTS	40

static void
rotate()
{
    Pfloat	angle = 0, incr = (2*M_PI) / NUM_INCREMENTS;
    Pmatrix	xform;
    Pint	err;
    int		i;

    pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

    pset_edit_mode( PEDIT_REPLACE );
    for ( i = 0; i <= NUM_INCREMENTS; i++, angle += incr ) {
	/* Compute and apply the planet's transform for this angle. */
	protate( angle, &err, xform );
	popen_struct( SUN );
	    pset_elem_ptr( (Pint) 3 );
	    pset_local_tran( xform, PTYPE_REPLACE );
	pclose_struct();

	/* Compute and apply the moon's transform for this angle. */
	protate( 2 * angle, &err, xform );
	popen_struct( PLANET );
	    pset_elem_ptr( (Pint) 3 );
	    pset_local_tran( xform, PTYPE_REPLACE );
	pclose_struct();

	pupd_ws( WS_ID, PFLAG_PERFORM );
	sleep( 1 );
    }
}
