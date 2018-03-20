/** @file */
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <stddef.h>


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


// Error checking


/// To be called in case of an error
/** Prints log s to the error file, perrors,
 *  then kills the process group */
void program_err(const char * const s);

/** assert b, if false call program_err(s) */
void ss_assert(const bool b, const char * const s);


// For logging


/// Logs the arguments as printf would to the log file
/** This function promises **NOTHING** on failure */
void ss_log_no_newline(const char * const format, ...);

/// Logs the arguments as printf would to the log file
/** Ends the printed line with a newline then flushes the buffer
 *  This function promises **NOTHING** on failure */
void ss_log(const char * const format, ...);

/// Logs the arguments as printf would to the error and log files
/** Ends the printed line with a newline then flushes the buffer
 *  This function promises **NOTHING** on failure */
void ss_log_error(const char * const format, ...);


#endif /* utilities_h */
