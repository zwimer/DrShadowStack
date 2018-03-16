#include "utilities.h"
#include "stack.hpp"
#include "group.h"

#include <stdlib.h>
#include <stdio.h>


/*********************************************************/
/*                                                       */
/*                		Error checking		             */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Prints s to stderr, perrors, kill's child processes,
// Finally exits the program
void program_err(const char * const s) {
    fprintf(stderr, "\nERROR: %s\n", s);
	perror("");
	terminate_group();
    exit(EXIT_FAILURE);
}

// Assert b, if false call err(s)
void assert(const int b, const char * const s) {
    if ( ! b ) {
		program_err(s);
	}
}


/*********************************************************/
/*                                                       */
/*               	Memory allocators		             */
/*                                                       */
/*********************************************************/


// Malloc that takes in a count and size argument
// calls assert to ensure no malloc faliure
// Returns malloc(size*count)
void * safe_malloc(size_t count, size_t size) {
	void * const ret = malloc( size * count );
	assert( ret != NULL, "malloc() failed" );
	return ret;
}
