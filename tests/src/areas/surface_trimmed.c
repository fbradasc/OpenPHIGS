/* surface_trimmed.c 1.3 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID	(Pint) 1
#define MODEL	(Pint) 1

static void set_view(); /* defined below */

/* The surface knots and control points. */
static	Pfloat		uknots[] = { 0, 0, 0, 1, 1, 1 };
static	Pfloat		vknots[] = { 0, 0, 0, 0, 1, 1, 1, 1 };
static	Ppoint3		surf_points[] = {
    {0.0, 0.5, 0.0}, {0.0, 0.6, 0.5}, {0.0, 0.5, 1.0},
    {0.3, 0.5, 0.0}, {0.3, 1.0, 0.5}, {0.3, 0.5, 1.0},
    {0.6, 0.4, 0.0}, {0.6, 0.5, 0.5}, {0.6, 0.4, 1.0},
    {0.9, 0.5, 0.0}, {0.9, 0.3, 0.5}, {0.9, 0.2, 1.0}};

/* Trimming Curves, from inner-most to outer-most. */
static	Pfloat		t0_knots[] = {0,0,0,1,1,2,2,3,3,4,4,4 };
static	Ppoint		t0_pts[] = {
    {0.5,0.32},{0.68,0.32},{0.68,0.5},{0.68,0.68},{0.5,0.68},
    {0.32,0.68},{0.32,0.5},{0.32,0.32},{0.5,0.32}};

static	Pfloat		t1_knots[] = {0,0,0,1,1,2,2,3,3,4,4,4 };
static	Ppoint		t1_pts[] = {
    {0.5,0.25},{0.75,0.25},{0.75,0.5},{0.75,0.75},{0.5,0.75},
    {0.25,0.75},{0.25,0.5},{0.25,0.25},{0.5,0.25}};

static	Pfloat		t2_knots[] = { 0, 0, 1, 2, 3, 4, 4 };
static	Ppoint		t2_pts[] =
    {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};

main()
{
    Pint		uorder, vorder;
    Pfloat_list		uknot_list, vknot_list;
    Ppoint_grid34	grid;
    Ppara_surf_characs	psc;
    Ptrimcurve_list	trim_loops[3];
    Ptrimcurve		trims[3];
    Pint		num_loops;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );

    /* Assign the surface parameters. */
    uorder = 3, vorder = 4;
    grid.num_points.u_dim = 3;
    grid.num_points.v_dim = 4;
    grid.points.point3d = surf_points;
    uknot_list.num_floats = sizeof(uknots)/sizeof(uknots[0]);
    uknot_list.floats = uknots;
    vknot_list.num_floats = sizeof(vknots)/sizeof(vknots[0]);
    vknot_list.floats = vknots;

    /* Build the trimming loops. */
    num_loops = 3;
    trim_loops[0].num_curves = 1;
    trim_loops[0].curves = &trims[0];
    trim_loops[1].num_curves = 1;
    trim_loops[1].curves = &trims[1];
    trim_loops[2].num_curves = 1;
    trim_loops[2].curves = &trims[2];

    trims[0].rationality = PNON_RATIONAL;
    trims[0].visible = PEDGE_ON;
    trims[0].order = 3;
    trims[0].tmin = 0;
    trims[0].tmax = 4;
    trims[0].knots.num_floats = sizeof(t0_knots)/sizeof(t0_knots[0]);
    trims[0].knots.floats = t0_knots;
    trims[0].cpts.num_points = sizeof(t0_pts)/sizeof(t0_pts[0]);
    trims[0].cpts.points.point2d = t0_pts;
    trims[0].approx_type = PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
    trims[0].approx_val = 10;

    trims[1].rationality = PNON_RATIONAL;
    trims[1].visible = PEDGE_ON;
    trims[1].order = 3;
    trims[1].tmin = 0;
    trims[1].tmax = 4;
    trims[1].knots.num_floats = sizeof(t1_knots)/sizeof(t1_knots[0]);
    trims[1].knots.floats = t1_knots;
    trims[1].cpts.num_points = sizeof(t1_pts)/sizeof(t1_pts[0]);
    trims[1].cpts.points.point2d = t1_pts;
    trims[1].approx_type = PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
    trims[1].approx_val = 10;

    trims[2].rationality = PNON_RATIONAL;
    trims[2].visible = PEDGE_ON;
    trims[2].order = 2;
    trims[2].tmin = 0;
    trims[2].tmax = 4;
    trims[2].knots.num_floats = sizeof(t2_knots)/sizeof(t2_knots[0]);
    trims[2].knots.floats = t2_knots;
    trims[2].cpts.num_points = sizeof(t2_pts)/sizeof(t2_pts[0]);
    trims[2].cpts.points.point2d = t2_pts;
    trims[2].approx_type = PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
    trims[2].approx_val = 10;
    
    /* Create the structure. */
    popen_struct( MODEL );
	pset_view_ind( (Pint) 1 );
	pset_int_style( PSTYLE_EMPTY );

	/* Set the parametric surface characteristics. */
	psc.psc_3.placement = PCP_UNIFORM;
	psc.psc_3.u_count = 20;
	psc.psc_3.v_count = 20;
	pset_para_surf_characs( PSC_ISOPARAMETRIC_CURVES, &psc );

	/* Set the surface approximation criteria. */
	pset_surf_approx( PSURF_CONSTANT_PARAMETRIC_BETWEEN_KNOTS,
	    (Pfloat) 10.0, (Pfloat) 10.0 );

	/* Create the surface. */
	pset_edge_flag( PEDGE_ON );
	pnuni_bsp_surf( uorder, vorder, &uknot_list, &vknot_list,
	    PNON_RATIONAL, &grid, num_loops, trim_loops );
    pclose_struct();

    /* Set the view, and post the top-level structure. */
    set_view();
    ppost_struct( WS_ID, MODEL, (Pfloat) 1.0 );
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 30 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static void
set_view()
{
    Pvec3	view_up_vec, view_plane_normal;
    Ppoint3	view_ref_pt;
    Pview_rep3	view_rep;
    Pview_map3	view_map;
    Pint	error;

    /* Calculate the view orientation matrix. */
    view_ref_pt.x = 0.5;
    view_ref_pt.y = 0.5;
    view_ref_pt.z = 0.5;
    view_up_vec.delta_x = 0;
    view_up_vec.delta_y = 1;
    view_up_vec.delta_z = 0;
    view_plane_normal.delta_x = 0.5643;
    view_plane_normal.delta_y = 0.4775;
    view_plane_normal.delta_z = 0.6735;
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &error, view_rep.ori_matrix );

    /* Calculate the view mapping matrix (). */
    view_map.proj_type = PTYPE_PARAL;
    view_map.view_plane = 0.0;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 10;
    view_map.win.x_min = -0.78; view_map.win.x_max = 0.7;
    view_map.win.y_min = -0.78; view_map.win.y_max = 0.7;
    view_map.back_plane = -1; view_map.front_plane = 1;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;
    peval_view_map_matrix3( &view_map, &error, view_rep.map_matrix );

    /* Store the view in the view table as view 1. */
    view_rep.clip_limit = view_map.proj_vp;
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = view_rep.back_clip = PIND_CLIP;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
}
