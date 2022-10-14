#ifndef DAEDALOS_DE_INCLUDES_H
#define DAEDALOS_DE_INCLUDES_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "library_visibilities.h"


#define VERSION "0.0.2"

#define VERB_OFF    0
#define VERB_FATAL  1
#define VERB_ERROR  2
#define VERB_WARN   3
#define VERB_INFO   4
#define VERB_DEBUG  5
#define VERB_TRACE  6


int glob_veriable = 5;

#define dlprintf( lvl, fmt, ... ) { \
	if( lvl <= glob_veriable ) \
		printf( "[%d]%s:%u:" fmt "\n", lvl, __FUNCTION__, __LINE__, \
			##__VA_ARGS__ );   \
}


#define error_print( fmt, ...) { dlprintf( VERB_ERROR, fmt, ##__VA_ARGS__ ); }

#define return_error_print( ret, fmt, ...) { dlprintf( VERB_ERROR, fmt, ##__VA_ARGS__ ); return ret; }

#define xmalloc( destination, size, destination_type, error_return_value ) \
	if( ( destination = (destination_type)malloc(size) ) == NULL ) { \
		error_print("malloc() error\n"); \
		return error_return_value; \
	}

#define xcalloc( destination, size, destination_type, error_return_value ) \
	if( ( destination = (destination_type)calloc(1, size) ) == NULL ) { \
		error_print("malloc() error\n"); \
		return error_return_value; \
	}

/**
 * Malloc Wrappers
 */

#endif //DAEDALOS_DE_INCLUDES_H
