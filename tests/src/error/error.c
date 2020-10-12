/* error.c 1.2 */
/* Copyright 1992 O'Reilly and Associates, Inc.  Permission to use, copy,
 * and modify this program is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 */
#include <phigs/phigs.h>

/* The global error number and function number.*/
static Pint	cur_error = 0, cur_function;

static void
my_err_handler( error, function, error_file )
    Pint	error;
    Pint	function;
    char	*error_file;
{
    /* Store the current error and function number. */
    cur_error = error;
    cur_function = function;

    /* Check for errors from specific functions. */
    switch ( function ) {
	case Pfn_open_phigs:
	    exit( 1 );
    }

    /* Check for certain errors. */
    switch ( error ) {
	case PE_BAD_ERROR_FILE:
	    fprintf( stderr, "The error file cannot be opened.\n" );
	    break;
	case PE_MAX_WS:
	    fprintf( stderr, "Cannot open anymore workstations.\n" );
	    break;
	default:
	    /* Print the PHIGS error message. */
	    perr_log( error, function, error_file );
	    break;
    }
}

main()
{
    void	(*default_err_handler)();

    pset_err_hand( my_err_handler, &default_err_handler );

    popen_ws( 1, NULL, phigs_ws_type_x_tool );
    popen_phigs( "local_error_file", PDEF_MEM_SIZE );
    if ( cur_error != 0 )
	exit( 1 );

    pclose_phigs();
}
