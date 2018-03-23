/** @file */
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include "string.h"


/*********************************************************/
/*                                                       */
/*					Libraries for drrun					 */ 
/*                                                       */
/*********************************************************/


// The .so of the shadow stack DynamoRIO client.
// This should be defined by the cmake on compilation
#ifndef DYNAMORIO_CLIENT_SO
#define DYNAMORIO_CLIENT_SO "/path/to/.so/"
#endif


/*********************************************************/
/*                                                       */
/*				Static assert helper functions			 */ 
/*                                                       */
/*********************************************************/


/// A compile time strcmp
/** Recursively checks if the first characters of the strings match */
static constexpr bool str_equal( const char * const s1, 	
								 const char * const s2 ) {
	return (s1[0] != s2[0]) ? false :
		(s1[0] == 0) ? true : str_equal(s1 + 1, s2 + 1);
}


/*********************************************************/
/*                                                       */
/*						 General						 */ 
/*                                                       */
/*********************************************************/


// The name of the program
// This should be defined by the cmake on compilation
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "SatisfyTheIDECompiler"
#endif

// The default log file. 
// This fill may be nullptr, but may not be NULL 
// This should be defined by the cmake on compilation
// #define LOG_FILE nullptr

/** The default stdout
 *  This fill may be nullptr, but may not be NULL */
#define STDOUT_FILE stdout

/** The default error log file (important events are also noted here)
 *  This fill may be nullptr, but may not be NULL */
#define ERROR_FILE stderr


/** The flag that must be passed to invoke internal mode */
#define INTERNAL_MODE_FLAG "int"

/** The flag that must be passed to invoke external mode */
#define EXTERNAL_MODE_FLAG "ext"

/** Verify the modes are not equal */
static_assert( ! str_equal(INTERNAL_MODE_FLAG, EXTERNAL_MODE_FLAG),
				"iternal mode flag cannot equal external mode flag");


/*********************************************************/
/*                                                       */
/*	  				   Due to the OS					 */
/*                                                       */
/*********************************************************/


/** If this is on a mac */
#ifdef __APPLE__

/** The max number of signals on linux allowed */
#define _NSIG 65
#endif


#endif
