/* bundles.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCT_ID	(Pint) 1
#define HIGHWAY		(Pint) 1
#define STREET		(Pint) 2
#define TRAIL  		(Pint) 3

static Ppoint		points[] = {{.2,.8},{.8,.8}};
static Ppoint_list	point_list = {2, points};

main()
{
    Pline_bundle_plus	bundle;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Initialize the common bundle values. */
    bundle.colr.type = PMODEL_RGB;
    bundle.shad_meth = 1;
    bundle.approx_type = 1;
    bundle.approx_val = 1;

    /* Set the line bundles. */
    bundle.type = PLINE_SOLID;
    bundle.width = 3;
    bundle.colr.val.general.x = 1;
    bundle.colr.val.general.y = 0;
    bundle.colr.val.general.z = 0;
    pset_line_rep_plus( WS_ID, HIGHWAY, &bundle );

    bundle.type = PLINE_DASH;
    bundle.width = 2;
    bundle.colr.val.general.x = 0;
    bundle.colr.val.general.y = 0;
    bundle.colr.val.general.z = 1;
    pset_line_rep_plus( WS_ID, STREET, &bundle );

    bundle.type = PLINE_DOT;
    bundle.width = 1;
    bundle.colr.val.general.x = 0;
    bundle.colr.val.general.y = 1;
    bundle.colr.val.general.z = 0;
    pset_line_rep_plus( WS_ID, TRAIL, &bundle );

    popen_struct( STRUCT_ID );
	/* Set the asfs for line type, width and color to bundled. */
	pset_indiv_asf( PASPECT_LINETYPE, PASF_BUNDLED );
	pset_indiv_asf( PASPECT_LINEWIDTH, PASF_BUNDLED );
	pset_indiv_asf( PASPECT_LINE_COLR_IND, PASF_BUNDLED );

	/* Create the polylines, setting the line index before each. */
	pset_line_ind( HIGHWAY );
	ppolyline( &point_list );

	pset_line_ind( STREET );
	points[0].y = points[1].y = 0.5;
	ppolyline( &point_list );

	pset_line_ind( TRAIL );
	points[0].y = points[1].y = 0.2;
	ppolyline( &point_list );
    pclose_struct();

    ppost_struct( WS_ID, STRUCT_ID, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 10 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
