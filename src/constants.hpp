/** @file */
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include <string.h>


/*********************************************************/
/*                                                       */
/*                  Libraries for drrun                  */
/*                                                       */
/*********************************************************/


/* clang-format off */

#ifndef DYNAMORIO_CLIENT_SO
	/** The .so of the shadow stack DynamoRIO client.
	 *  This should be defined by the cmake on compilation */
#	define DYNAMORIO_CLIENT_SO "/path/to/.so/"
#endif


/*********************************************************/
/*                                                       */
/*                       General                         */
/*                                                       */
/*********************************************************/


#ifndef PROGRAM_NAME
    /** The name of the program
     *  This should be defined by the cmake on compilation */
#	define PROGRAM_NAME "PROGRAM_NAME_PLACEHOLDER"
#endif

#ifdef DEBUG_MODE
#	ifndef LOG_FILE
        /** The default log file for debug mode
         *  This fill may be nullptr
         *  This should be defined by the cmake on compilation */
#		define LOG_FILE "LOG_FILE_PLACEHOLDER"
#	endif
#endif

/** The default stdout
 *   This fill may be nullptr */
#define STDOUT_FILE stdout

/** The default error log file (important events are also noted here)
 *   This fill may be nullptr */
#define ERROR_FILE stderr

/* clang-format on */

/** Wildcard address matching value
 *  This address matches any address on the stack
 *  Since no call or ret will ever happen here,
 *  during normal use, it is fine */
#define WILDCARD ( -1 )


#endif
