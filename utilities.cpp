#include "utilities.hpp"
#include "constants.hpp"
#include "group.hpp"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>


/*********************************************************/
/*                                                       */
/*                		Error checking		             */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Error logs s, perrors, then kills the process group
void program_err(const char * const s) {
	TerminateOnDestruction tod;
    ss_log_error("\nERROR: %s", s);
	perror("");
	terminate_group();
}

// assert b, if false call err(s)
void ss_assert(const bool b, const char * const s) {
    if ( ! b ) {
		program_err(s);
	}
}


/*********************************************************/
/*                                                       */
/*                		  Logging						 */
/*                                                       */
/*********************************************************/


// The helper that writes args in the format of format to f
// Ends the printed line with a '\n' then flushes the buffer
// This function promises NOTHING on failure
void ss_write_log(FILE * f, const char * const format, va_list args) {
	vfprintf(f, format, args);
	fprintf(f, "\n");
	fflush(f);
}

// Logs the arguments as printf would to the log file
// Ends the printed line with a '\n' then flushes the buffer
// This function promises NOTHING on failure
void ss_log(const char * const format, ...) {
	va_list args;
	va_start(args, format);
	ss_write_log(LOG_FILE, format, args);
	va_end(args);
}

// Logs the arguments as printf would to the error and log files
// Ends the printed line with a '\n' then flushes the buffer
// This function promises NOTHING on failure
void ss_log_error(const char * const format, ...) {

	// What files to write to
	std::vector<FILE *> fs = { 
		ERROR_FILE, 
		LOG_FILE 
	};	

	// Write to the files
	for ( unsigned int i = 0; i < fs.size(); ++i ) {
		va_list args;
		va_start(args, format);
		ss_write_log(fs[i], format, args);
		va_end(args);
	}
}
