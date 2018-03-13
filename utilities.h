#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stddef.h>


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


// Error checking


// To be called in case of an error
// Prints s to stderr, perrors, kill's child processes,
// Finall exits the program
void program_err(const char * const s);

// Assert b, if false call err(s)
void assert(const int b, const char * const s);


// Memory allocators


// Malloc that takes in a count and size argument
// calls assert to ensure no malloc faliure
// Returns malloc(size*count), 
void * safe_malloc(size_t count, size_t size);


#endif /* utilities_h */
