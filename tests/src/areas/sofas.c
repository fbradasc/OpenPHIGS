/* sofas.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

#define WS_ID		(Pint) 1
#define STRUCTURE_ID	(Pint) 1

#define SET_VERTEX_COORD( v, _x, _y, _z ) \
    {(v).x = (_x); (v).y = (_y); (v).z = (_z);}

static int set_view();

main()
{
    Ppoint3		vertex_data[8];   /* eight vertices */
    Pfacet_vdata_list3	vertices;
    Pint_list_list	contour_lists[6]; /* six fill area sets */
    Pint_list		contours[6];	  /* each with one contour */
    Pint		indices[6][4];	  /* of four indices each */

    /* Build the vertex list. */
    SET_VERTEX_COORD( vertex_data[0], .3, .3, .7 )
    SET_VERTEX_COORD( vertex_data[1], .7, .3, .7 )
    SET_VERTEX_COORD( vertex_data[2], .7, .3, .3 )
    SET_VERTEX_COORD( vertex_data[3], .3, .3, .3 )
    SET_VERTEX_COORD( vertex_data[4], .3, .7, .7 )
    SET_VERTEX_COORD( vertex_data[5], .7, .7, .7 )
    SET_VERTEX_COORD( vertex_data[6], .7, .7, .3 )
    SET_VERTEX_COORD( vertex_data[7], .3, .7, .3 )
    vertices.num_vertices = 8;
    vertices.vertex_data.points = vertex_data;

    /* Build the contour lists. */
    /* Front Face. */
    contour_lists[0].num_lists = 1;
    contour_lists[0].lists = &contours[0];
    contours[0].num_ints = 4;
    contours[0].ints = indices[0];
    indices[0][0] = 0; indices[0][1] = 1;
    indices[0][2] = 5; indices[0][3] = 4;

    /* Right Face. */
    contour_lists[1].num_lists = 1;
    contour_lists[1].lists = &contours[1];
    contours[1].num_ints = 4;
    contours[1].ints = indices[1];
    indices[1][0] = 1; indices[1][1] = 2;
    indices[1][2] = 6; indices[1][3] = 5;

    /* Back Face. */
    contour_lists[2].num_lists = 1;
    contour_lists[2].lists = &contours[2];
    contours[2].num_ints = 4;
    contours[2].ints = indices[2];
    indices[2][0] = 2; indices[2][1] = 3;
    indices[2][2] = 7; indices[2][3] = 6;

    /* Left Face. */
    contour_lists[3].num_lists = 1;
    contour_lists[3].lists = &contours[3];
    contours[3].num_ints = 4;
    contours[3].ints = indices[3];
    indices[3][0] = 3; indices[3][1] = 0;
    indices[3][2] = 4; indices[3][3] = 7;

    /* Bottom Face. */
    contour_lists[4].num_lists = 1;
    contour_lists[4].lists = &contours[4];
    contours[4].num_ints = 4;
    contours[4].ints = indices[4];
    indices[4][0] = 0; indices[4][1] = 3;
    indices[4][2] = 2; indices[4][3] = 1;

    /* Top Face. */
    contour_lists[5].num_lists = 1;
    contour_lists[5].lists = &contours[5];
    contours[5].num_ints = 4;
    contours[5].ints = indices[5];
    indices[5][0] = 4; indices[5][1] = 5;
    indices[5][2] = 6; indices[5][3] = 7;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );

    popen_struct( STRUCTURE_ID );
	/* Set the view index to the view we build below. */
	pset_view_ind( (Pint) 1 );

	/* Create the SOFAS element. */
	pset_of_fill_area_set3_data( PFACET_NONE, PEDGE_NONE,
	    PVERT_COORD, PMODEL_RGB, (Pint) 6, (Pfacet_data_arr3 *)NULL,
	    (Pedge_data_list *)NULL, contour_lists, &vertices );
    pclose_struct();

    popen_ws( WS_ID, (void *)NULL, phigs_ws_type_x_tool );
    set_view();	/* This function is defined below. */

    /* Post the structure. */
    ppost_struct( WS_ID , STRUCTURE_ID, (Pfloat) 1.0 );

    /* Ensure the structure is traversed. */
    pupd_ws( WS_ID, PFLAG_PERFORM );

    sleep( 60 );

    pclose_ws( WS_ID );
    pclose_phigs();
    return 0;
}

static int
set_view()
{
    Pview_rep3		view_rep;
    Pview_map3		view_map;
    Pint		err;

    /* The view orientation parameters. */
    static Ppoint3	view_ref_pt = {0.5,0.5,0.5};
    static Pvec3	view_plane_normal = {2,0.5,0.5};
    static Pvec3	view_up_vec = {0,1,0};

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &err, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PARAL;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 20.0;
    view_map.win.x_min = -0.5; view_map.win.x_max = 0.5;
    view_map.win.y_min = -0.5; view_map.win.y_max = 0.5;
    view_map.back_plane = -100.0; view_map.front_plane = 100.0;
    view_map.view_plane = 0.0;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;

    /* Compute the view mapping matrix. */
    peval_view_map_matrix3( &view_map, &err, view_rep.map_matrix );

    /* Set the view representation. */
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = PIND_CLIP;
    view_rep.back_clip = PIND_CLIP;
    view_rep.clip_limit.x_min = 0.0; view_rep.clip_limit.x_max = 1.0;
    view_rep.clip_limit.y_min = 0.0; view_rep.clip_limit.y_max = 1.0;
    view_rep.clip_limit.z_min = 0.0; view_rep.clip_limit.z_max = 1.0;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
}
