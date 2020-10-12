/* light_tool.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */

/*  Use this program to explore PHIGS rendering.  There are three objects,
 *  each centered at the center of the unit cube, (0.5, 0.5, 0.5).  Four
 *  light sources are defined, one of each type: ambient, directional,
 *  positional, and spot.  The direction of the directional light is
 *  <-1, 0, 0>.  The positional light is directly above the objects, at
 *  position (0.5, 2.0, 0.5).  The spot light is at the same position; its
 *  direction is <0, -1, 0>.  All other light parameters can be set
 *  interactively, as can the reflectance properties.   To see the effect
 *  of modifying any one light's parameters, you usually need to turn all
 *  the other lights off, except maybe the ambient light.
 *
 *  This program is very limited.  Don't expect too much of it.  Feel free
 *  to modify it to suit your taste.  Please send bugs or suggestions to
 *  phigs@ora.com.
 *
 *  The raw PEX-SI from the X Consortium does not peform lighting, shading,
 *  and some other rendering operations, such as HLHSR.  This program,
 *  then, will not be very helpful when used with that code.
 *
 *  This program uses the Motif toolkit.
 */
	

/* light_tool.c 1.1 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/ToggleB.h>
#include <Xm/Protocols.h>
#include <Xm/Scale.h>
#include <Xm/Separator.h>
#include <Xm/Frame.h>
#include <phigs/phigs.h>
#include <math.h>

/* PHIGS Identifiers */
#define WS_ID		(Pint) 1
#define STRUCT_ID	(Pint) 1
#define ARCHIVE_ID	(Pint) 1
#define CUBE		(Pint) 101
#define CYLINDER	(Pint) 102
#define SPHERE		(Pint) 103
#define CUBE_QUAD	(Pint) 201

#define OBJECT_LABEL		(Pint) 1
#define LIGHT_STATE_LABEL	(Pint) 2
#define SHADING_METHOD_LABEL	(Pint) 3
#define REFL_PROPS_LABEL	(Pint) 4
#define REFL_EQN_LABEL		(Pint) 5
#define INTERIOR_STYLE_LABEL	(Pint) 6

/* Window layout information */
#define WS_WIDTH		400
#define WS_HEIGHT		400

#define NO_COLOR	(Pint) 0

/* Light source parameters */
#define RED_COORD	1
#define GREEN_COORD	2
#define BLUE_COORD	3
#define ATTENUATION_C1	4
#define ATTENUATION_C2	5
#define SPREAD_ANGLE	6
#define CONCENTRATION	7
#define AMBIENT_COEF	8
#define DIFFUSE_COEF	9
#define SPECULAR_COEF	10
#define SPECULAR_EXP	11

/* Light types */
#define AMBIENT		0
#define DIRECTIONAL	1
#define POSITIONAL	2
#define SPOT		3

static Widget	top, canvas, object_menu, lights_menu, shading_menu,
		refl_menu, depth_cue_menu, hlhsr_menu, cur_light_menu,
		light_red, light_blue, light_green, refl_panel, sep_1,
		scale_C1, scale_C2, sep_2, scale_spread, scale_conc,
		scale_Ka, scale_Kd, scale_Ks, scale_Sr, scale_Sg, scale_Sb,
		scale_Se;
static Atom	delete_window_atom;

static Pint			light_switches[4] = {1,1,1,1};
static Plight_src_bundle	lights[4];
static Pdcue_bundle		depth_cue_rep;
static Prefl_props		refl_props;
static Pint			cur_light = SPOT;
static Pint			cur_shading = PSD_COLOUR;
static Pint			cur_refl = PREFL_AMB_DIFF_SPEC;
static Pint			cur_hlhsr_mode = PHIGS_HLHSR_MODE_ZBUFF;
static Pint			cur_object = CUBE;
static Pint_style		cur_interior_style = PSTYLE_SOLID;
static Pstore			store;

/* The viewing parameters. */
static Ppoint3		view_ref_pt = {0.5,0.5,0.5};
static Pvec3		view_up_vec = {0,1,0};
static Pvec3		view_plane_normal = {1,1,1};
static Pview_map3	view_map;

static void initialize_light_table(),
	initialize_depth_cue_table(),
	initialize_view(),
	build_cube(), build_cylinder(), build_sphere();

static void create_appl_windows( int argc, char *argv[] );

#define SET_COLOR( colr, r, g, b ) \
	    colr.type = PMODEL_RGB,	\
	    colr.val.general.x = r,	\
	    colr.val.general.y = g,	\
	    colr.val.general.z = b	\

/* Callback function to quit the program. */
static void
quit( w, client_data, call_data )
    Widget			w;
    caddr_t			client_data;
    XmAnyCallbackStruct		*call_data;
{
    XClientMessageEvent		*cm = &call_data->event->xclient;

    if ( cm->data.l[0] == delete_window_atom ) {
	pclose_ws( 1 );
	pclose_phigs();
	XtDestroyWidget( top );
	exit( 0 );
    }
}

/* Callback function to redisplay the PHIGS output. */
static void
redraw( w, client_data, event )
    Widget	w;
    caddr_t	client_data;
    XEvent	*event;
{
    static int	first_time = 1;

    if ( first_time ) {
	/* The window is visible.  Open the workstation. */

	Pint			ws_type;
	Pconnid_x_drawable	conn_id;
	XSetWindowAttributes	win_attrs;

	/* Set the backing store attribute. */
	win_attrs.backing_store = NotUseful;
	XChangeWindowAttributes( XtDisplay(w), XtWindow(w),
	    CWBackingStore, &win_attrs );

	/* Open the workstation. */
	conn_id.display = XtDisplay(w);
	conn_id.drawable_id = XtWindow(w);
	ws_type = phigs_ws_type_create( phigs_ws_type_x_drawable,
	    PHIGS_X_BUF_MODE, PHIGS_BUF_DOUBLE,
	    NULL );
	popen_ws( WS_ID, (void *)&conn_id, ws_type );

	/* Initialize view 1. */
	initialize_view();

	/* Initialize light table and depth cueing table. */
	initialize_light_table();
	initialize_depth_cue_table();
	pset_hlhsr_mode( WS_ID, cur_hlhsr_mode );

	/* Set the update mode to update with each change. */
	pset_disp_upd_st( WS_ID, PDEFER_WAIT, PMODE_NIVE );

	/* Post the structure created earlier. */
	ppost_struct( WS_ID, STRUCT_ID, (Pfloat) 1.0 );
	pupd_ws( WS_ID, PFLAG_PERFORM );
	/* Remember that we've been called before. */
	first_time = 0;

    } else {
	/* Redraw the PHIGS workstation. */
	if ( event->xexpose.count == 0 ) /* last Expose event */
	    predraw_all_structs( WS_ID, PFLAG_ALWAYS );
    }
}

