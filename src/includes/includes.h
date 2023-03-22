#ifndef DECOMPILER_INCLUDES_H
#define DECOMPILER_INCLUDES_H
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


extern int glob_variable;


#define dlprintf( lvl, fmt, ... ) { \
	if( lvl <= glob_variable ) \
		printf( fmt,  \
			##__VA_ARGS__ );   \
}


#define vbprintf(verbCF, fmt, ...) \
{ \
    if (verbCF) \
    { \
        printf(fmt, ##__VA_ARGS__); \
    } \
}

#define vbcall(verbCF, func, ...) \
{ \
    if (verbCF) \
    { \
        func(__VA_ARGS__); \
    } \
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

#endif //DECOMPILER_INCLUDES_H
