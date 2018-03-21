#include "utilities.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

#include "constants.hpp"
#include "group.hpp"


/*********************************************************/
/*                                                       */
/*                		Error checking		             */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Error logs s, perrors, then kills the process group
void Utilities::err(const char * const s) {
	TerminateOnDestruction tod;
    log_error("\nERROR: %s\nMessage from strerror: %s", s, strerror(errno));
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

// Prints the arguments as printf would to the STDOUT_FILE file
// Ends the printed line with a newline then flushes the buffer
// If STDOUT_FILE is nullptr, this function does nothing
// This function promises **NOTHING** on failure */
void Utilities::message(const char * const format, ...) {
	if ( STDOUT_FILE != nullptr ) {
		va_list args;
		va_start(args, format);
		write_log(STDOUT_FILE, format, args);
		va_end(args);
	}
}

// Logs the arguments as printf would to the log file
// Ends the printed line with a newline then flushes the buffer
// If LOG_FILE is nullptr, this function does nothing
// This function promises NOTHING on failure
void Utilities::log(const char * const format, ...) {
	if ( LOG_FILE != nullptr ) {
		va_list args;
		va_start(args, format);
		write_log(LOG_FILE, format, args);
		va_end(args);
	}
}

// Logs the arguments as printf would to the error and log files
// Ends the printed line with a newline then flushes the buffer
// If LOG_FILE and ERROR_FILE are the same, only prints once
// If either LOG_FILE is nullptr, that file is skipped
// This function promises NOTHING on failure
void Utilities::log_error(const char * const format, ...) {

	// What files to write to
	std::vector<FILE *> fs = { 
		ERROR_FILE, 
		LOG_FILE, 
	};	

	// If LOG_FILE and ERROR_FILE are the same, only print once
	if ( LOG_FILE == ERROR_FILE ) {
		fs.pop_back();
	}

	// Write to the files
	for ( unsigned int i = 0; i < fs.size(); ++i ) {
		if ( fs[i] != nullptr ) {
			va_list args;
			va_start(args, format);
			write_log(fs[i], format, args);
			va_end(args);
		}
	}
}