static void
set_object( widget, choice, cbs )
    Widget	widget;
    int		choice;
{
    Pint	object;

    XSync( XtDisplay(top), False );
    switch ( choice ) {
	case 0: object = CUBE; break;
	case 1: object = CYLINDER; break;
	case 2: object = SPHERE; break;
    }

    if ( object != cur_object ) {
	cur_object = object;
	pset_edit_mode( PEDIT_REPLACE );
	popen_struct( STRUCT_ID );
	    pset_elem_ptr( (Pint) 0 );
	    pset_elem_ptr_label( OBJECT_LABEL );
	    poffset_elem_ptr( (Pint) 1 );
	    pexec_struct( cur_object );
	pclose_struct();
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_interior_style( widget, choice, cbs )
    Widget	widget;
    int		choice;
{
    Pint_style	interior_style;

    XSync( XtDisplay(top), False );
    switch ( choice ) {
	case 0: interior_style = PSTYLE_HOLLOW; break;
	case 1: interior_style = PSTYLE_SOLID; break;
	case 2: interior_style = PSTYLE_PAT; break;
	case 3: interior_style = PSTYLE_HATCH; break;
	case 4: interior_style = PSTYLE_EMPTY; break;
    }

    if ( interior_style != cur_interior_style ) {
	cur_interior_style = interior_style;
	pset_edit_mode( PEDIT_REPLACE );
	popen_struct( STRUCT_ID );
	    pset_elem_ptr( (Pint) 0 );
	    pset_elem_ptr_label( INTERIOR_STYLE_LABEL );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_int_style( cur_interior_style );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_back_int_style( cur_interior_style );
	pclose_struct();
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_lights( widget, choice )
    Widget	widget;
    int		choice;
{
    Pint	i, on_off, state, light_sw[4];
    Pint_list	light_list, dummy;
    Widget	button;

    XSync( XtDisplay(top), False );
    if ( choice == 0 || choice == 5 ) {
	/* Set all lights. */
	if ( choice == 0 )
	    on_off = 1, state = True;
	else
	    on_off = 0, state = False;

	for ( i = 0; i < 4 ; i++ )
	    light_switches[i] = on_off;

	if ( button = XtNameToWidget( lights_menu, "button_1" ) )
	    XtVaSetValues( button, XmNset, state, NULL );
	if ( button = XtNameToWidget( lights_menu, "button_2" ) )
	    XtVaSetValues( button, XmNset, state, NULL );
	if ( button = XtNameToWidget( lights_menu, "button_3" ) )
	    XtVaSetValues( button, XmNset, state, NULL );
	if ( button = XtNameToWidget( lights_menu, "button_4" ) )
	    XtVaSetValues( button, XmNset, state, NULL );
    } else {
	light_switches[choice - 1] = XmToggleButtonGetState( widget );
    }

    light_list.num_ints = 0;
    light_list.ints = light_sw;
    for ( i = 0; i < 4; i++ ) {
	if ( light_switches[i] )
	    light_list.ints[light_list.num_ints++] = i + 1;
    }
    dummy.num_ints = 0;

    pset_edit_mode( PEDIT_REPLACE );
    popen_struct( STRUCT_ID );
	pset_elem_ptr( (Pint) 0 );
	pset_elem_ptr_label( LIGHT_STATE_LABEL );
	poffset_elem_ptr( (Pint) 1 );
	pset_light_src_state( &light_list, &dummy );
    pclose_struct();
    pupd_ws( WS_ID, PFLAG_PERFORM );
}

static void
set_shading( widget, choice )
    Widget	widget;
    int		choice;
{
    Pint	shading_method;

    XSync( XtDisplay(top), False );
    switch ( choice ) {
	case 0: shading_method = PSD_NONE; break;
	case 1: shading_method = PSD_COLOUR; break;
	case 2: shading_method = PSD_DOT_PRODUCT; break;
	case 3: shading_method = PSD_NORMAL; break;
    }

    if ( shading_method != cur_shading ) {
	cur_shading = shading_method;
	pset_edit_mode( PEDIT_REPLACE );
	popen_struct( STRUCT_ID );
	    pset_elem_ptr( (Pint) 0 );
	    pset_elem_ptr_label( SHADING_METHOD_LABEL );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_int_shad_meth( shading_method );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_back_int_shad_meth( shading_method );
	pclose_struct();
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_reflectance( widget, choice )
    Widget	widget;
    int		choice;
{
    Pint	refl;

    XSync( XtDisplay(top), False );
    switch ( choice ) {
	case 0: refl = PREFL_NONE; break;
	case 1: refl = PREFL_AMBIENT; break;
	case 2: refl = PREFL_AMB_DIFF; break;
	case 3: refl = PREFL_AMB_DIFF_SPEC; break;
    }

    if ( refl != cur_refl ) {
	cur_refl = refl;
	pset_edit_mode( PEDIT_REPLACE );
	popen_struct( STRUCT_ID );
	    pset_elem_ptr( (Pint) 0 );
	    pset_elem_ptr_label( REFL_EQN_LABEL );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_refl_eqn( refl );
	    poffset_elem_ptr( (Pint) 1 );
	    pset_back_refl_eqn( refl );
	pclose_struct();
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_depth_cue( widget, choice )
    Widget	widget;
    int		choice;
{
    Pdcue_mode		mode;

    XSync( XtDisplay(top), False );
    mode = choice == 1 ? PALLOWED : PSUPPRESSED;
    if ( mode != depth_cue_rep.mode ) {
	depth_cue_rep.mode = mode;
	pset_dcue_rep( WS_ID, (Pint) 1, &depth_cue_rep );
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_hlhsr( widget, choice )
    Widget	widget;
    int		choice;
{
    int		mode;

    XSync( XtDisplay(top), False );
    mode = choice == 1 ? PHIGS_HLHSR_MODE_ZBUFF : PHIGS_HLHSR_MODE_NONE;
    if ( mode != cur_hlhsr_mode ) {
	cur_hlhsr_mode = mode;
	pset_hlhsr_mode( WS_ID, mode );
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}

static void
set_light_source( widget, parameter, cbs )
    Widget			widget;
    caddr_t			parameter;
    XmScaleCallbackStruct	*cbs;
{
    Pgcolr	*color;
    Pfloat	*coef;
    int		set_light = 1;

    switch ( cur_light ) {
	case AMBIENT:
	    color = &lights[AMBIENT].rec.ambient.colr;
	    break;
	case DIRECTIONAL:
	    color = &lights[DIRECTIONAL].rec.directional.colr;
	    break;
	case POSITIONAL:
	    color = &lights[POSITIONAL].rec.positional.colr;
	    coef = lights[POSITIONAL].rec.positional.coef;
	    break;
	case SPOT:
	    color = &lights[SPOT].rec.spot.colr;
	    coef = lights[SPOT].rec.spot.coef;
	    break;
    }

    switch ( (int)parameter ) {
	case RED_COORD:
	    color->val.general.x = 0.1 * cbs->value;
	    break;
	case GREEN_COORD:
	    color->val.general.y = 0.1 * cbs->value;
	    break;
	case BLUE_COORD:
	    color->val.general.z = 0.1 * cbs->value;
	    break;
	case ATTENUATION_C1:
	    if ( cur_light == POSITIONAL || cur_light == SPOT )
		coef[0] = 0.1 * cbs->value;
	    else
		set_light = 0;
	    break;
	case ATTENUATION_C2:
	    if ( cur_light == POSITIONAL || cur_light == SPOT )
		coef[1] = 0.1 * cbs->value;
	    else
		set_light = 0;
	    break;
	case SPREAD_ANGLE:
	    if ( cur_light == SPOT )
		lights[SPOT].rec.spot.angle = cbs->value * M_PI/180;
	    else
		set_light = 0;
	    break;
	case CONCENTRATION:
	    if ( cur_light == SPOT )
		lights[SPOT].rec.spot.exp = cbs->value;
	    else
		set_light = 0;
	    break;
    }

    if ( set_light ) {
	pset_light_src_rep( WS_ID, cur_light + 1, &lights[cur_light] );
	pupd_ws( WS_ID, PFLAG_PERFORM );
    }
}
static void
set_refl_props( widget, parameter, cbs )
    Widget			widget;
    caddr_t			parameter;
    XmScaleCallbackStruct	*cbs;
{
    switch ( (int)parameter ) {
	case RED_COORD:
	    refl_props.specular_colr.val.general.x = 0.1 * cbs->value;
	    break;
	case GREEN_COORD:
	    refl_props.specular_colr.val.general.y = 0.1 * cbs->value;
	    break;
	case BLUE_COORD:
	    refl_props.specular_colr.val.general.z = 0.1 * cbs->value;
	    break;
	case AMBIENT_COEF:
	    refl_props.ambient_coef = 0.1 * cbs->value;
	    break;
	case DIFFUSE_COEF:
	    refl_props.diffuse_coef = 0.1 * cbs->value;
	    break;
	case SPECULAR_COEF:
	    refl_props.specular_coef = 0.1 * cbs->value;
	    break;
	case SPECULAR_EXP:
	    refl_props.specular_exp = cbs->value;
	    break;
    }

    pset_edit_mode( PEDIT_REPLACE );
    popen_struct( STRUCT_ID );
	pset_elem_ptr( (Pint) 0 );
	pset_elem_ptr_label( REFL_PROPS_LABEL );
	poffset_elem_ptr( (Pint) 1 );
	pset_refl_props( &refl_props );
	poffset_elem_ptr( (Pint) 1 );
	pset_back_refl_props( &refl_props );
    pclose_struct();
    pupd_ws( WS_ID, PFLAG_PERFORM );
}

static void
select_cur_light( widget, choice )
    Widget	widget;
    int		choice;
{
    Pgcolr	*color;

    switch ( choice ) {
	case 0:
	    cur_light = AMBIENT;
	    XtUnmanageChild( scale_C1 );
	    XtUnmanageChild( scale_C2 );
	    XtUnmanageChild( sep_2 );
	    XtUnmanageChild( scale_spread );
	    XtUnmanageChild( scale_conc );
	    color = &lights[AMBIENT].rec.ambient.colr;
	    break;
	case 1:
	    cur_light = DIRECTIONAL;
	    XtUnmanageChild( scale_C1 );
	    XtUnmanageChild( scale_C2 );
	    XtUnmanageChild( sep_2 );
	    XtUnmanageChild( scale_spread );
	    XtUnmanageChild( scale_conc );
	    color = &lights[DIRECTIONAL].rec.directional.colr;
	    break;
	case 2:
	    cur_light = POSITIONAL;
	    XmScaleSetValue( scale_C1, (int)
		(10 * lights[POSITIONAL].rec.positional.coef[0]) );
	    XmScaleSetValue( scale_C2, (int)
		(10 * lights[POSITIONAL].rec.positional.coef[1]) );
	    color = &lights[POSITIONAL].rec.positional.colr;
	    XtManageChild( scale_C1 );
	    XtManageChild( scale_C2 );
	    XtManageChild( sep_2 );
	    XtUnmanageChild( scale_spread );
	    XtUnmanageChild( scale_conc );
	    break;
	case 3:
	    cur_light = SPOT;
	    XmScaleSetValue( scale_C1, (int)
		(10 * lights[SPOT].rec.spot.coef[0]) );
	    XmScaleSetValue( scale_C2, (int)
		(10 * lights[SPOT].rec.spot.coef[1]) );
	    XmScaleSetValue( scale_spread, (int)
		((180/M_PI) * lights[SPOT].rec.spot.angle) );
	    XmScaleSetValue( scale_conc,
		(int)lights[SPOT].rec.spot.exp );
	    color = &lights[SPOT].rec.spot.colr;
	    XtManageChild( scale_C1 );
	    XtManageChild( scale_C2 );
	    XtManageChild( sep_2 );
	    XtManageChild( scale_spread );
	    XtManageChild( scale_conc );
	    break;
    }

    /* Have the sliders show the current light color. */
    XmScaleSetValue( light_red, (int)(10 * color->val.general.x) );
    XmScaleSetValue( light_green, (int)(10 * color->val.general.y) );
    XmScaleSetValue( light_blue, (int)(10 * color->val.general.z) );
}

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Pint		list[4];
    Pint_list		lights_on, lights_off;
    Pgcolr		color;
    Pint		error;
    Pxphigs_info	xphigs_info;
    unsigned long	mask;

    create_appl_windows( argc, argv );

    /* Open PHIGS, telling it not to monitor window events. */
    mask = PXPHIGS_INFO_FLAGS_NO_MON;
    xphigs_info.flags.no_monitor = 1;
    popen_xphigs( PDEF_ERR_FILE, PDEF_MEM_SIZE, mask, &xphigs_info );

    pcreate_store( &error, &store );
    if ( error ) {
	fprintf( stderr, "Error %d creating store.\n", error );
	exit( 1 );
    }

    build_cube();
    build_cylinder();
    build_sphere();

    popen_struct( STRUCT_ID );
	pset_view_ind( (Pint) 1 );
	pset_face_disting_mode( PDISTING_YES );
	plabel( INTERIOR_STYLE_LABEL );
	pset_int_style( cur_interior_style );
	pset_back_int_style( cur_interior_style );
	SET_COLOR( color, 1, 0, 0.5 );
	pset_int_colr( &color );
	SET_COLOR( color, 0.5, 0.5, 0.5 );
	pset_back_int_colr( &color );

	lights_on.num_ints = 4; lights_on.ints = list;
	list[0] = 1; list[1] = 2; list[2] = 3; list[3] = 4;
	lights_off.num_ints = 0;
	plabel( LIGHT_STATE_LABEL );
	pset_light_src_state( &lights_on, &lights_off );

	plabel( REFL_EQN_LABEL );
	pset_refl_eqn( cur_refl );
	pset_back_refl_eqn( cur_refl );

	refl_props.ambient_coef = 0.4;
	XmNorientation, XmHORIZONTAL,
	refl_props.diffuse_coef = 0.4;
	refl_props.specular_coef = 0.5;
	refl_props.specular_exp = 3.0;
	SET_COLOR( refl_props.specular_colr, 1, 1, 1 );
	plabel( REFL_PROPS_LABEL );
	pset_refl_props( &refl_props );
	pset_back_refl_props( &refl_props );

	plabel( SHADING_METHOD_LABEL );
	pset_int_shad_meth( cur_shading );
	pset_back_int_shad_meth( cur_shading );

	pset_dcue_ind( (Pint) 1 );
	pset_hlhsr_id( PHIGS_HLHSR_ID_ON );

	plabel( OBJECT_LABEL );
	pexec_struct( cur_object );
    pclose_struct();

    XtMainLoop();
}

static void
initialize_light_table()
{
    lights[AMBIENT].type = PLIGHT_AMBIENT;
    SET_COLOR( lights[AMBIENT].rec.ambient.colr, 1, 1, 1);
    pset_light_src_rep( WS_ID, (Pint) 1, &lights[AMBIENT] );

    lights[DIRECTIONAL].type = PLIGHT_DIRECTIONAL;
    lights[DIRECTIONAL].rec.directional.dir.delta_x = -1;
    lights[DIRECTIONAL].rec.directional.dir.delta_y =  0;
    lights[DIRECTIONAL].rec.directional.dir.delta_z =  0;
    SET_COLOR( lights[DIRECTIONAL].rec.directional.colr, 1, 1, 1);
    pset_light_src_rep( WS_ID, (Pint) 2, &lights[DIRECTIONAL] );

    lights[POSITIONAL].type = PLIGHT_POSITIONAL;
    lights[POSITIONAL].rec.positional.pos.x = 0.5;
    lights[POSITIONAL].rec.positional.pos.y = 2.0;
    lights[POSITIONAL].rec.positional.pos.z = 0.5;
    lights[POSITIONAL].rec.positional.coef[0] = 0;
    lights[POSITIONAL].rec.positional.coef[1] = 1;
    SET_COLOR( lights[POSITIONAL].rec.positional.colr, 1, 1, 1);
    pset_light_src_rep( WS_ID, (Pint) 3, &lights[POSITIONAL] );

    lights[SPOT].type = PLIGHT_SPOT;
    lights[SPOT].rec.spot.pos.x = 0.5;
    lights[SPOT].rec.spot.pos.y = 2.0;
    lights[SPOT].rec.spot.pos.z = 0.5;
    lights[SPOT].rec.spot.dir.delta_x = 0;
    lights[SPOT].rec.spot.dir.delta_y = -1;
    lights[SPOT].rec.spot.dir.delta_z = 0;
    lights[SPOT].rec.spot.coef[0] = 0;
    lights[SPOT].rec.spot.coef[1] = 1;
    lights[SPOT].rec.spot.exp = 2;
    lights[SPOT].rec.spot.angle = M_PI/2;
    SET_COLOR( lights[SPOT].rec.spot.colr, 1, 1, 1);
    pset_light_src_rep( WS_ID, (Pint) 4, &lights[SPOT] );
}

static void
initialize_depth_cue_table()
{
    depth_cue_rep.mode = PSUPPRESSED;
    depth_cue_rep.ref_planes[0] = 1;
    depth_cue_rep.ref_planes[0] = 0;
    depth_cue_rep.scaling[1] = 1;
    depth_cue_rep.scaling[1] = 0.5;
    SET_COLOR( depth_cue_rep.colr, 0, 0, 0);
    pset_dcue_rep( WS_ID, (Pint) 1, &depth_cue_rep );
}

static void
initialize_view()
{
    Pview_rep3		view_rep;
    Pint		error;

    /* Compute the view orientation matrix. */
    peval_view_ori_matrix3( &view_ref_pt, &view_plane_normal,
	&view_up_vec, &error, view_rep.ori_matrix );

    /* Set the view mapping parameters. */
    view_map.proj_type = PTYPE_PERSPECT;
    view_map.proj_ref_point.x = 0;
    view_map.proj_ref_point.y = 0;
    view_map.proj_ref_point.z = 10.0;
    view_map.win.x_min = -1.5; view_map.win.x_max = 1.5;
    view_map.win.y_min = -1.5; view_map.win.y_max = 1.5;
    view_map.back_plane = -1.5; view_map.front_plane = 1.5;
    view_map.view_plane = 0.0;
    view_map.proj_vp.x_min = 0.0; view_map.proj_vp.x_max = 1.0;
    view_map.proj_vp.y_min = 0.0; view_map.proj_vp.y_max = 1.0;
    view_map.proj_vp.z_min = 0.0; view_map.proj_vp.z_max = 1.0;

    /* Compute the view mapping matrix. */
    peval_view_map_matrix3( &view_map, &error, view_rep.map_matrix );

    /* Set the view representation. */
    view_rep.xy_clip = PIND_CLIP;
    view_rep.front_clip = PIND_CLIP;
    view_rep.back_clip = PIND_CLIP;
    view_rep.clip_limit = view_map.proj_vp;
    pset_view_rep3( WS_ID, (Pint) 1, &view_rep );
}

#define PAN_SCALE	(M_PI/500)
#define PAN_OVER_SCALE	2.0/400

static void
pan_3D( ws_id, index, event, vrp, vpn, vup )
    Pint	ws_id;
    Pint	index;
    XEvent	*event;
    Ppoint3	*vrp;
    Pvec3	*vpn;
    Pvec3	*vup;
{
    int		i, j;
    Pint	error;
    Pupd_st	upd_st;
    Pview_rep3	cur_view, req_view;
    Ppoint3	movement, shift;

    static int		lastx, lasty;
    static float	theta, phi;
    static Pmatrix3	inv_ori;

    switch ( event->type ) {

	case ButtonPress:
	/* Initialize */
	lastx = event->xbutton.x;
	lasty = event->xbutton.y;

	/* Compute the current view plane normal angles. */
	theta = atan2( vpn->delta_x, vpn->delta_z );
	phi = atan2( vpn->delta_y, hypot( vpn->delta_x, vpn->delta_z) );
	break;

	case MotionNotify:
	/* Get the current view definition. */
	pinq_view_rep( ws_id, index, &error, &upd_st,
	    &cur_view, &req_view);
	if ( error ) return;

	if ( event->xbutton.state & ShiftMask ) {
	    /* Pan over the view plane.  See the text for a description
	     * of how the new view reference point is determined.
	     */
	    inv_ori[3][3] = 1;
	    for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
		    inv_ori[i][j] = req_view.ori_matrix[j][i];
	    movement.x = PAN_OVER_SCALE * (event->xmotion.x - lastx);
	    movement.y = PAN_OVER_SCALE * (lasty - event->xmotion.y);
	    movement.z = 0;
	    ptran_point3( &movement, inv_ori, &error, &shift );
	    vrp->x += shift.x; vrp->y += shift.y; vrp->z += shift.z;
		
	} else {
	    /* Pan around by moving the view plane normal. */
	    /* Increment the longitude and latitude. */
	    theta += PAN_SCALE * (event->xmotion.x - lastx);
	    phi += PAN_SCALE * (lasty - event->xmotion.y);

	    /* Reset the view plane normal. */
	    vpn->delta_x = cos( phi ) * sin( theta );
	    vpn->delta_y = sin( phi );
	    vpn->delta_z = cos( phi ) * cos( theta );
	}

	/* Calculate the new orientation matrix. */
	peval_view_ori_matrix3( vrp, vpn, vup, &error,
	    req_view.ori_matrix );
	if ( error ) return;

	/* Set the view. */
	pset_view_rep3( WS_ID, (Pint) 1, &req_view );

	lastx = event->xmotion.x;
	lasty = event->xmotion.y;
	break;
    }
}

static void
zoom_3D( ws_id, index, zoom_factor, event, view_map )
    Pint	ws_id;
    Pint	index;
    float	zoom_factor;
    XEvent	*event;
    Pview_map3	*view_map;
{
    Pint	error;
    Pupd_st	upd_st;
    Pview_rep3	cur_view, req_view;
    float	vw_width, vw_height;

    /* Compute the new view window. */
    vw_width = view_map->win.x_max - view_map->win.x_min;
    vw_height = view_map->win.y_max - view_map->win.y_min;
    zoom_factor *= 0.5; /* divide the change evenly between sides. */
    if ( event->xbutton.state & ShiftMask ) {
	/* Zoom out. */
	view_map->win.x_min -= zoom_factor * vw_width;
	view_map->win.x_max += zoom_factor * vw_width;
	view_map->win.y_min -= zoom_factor * vw_height;
	view_map->win.y_max += zoom_factor * vw_height;
    } else {
	/* Zoom in. */
	view_map->win.x_min += zoom_factor * vw_width;
	view_map->win.x_max -= zoom_factor * vw_width;
	view_map->win.y_min += zoom_factor * vw_height;
	view_map->win.y_max -= zoom_factor * vw_height;
    }

    /* Get the current view definition. */
    pinq_view_rep( ws_id, index, &error, &upd_st,
	&cur_view, &req_view);
    if ( error ) return;

    /* Calculate the new mapping matrix. */
    peval_view_map_matrix3( view_map, &error, req_view.map_matrix );
    if ( error ) return;

    /* Set the view.  */
    pset_view_rep3( WS_ID, (Pint) 1, &req_view );
}

static void
handle_input( w, client_data, event )
    Widget	w;
    caddr_t	client_data;
    XEvent	*event;
{
    if ( event->type == ButtonPress ) {
	switch ( event->xbutton.button ) {
	    case Button1:
		pan_3D( WS_ID, (Pint) 1, event, &view_ref_pt,
		    &view_plane_normal, &view_up_vec );
		break;
	    case Button2:
		zoom_3D( WS_ID, (Pint) 1, 0.1, event, &view_map );
		break;
	}

    } else if ( event->type == MotionNotify ) {
	pan_3D( WS_ID, (Pint) 1, event, &view_ref_pt,
	    &view_plane_normal, &view_up_vec );
    }

    pupd_ws( WS_ID, PFLAG_PERFORM );
}

#define FREE_STRINGS( _s, _n ) \
    {int i = (_n); while (i--) XmStringFree( (_s)[i] );}

static void
create_appl_windows( argc, argv )
    int		argc;
    char	*argv[];	
{
    Widget	form, bar, button, panel, scale, sep, refl_label, frame;
    Window	xid;
    XmString	strings[10];

    /* Initialize the toolkit and create the container for the PHIGS
     * window and control panel.
     */
    top = XtInitialize( argv[0], NULL, NULL, 0, &argc, argv );
    form = XtVaCreateManagedWidget( NULL, xmFormWidgetClass, top,
	NULL );
    
    strings[0] = XmStringCreateSimple( "Object" );
    strings[1] = XmStringCreateSimple( "Style" );
    strings[2] = XmStringCreateSimple( "Lights" );
    strings[3] = XmStringCreateSimple( "Shading" );
    strings[4] = XmStringCreateSimple( "Reflectance" );
    strings[5] = XmStringCreateSimple( "Depth-cue" );
    strings[6] = XmStringCreateSimple( "Hlhsr" );
    bar = XmVaCreateSimpleMenuBar( form, "menubar",
	XmVaCASCADEBUTTON, strings[0], 0,
	XmVaCASCADEBUTTON, strings[1], 0,
	XmVaCASCADEBUTTON, strings[2], 0,
	XmVaCASCADEBUTTON, strings[3], 0,
	XmVaCASCADEBUTTON, strings[4], 0,
	XmVaCASCADEBUTTON, strings[5], 0,
	XmVaCASCADEBUTTON, strings[6], 0,
	XtNwidth, WS_WIDTH,
	XmNrightAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	NULL );
    FREE_STRINGS( strings, 7 )

    strings[0] = XmStringCreateSimple( "cube" );
    strings[1] = XmStringCreateSimple( "cylinder" );
    strings[2] = XmStringCreateSimple( "sphere" );
    object_menu = XmVaCreateSimplePulldownMenu( bar, "object_menu",
	0, set_object,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[2], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 3 )
    if ( button = XtNameToWidget( object_menu, "button_0" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "hollow" );
    strings[1] = XmStringCreateSimple( "solid" );
    strings[2] = XmStringCreateSimple( "pattern" );
    strings[3] = XmStringCreateSimple( "hatch" );
    strings[4] = XmStringCreateSimple( "empty" );
    object_menu = XmVaCreateSimplePulldownMenu( bar, "style_menu",
	1, set_interior_style,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[2], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[3], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[4], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 5 )
    if ( button = XtNameToWidget( object_menu, "button_1" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "all on" );
    strings[1] = XmStringCreateSimple( "ambient" );
    strings[2] = XmStringCreateSimple( "directional" );
    strings[3] = XmStringCreateSimple( "positional" );
    strings[4] = XmStringCreateSimple( "spot" );
    strings[5] = XmStringCreateSimple( "all off" );
    lights_menu = XmVaCreateSimplePulldownMenu( bar, "lights_menu",
	2, set_lights,
	XmVaPUSHBUTTON, strings[0], 0, NULL, NULL,
	XmVaSEPARATOR,
	XmVaCHECKBUTTON, strings[1], 0, NULL, NULL,
	XmVaCHECKBUTTON, strings[2], 0, NULL, NULL,
	XmVaCHECKBUTTON, strings[3], 0, NULL, NULL,
	XmVaCHECKBUTTON, strings[4], 0, NULL, NULL,
	XmVaSEPARATOR,
	XmVaPUSHBUTTON, strings[5], 0, NULL, NULL,
	NULL );
    FREE_STRINGS( strings, 6 )
    if ( button = XtNameToWidget( lights_menu, "button_1" ) )
	XtVaSetValues( button, XmNset, True, NULL );
    if ( button = XtNameToWidget( lights_menu, "button_2" ) )
	XtVaSetValues( button, XmNset, True, NULL );
    if ( button = XtNameToWidget( lights_menu, "button_3" ) )
	XtVaSetValues( button, XmNset, True, NULL );
    if ( button = XtNameToWidget( lights_menu, "button_4" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "none" );
    strings[1] = XmStringCreateSimple( "color" );
    strings[2] = XmStringCreateSimple( "dot" );
    strings[3] = XmStringCreateSimple( "normal" );
    shading_menu = XmVaCreateSimplePulldownMenu( bar, "shading_menu",
	3, set_shading,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[2], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[3], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 4 )
    if ( button = XtNameToWidget( shading_menu, "button_1" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "none" );
    strings[1] = XmStringCreateSimple( "ambient" );
    strings[2] = XmStringCreateSimple( "amb-diff" );
    strings[3] = XmStringCreateSimple( "amb-diff-spec" );
    refl_menu = XmVaCreateSimplePulldownMenu( bar, "reflectance_menu",
	4, set_reflectance,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[2], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[3], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 4 )
    if ( button = XtNameToWidget( refl_menu, "button_3" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "suppressed" );
    strings[1] = XmStringCreateSimple( "allowed" );
    depth_cue_menu = XmVaCreateSimplePulldownMenu( bar, "depth_cue_menu",
	5, set_depth_cue,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 2 )
    if ( button = XtNameToWidget( depth_cue_menu, "button_0" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    strings[0] = XmStringCreateSimple( "off" );
    strings[1] = XmStringCreateSimple( "on" );
    hlhsr_menu = XmVaCreateSimplePulldownMenu( bar, "hlhsr_menu",
	6, set_hlhsr,
	XmVaRADIOBUTTON, strings[0], 0, NULL, NULL,
	XmVaRADIOBUTTON, strings[1], 0, NULL, NULL,
	XmNradioBehavior, True,
	XmNradioAlwaysOne, True,
	NULL );
    FREE_STRINGS( strings, 2 )
    if ( button = XtNameToWidget( hlhsr_menu, "button_1" ) )
	XtVaSetValues( button, XmNset, True, NULL );

    canvas = XtVaCreateManagedWidget( "canvas",
	xmDrawingAreaWidgetClass, form, 
	XtNwidth, WS_WIDTH, XtNheight, WS_HEIGHT,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, bar,
	XmNleftAttachment, XmATTACH_FORM,
	NULL );

    /* Register an event handler for Expose events. */
    XtAddEventHandler( canvas, ExposureMask, FALSE, redraw, NULL );
    XtAddEventHandler( canvas, ButtonPressMask, FALSE, handle_input, NULL );
    XtAddEventHandler( canvas, Button1MotionMask, FALSE, handle_input, NULL );

    frame = XtVaCreateManagedWidget( "frame", xmFrameWidgetClass, form, 
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, bar,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, canvas,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL );

    panel = XtVaCreateManagedWidget( "panel",
	xmRowColumnWidgetClass, frame, 
	NULL );

    strings[0] = XmStringCreateSimple( "Lights:" );
    strings[1] = XmStringCreateSimple( "ambient" );
    strings[2] = XmStringCreateSimple( "directional" );
    strings[3] = XmStringCreateSimple( "positional" );
    strings[4] = XmStringCreateSimple( "spot" );
    cur_light_menu = XmVaCreateSimpleOptionMenu( panel, "light_control",
	strings[0], 0, 3, select_cur_light,
	XmVaPUSHBUTTON, strings[1], 0, NULL, NULL,
	XmVaPUSHBUTTON, strings[2], 0, NULL, NULL,
	XmVaPUSHBUTTON, strings[3], 0, NULL, NULL,
	XmVaPUSHBUTTON, strings[4], 0, NULL, NULL,
	NULL );
    FREE_STRINGS( strings, 5 )

    light_red = XtVaCreateManagedWidget( "red", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "Red", 3,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( light_red, XmNvalueChangedCallback,
	set_light_source, RED_COORD );
    light_green = XtVaCreateManagedWidget( "green", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "Green", 5,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( light_green, XmNvalueChangedCallback,
	set_light_source, GREEN_COORD );
    light_blue = XtVaCreateManagedWidget( "blue", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "Blue", 4,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( light_blue, XmNvalueChangedCallback,
	set_light_source, BLUE_COORD );

    sep_1 = XtVaCreateManagedWidget( "separator", xmSeparatorWidgetClass,
	panel, XmNorientation, XmHORIZONTAL,
	NULL );

    scale_C1 = XtVaCreateManagedWidget( "C1", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "C1 Attenuation", 14,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 0,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL, XmNhighlightOnEnter, True,
	NULL );
    XtAddCallback( scale_C1, XmNvalueChangedCallback,
	set_light_source, ATTENUATION_C1 );
    scale_C2 = XtVaCreateManagedWidget( "C2", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "C2 Attenuation", 14,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( scale_C2, XmNvalueChangedCallback,
	set_light_source, ATTENUATION_C2 );

    sep_2 = XtVaCreateManagedWidget( "separator", xmSeparatorWidgetClass,
	panel, XmNorientation, XmHORIZONTAL,
	NULL );

    scale_spread = XtVaCreateManagedWidget( "spread", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "Spread Angle", 12,
	XmNmaximum, 90, XmNminimum, 0, XmNvalue, 90,
	XmNdecimalPoints, 0, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( scale_spread, XmNvalueChangedCallback,
	set_light_source, SPREAD_ANGLE );

    scale_conc = XtVaCreateManagedWidget( "concentration", xmScaleWidgetClass,
	panel, XtVaTypedArg, XmNtitleString, XmRString, "Concentration", 13,
	XmNmaximum, 200, XmNminimum, 0, XmNvalue, 2,
	XmNdecimalPoints, 0, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	NULL );
    XtAddCallback( scale_conc, XmNvalueChangedCallback,
	set_light_source, CONCENTRATION );


    /* The reflectance properties panel. */
    frame = XtVaCreateManagedWidget( "frame", xmFrameWidgetClass, form, 
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, canvas,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET, XmNrightWidget, canvas,
	XmNbottomAttachment, XmATTACH_FORM,
	NULL );

    refl_panel = XtVaCreateManagedWidget( "refl_panel",
	xmFormWidgetClass, frame, 
	XmNorientation, XmVERTICAL,
	NULL );

    refl_label = XtVaCreateManagedWidget( "Reflectance Properties",
	xmLabelWidgetClass, refl_panel,
	XmNtopAttachment, XmATTACH_FORM,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	NULL );
    scale_Ka = XtVaCreateManagedWidget( "Ka", xmScaleWidgetClass, refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Ambient Coef", 12,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 4,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, refl_label,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Ka, XmNvalueChangedCallback,
	set_refl_props, AMBIENT_COEF );
    scale_Kd = XtVaCreateManagedWidget( "Kd", xmScaleWidgetClass, refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Diffuse Coef", 12,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 4,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, scale_Ka,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Kd, XmNvalueChangedCallback,
	set_refl_props, DIFFUSE_COEF );
    scale_Ks = XtVaCreateManagedWidget( "Ks", xmScaleWidgetClass, refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Specular Coef", 13,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 5,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, scale_Kd,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Ks, XmNvalueChangedCallback,
	set_refl_props, SPECULAR_COEF );

    sep = XtVaCreateManagedWidget( "separator", xmSeparatorWidgetClass,
	refl_panel, XmNorientation, XmVERTICAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, scale_Ka,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, refl_label,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );

    scale_Sr = XtVaCreateManagedWidget( "r_red", xmScaleWidgetClass, refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Specular Red", 12,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, sep,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, refl_label,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Sr, XmNvalueChangedCallback,
	set_refl_props, RED_COORD );
    scale_Sg = XtVaCreateManagedWidget( "r_green", xmScaleWidgetClass,
	refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Specular Green", 14,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, sep,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, scale_Sr,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Sg, XmNvalueChangedCallback,
	set_refl_props, GREEN_COORD );
    scale_Sb = XtVaCreateManagedWidget( "r_blue", xmScaleWidgetClass,
	refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Specular Blue", 13,
	XmNmaximum, 10, XmNminimum, 0, XmNvalue, 10,
	XmNdecimalPoints, 1, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, sep,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, scale_Sg,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Sb, XmNvalueChangedCallback,
	set_refl_props, BLUE_COORD );

    sep = XtVaCreateManagedWidget( "separator", xmSeparatorWidgetClass,
	refl_panel, XmNorientation, XmVERTICAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, scale_Sr,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, refl_label,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );

    scale_Se = XtVaCreateManagedWidget( "spec_exp", xmScaleWidgetClass,
	refl_panel,
	XtVaTypedArg, XmNtitleString, XmRString, "Specular Exponent", 17,
	XmNmaximum, 100, XmNminimum, 0, XmNvalue, 3,
	XmNdecimalPoints, 0, XmNshowValue, True,
	XmNorientation, XmHORIZONTAL,
	XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, sep,
	XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, refl_label,
	XmNtopOffset, 10, XmNbottomOffset, 10,
	XmNleftOffset, 10, XmNrightOffset, 10,
	NULL );
    XtAddCallback( scale_Se, XmNvalueChangedCallback,
	set_refl_props, SPECULAR_EXP );


    /* Manage and realize the widgets. */
    XtManageChild( bar );
    XtManageChild( cur_light_menu );
    XtRealizeWidget( top );

    /* Set up to use window manager DELETE_WINDOW protocol. */
    delete_window_atom = XInternAtom( XtDisplay(top),
	"WM_DELETE_WINDOW", TRUE );
    XmAddWMProtocols( top, &delete_window_atom, 1 );
    XmSetWMProtocolHooks( top, delete_window_atom,
	NULL, NULL, quit, NULL );

    /* Tell the window manager to keep track of the colormap. */
    xid = XtWindow( canvas );
    XSetWMColormapWindows( XtDisplay(top), XtWindow(top), &xid, 1 );
}

#define SET_VERTEX_COORD( v, _x, _y, _z ) \
    (v).x = (_x), (v).y = (_y), (v).z = (_z)

#define NORMALIZE( _v ) \
    {double	mag; \
     mag = 1.0 / sqrt((_v).delta_x * (_v).delta_x \
	 + (_v).delta_y * (_v).delta_y \
	 + (_v).delta_z * (_v).delta_z); \
    (_v).delta_x *= mag, (_v).delta_y *= mag, (_v).delta_z *= mag;}

#define NUM_FACETS	16
#define NUM_POINTS	(NUM_FACETS + 1)
#define RADIUS		0.5

static void
build_cylinder()
{
    Pptnorm3		vertex_data[2 * NUM_POINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    double		theta = 0;
    int			i;

    /* Compute the top row of points. */
    for ( i = 0; i < NUM_FACETS; i++ ) {
	vertex_data[i].point.x = RADIUS * (1 + cos( theta ));
	vertex_data[i].point.z = RADIUS * (1 + sin( theta ));
	vertex_data[i].point.y = 1;
	vertex_data[i].norm.delta_x = vertex_data[i].point.x;
	vertex_data[i].norm.delta_y = 0;
	vertex_data[i].norm.delta_z = vertex_data[i].point.z;
	NORMALIZE(vertex_data[i].norm);
	theta += (2 * M_PI) / NUM_FACETS;
    }

    /* Close the cylinder. */
    vertex_data[NUM_FACETS].point = vertex_data[0].point;
    vertex_data[NUM_FACETS].norm = vertex_data[0].norm;

    /* Compute the bottom row of points. */
    for ( i = 0; i < NUM_POINTS; i++ ) {
	vertex_data[NUM_POINTS + i].point.x = vertex_data[i].point.x;
	vertex_data[NUM_POINTS + i].point.y = 0;
	vertex_data[NUM_POINTS + i].point.z = vertex_data[i].point.z;
	vertex_data[NUM_POINTS + i].norm = vertex_data[i].norm;
    }

    popen_struct( CYLINDER );
	vertices.ptnorms = vertex_data;
	array_dims.size_x = NUM_POINTS;
	array_dims.size_y = 2;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD_NORMAL, NO_COLOR,
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
    Pptnorm3		vertex_data[NUM_LONG_POINTS * NUM_LAT_POINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    double		theta = 0, phi = 0;
    int			i, j, index;

    for ( i = 0; i < NUM_LAT_POINTS; i++ ) {
	for ( j = 0; j < NUM_LONG_FACETS; j++ ) {
	    index = i * NUM_LONG_POINTS + j;
	    vertex_data[index].point.x = RADIUS * (1 + sin(phi) * cos(theta));
	    vertex_data[index].point.y = RADIUS * (1 + cos(phi));
	    vertex_data[index].point.z = RADIUS * (1 + sin(phi) * sin(theta));
	    vertex_data[index].norm.delta_x = vertex_data[index].point.x;
	    vertex_data[index].norm.delta_y = vertex_data[index].point.y;
	    vertex_data[index].norm.delta_z = vertex_data[index].point.z;
	    NORMALIZE(vertex_data[index].norm);
	    theta += (2 * M_PI) / NUM_LONG_FACETS;
	}
	phi += M_PI / NUM_LAT_FACETS;

	/* Close the circle. */
	index = i * NUM_LONG_POINTS + NUM_LONG_FACETS;
	vertex_data[index].point = vertex_data[index - NUM_LONG_FACETS].point;
	vertex_data[index].norm = vertex_data[index - NUM_LONG_FACETS].norm;
    }

    popen_struct( SPHERE );
	vertices.ptnorms = vertex_data;
	array_dims.size_x = NUM_LONG_POINTS;
	array_dims.size_y = NUM_LAT_POINTS;
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD_NORMAL, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertices );
    pclose_struct();
}

#define NUM_QPOINTS	10

static void
build_cube()
{
    Ppoint3		vertex_data[NUM_QPOINTS * NUM_QPOINTS];
    Pfacet_vdata_arr3	vertices;
    Pint_size		array_dims;
    Pint		error;
    Ppoint3		fixed_pt;
    Pvec3		trans, scale;
    Pmatrix3		xform;
    int			i, j, index;

    vertices.points = vertex_data;
    array_dims.size_x = NUM_QPOINTS;
    array_dims.size_y = NUM_QPOINTS;

    for ( i = 0; i < NUM_QPOINTS; i++ ) {
	for ( j = 0; j < NUM_QPOINTS; j++ ) {
	    index = i * NUM_QPOINTS + j;
	    vertex_data[index].x = j * (1.0/(NUM_QPOINTS-1));
#ifdef SunPHIGS2.0
	    vertex_data[index].y =  1.0 - i * (1.0/(NUM_QPOINTS-1));
#else
	    vertex_data[index].y =  i * (1.0/(NUM_QPOINTS-1));
#endif
	    vertex_data[index].z = 0;
	}
    }

    popen_struct( CUBE_QUAD );
	pquad_mesh3_data( PFACET_NONE, PVERT_COORD, NO_COLOR,
	    &array_dims, (Pfacet_data_arr3 *)NULL, &vertices );
    pclose_struct();

    fixed_pt.x = fixed_pt.y = 0.5; fixed_pt.z = 0;
    scale.delta_x = scale.delta_y = scale.delta_z = 1;

    popen_struct( CUBE );
	/* back */
	pexec_struct( CUBE_QUAD );

	/* front */
	trans.delta_x = trans.delta_y = 0; trans.delta_z = 1;
	pbuild_tran_matrix3( &fixed_pt, &trans, 0.0, M_PI, 0.0,
	    &scale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );
	pexec_struct( CUBE_QUAD );

	/* bottom */
	trans.delta_x = 0; trans.delta_y = -0.5; trans.delta_z = 0.5;
	pbuild_tran_matrix3( &fixed_pt, &trans, -M_PI/2, 0.0, 0.0,
	    &scale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );
	pexec_struct( CUBE_QUAD );

	/* top */
	trans.delta_x = 0; trans.delta_y = 0.5; trans.delta_z = 0.5;
	pbuild_tran_matrix3( &fixed_pt, &trans, M_PI/2, 0.0, 0.0,
	    &scale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );
	pexec_struct( CUBE_QUAD );

#ifdef CLOSE_CUBE
	/* left */
	trans.delta_x = -0.5; trans.delta_y = 0; trans.delta_z = 0.5;
	pbuild_tran_matrix3( &fixed_pt, &trans, 0.0, M_PI/2, 0.0,
	    &scale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );
	pexec_struct( CUBE_QUAD );

	/* right */
	trans.delta_x = 0.5; trans.delta_y = 0; trans.delta_z = 0.5;
	pbuild_tran_matrix3( &fixed_pt, &trans, 0.0, -M_PI/2, 0.0,
	    &scale, &error, xform );
	pset_local_tran3( xform, PTYPE_REPLACE );
	pexec_struct( CUBE_QUAD );
#endif

    pclose_struct();
}
