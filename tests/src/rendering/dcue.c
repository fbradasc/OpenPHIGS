/* dcue.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>
#include <math.h>

/* Miscellaneous constants we use. */
#define WS_ID		(Pint) 1
#define MODEL		(Pint) 1

static Ppoint3		prim_pts[][4] = {
    {{.2,.3,1},{.4,.3,1},{.4,.7,0},{.2,.7,0}},
    {{.6,.3,1},{.8,.3,1},{.8,.7,0},{.6,.7,0}}};
static Ppoint_list3	prims[] = {{4, prim_pts[0]}, {4, prim_pts[1]}};

main()
{
    Pdcue_bundle	dcue_rep;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Set depth cue table entry 1. */
    dcue_rep.mode = PALLOWED;
    dcue_rep.ref_planes[0] = 0.1;
    dcue_rep.ref_planes[1] = 0.9;
    dcue_rep.scaling[0] = 0;
    dcue_rep.scaling[1] = 1;
    /* Make the depth cue color the background color. */
    dcue_rep.colr.type = PINDIRECT;
    dcue_rep.colr.val.ind = 0;
    pset_dcue_rep( WS_ID, (Pint) 1, &dcue_rep );

    popen_struct( MODEL );
	pset_int_style( PSTYLE_SOLID );

	/* Use the default depth cue entry for the left primitve. */
	pfill_area3( &prims[0] );

	/* Use our new depth cue entry for the right primitive. */
	pset_dcue_ind( (Pint) 1 );
	pfill_area3( &prims[1] );
    pclose_struct();

    ppost_struct( WS_ID, MODEL, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 30 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}
