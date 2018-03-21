/** @file */
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <stddef.h>


// Ingore the macro assert if it is defined
#undef assert


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


// A namespace for utilities
namespace Utilities {


	// Error checking

	/// To be called in case of an error
	/** Prints log s to the error file, perrors,
	 *  then kills the process group */
	void err(const char * const s);

	/** assert b, if false call program_err(s) */
	void assert(const bool b, const char * const s);


	// For logging

	/// Logs the arguments as printf would to the log file
	/** This function promises **NOTHING** on failure */
	void log_no_newline(const char * const format, ...);

	/// Prints the arguments as printf would to the STDOUT_FILE file
	/** Ends the printed line with a newline then flushes the buffer
	 *  If STDOUT_FILE is nullptr, this function does nothing
	 *  This function promises **NOTHING** on failure */
	void message(const char * const format, ...);

	/// Logs the arguments as printf would to the log file
	/** Ends the printed line with a newline then flushes the buffer
	 *  If LOG_FILE is nullptr, this function does nothing
	 *  This function promises **NOTHING** on failure */
	void log(const char * const format, ...);

	/// Logs the arguments as printf would to the error, log, and stdout files
	/** Ends the printed line with a newline then flushes the buffer
	 *  If any of STDOUT, LOG_FILE, ERROR_FILE are the same, only prints once
	 *  If either file is nullptr, that file is skipped
	 *  This function promises **NOTHING** on failure */
	void log_error(const char * const format, ...);

};


#endif /* utilities_h */
