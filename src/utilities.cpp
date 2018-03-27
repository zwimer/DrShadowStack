#include "utilities.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <set>

#include "constants.hpp"
#include "group.hpp"


/*********************************************************/
/*                                                       */
/*                		Error checking		             */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Error logs s, perrors, then kills the process group
[[ noreturn ]] void Utilities::err(const char * const s) {
	TerminateOnDestruction tod;
    log_error("%s\nMessage from strerror: %s", s, strerror(errno));
	Group::terminate(nullptr);
}

// assert b, if false call err(s)
void Utilities::assert(const bool b, const char * const s) {
    if ( ! b ) {
		err(s);
	}
}

/*********************************************************/
/*                                                       */
/*                		  Logging						 */
/*                                                       */
/*********************************************************/


// The helper that writes args in the format of format to f
// Ends the printed line with a newline then flushes the buffer
// This function promises NOTHING on failure
inline static void write_log(FILE * f, const char * const format, va_list args) {
	vfprintf(f, format, args);
	fprintf(f, "\n");
	fflush(f);
}


/// Logs the arguments as printf would to the log file
/** This function promises **NOTHING** on failure */
void Utilities::log_no_newline(const char * const format, ...) {
	if ( LOG_FILE != nullptr ) {
		va_list args;
		va_start(args, format);
		vfprintf(LOG_FILE, format, args);
		va_end(args);
	}
}

// Logs the arguments as printf would to the log file
// Ends the printed line with a newline then flushes the buffer
// If LOG_FILE is nullptr, this function does nothing
// This function promises **NOTHING** on failure
void Utilities::log(const char * const format, ...) {
	if ( LOG_FILE != nullptr ) {
		va_list args;
		va_start(args, format);
		write_log(LOG_FILE, format, args);
		va_end(args);
	}
}

// Prints the arguments as printf would to the stdout and log files
// Ends the printed line with a newline then flushes the buffer
// If STDOUT_FILE is nullptr, this function does nothing
// This function promises **NOTHING** on failure
void Utilities::message(const char * const format, ...) {

	// What files to write to (the set eliminates duplicates)
	static FILE * const fs[] { STDOUT_FILE, LOG_FILE };
	static const int len = 2;

	// Write to the files
	for ( int i = 0; i < len; ++i ) {
		if ( fs[i] != nullptr ) {
			va_list args;
			va_start(args, format);
			write_log(fs[i], format, args);
			va_end(args);
		}
	}
}

// Logs the arguments as printf would to the error, and log files
// Ends the printed line with a newline then flushes the buffer
// If either file is nullptr, that file is skipped
// This function does NOT print to STDOUT_FILE
// This function promises **NOTHING** on failure
void Utilities::log_error(const char * const format, ...) {

	// What files to write to (the set eliminates duplicates)
	static FILE * const fs[] { ERROR_FILE, LOG_FILE };
	static const int len = 2;

	// Write to the files
	for ( int i = 0; i < len; ++i ) {
		if ( fs[i] != nullptr ) {
			fprintf(fs[i], "ERROR: ");
			va_list args;
			va_start(args, format);
			write_log(fs[i], format, args);
			va_end(args);
		}
	}
}
