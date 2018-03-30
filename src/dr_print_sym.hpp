/** @file */
#ifndef __DR_PRINT_SYM_HPP__
#define __DR_PRINT_SYM_HPP__

#include "dr_api.h"

// For clarity
using PrintFn = void (*) (const char * const format, ...);

/// Print symbol information for the what is located at addr
/** Prints out information via the pnt function which works like printf
 *  Prints out errors via the perr function which works like printf
 *  description is a description of what the address addr points to */
void print_sym( PrintFn pnt, PrintFn perr, const char * const description,
				const app_pc addr );

#endif
