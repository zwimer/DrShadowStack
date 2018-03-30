/** @file */
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <stdio.h>


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


/** A general class for utilities */
class Utilities {
public:

	/// To be called in case of an error
	/** Prints log s to the error file, perrors,
	 *  then kills the process group */
	[[noreturn]] static void err(const char * const s);

	/** assert b, if false call program_err(s) */
	static void assert(const bool b, const char * const s);

	// For logging

	/// Logs the arguments as printf would to the log file
	/** Ends the printed line with a newline then flushes the buffer
	 *  If log_file is nullptr, this function does nothing
	 *  This function promises **NOTHING** on failure
	 *  This function only runs if VERBOSE is defined */
	static void (* const verbose_log) (const char * const format, ...);

	/// Logs the arguments as printf would to the log file
	/** Ends the printed line with a newline then flushes the buffer
	 *  If log_file is nullptr, this function does nothing
	 *  This function promises **NOTHING** on failure */
	static void log(const char * const format, ...);

	/// Prints the arguments as printf would to the stdout and log files
	/** Ends the printed line with a newline then flushes the buffer
	 *  If stdout_file is nullptr, this function does nothing
	 *  This function promises **NOTHING** on failure */
	static void message(const char * const format, ...);

	/// Logs the arguments as printf would to the error, and log files
	/** Ends the printed line with a newline then flushes the buffer
	 *  If either file is nullptr, that file is skipped
	 *  This function does NOT print to stdout_file
	 *  This function promises **NOTHING** on failure */
	static void log_error(const char * const format, ...);

private:

	/// The log file
	/** This must be defined before main() */
	static FILE * const log_file;

	/// The stdout file
	/** This must be defined before main() */
	static FILE * const stdout_file;

	/// The error file
	/** This must be defined before main() */
	static FILE * const error_file;
};


#endif /* utilities_h */
