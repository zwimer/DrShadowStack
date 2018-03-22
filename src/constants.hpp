/** @file */
#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__


/*********************************************************/
/*                                                       */
/*					Libraries for drrun					 */ 
/*                                                       */
/*********************************************************/


// The .so of the shadow stack DynamoRIO client.
// This should be defined by the cmake on compilation
// #define DYNAMORIO_CLIENT_SO "/path/to/.so/"


/*********************************************************/
/*                                                       */
/*						 General						 */ 
/*                                                       */
/*********************************************************/


// The name of the program
// This should be defined by the cmake on compilation
/* #define PROGRAM_NAME "DrShadowStack" */

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
