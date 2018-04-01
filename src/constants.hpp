/** @file */
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <string.h>


/*********************************************************/
/*                                                       */
/*					Libraries for drrun					 */ 
/*                                                       */
/*********************************************************/


#ifndef DYNAMORIO_CLIENT_SO
/// The .so of the shadow stack DynamoRIO client.
/** This should be defined by the cmake on compilation */
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


#ifndef PROGRAM_NAME
/// The name of the program
/** This should be defined by the cmake on compilation */
#define PROGRAM_NAME "PROGRAM_NAME_PLACEHOLDER"
#endif

#ifdef DEBUG_MODE 

/// The default log file for debug mode
/** This fill may be nullptr
 *  This should be defined by the cmake on compilation */
#define LOG_FILE "./log"
#else 

/// The default log file for non-debug mode
/** This fill may be nullptr
 *  This should be defined by the cmake on compilation */
#define LOG_FILE nullptr 
#endif

/// The default stdout
/**  This fill may be nullptr */
#define STDOUT_FILE stdout

/// The default error log file (important events are also noted here)
/**  This fill may be nullptr */
#define ERROR_FILE stderr


/** The flag that must be passed to invoke internal mode */
#define INTERNAL_MODE_FLAG "int"

/** The flag that must be passed to invoke external mode */
#define EXTERNAL_MODE_FLAG "ext"

/** Verify the modes are not equal */
static_assert( ! str_equal(INTERNAL_MODE_FLAG, EXTERNAL_MODE_FLAG),
				"iternal mode flag cannot equal external mode flag");

/// Wildcard address matching value
/** This address matches any address on the stack
 *  Since no call or ret will ever happen here, 
 *  during normal use, it is fine */
#define WILDCARD ( -1 )

#endif
