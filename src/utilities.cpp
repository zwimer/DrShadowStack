#include "utilities.hpp"
#include "constants.hpp"
#include "get_tid.hpp"
#include "group.hpp"

#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <set>


/*********************************************************/
/*                                                       */
/*             			Defining files					 */
/*                                                       */
/*********************************************************/


// Error checking
FILE * const Utilities::log_file = LOG_FILE;
FILE * const Utilities::error_file = ERROR_FILE;
FILE * const Utilities::stdout_file= STDOUT_FILE;


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


// This function does nothing but return
inline static void no_op(const char * const, ...) {}

// The helper that writes args in the format of format to f
// Ends the printed line with a newline then flushes the buffer
// This function promises NOTHING on failure
inline static void write_log(FILE * f, const char * const format, va_list args) {
	fprintf(f, "TID %jd: ", (intmax_t) get_tid());
	vfprintf(f, format, args);
	fprintf(f, "\n");
	fflush(f);
}


// Logs the arguments as printf would to the log file
// Ends the printed line with a newline then flushes the buffer
// If log_file is nullptr, this function does nothing
// This function promises **NOTHING** on failure
// This function only runs if VERBOSE is defined
void (* const Utilities::verbose_log) (const char * const format, ...)
#ifdef VERBOSE
	= Utilities::log;
#else
	= no_op;
#endif

// Logs the arguments as printf would to the log file
// Ends the printed line with a newline then flushes the buffer
// If log_file is nullptr, this function does nothing
// This function promises **NOTHING** on failure
void Utilities::log(const char * const format, ...) {
	if ( log_file != nullptr ) {
		va_list args;
		va_start(args, format);
		write_log(log_file, format, args);
		va_end(args);
	}
}

// Prints the arguments as printf would to the stdout and log files
// Ends the printed line with a newline then flushes the buffer
// If stdout_file is nullptr, this function does nothing
// This function promises **NOTHING** on failure
void Utilities::message(const char * const format, ...) {

	// What files to write to (the set eliminates duplicates)
	const std::set<FILE *> fs { stdout_file, log_file };

	// Write to the files
	for ( auto i = fs.begin(); i != fs.end(); ++i ) {
		if ( *i != nullptr ) {
			va_list args;
			va_start(args, format);
			write_log(*i, format, args);
			va_end(args);
		}
	}
}

// Logs the arguments as printf would to the error, and log files
// Ends the printed line with a newline then flushes the buffer
// If either file is nullptr, that file is skipped
// This function does NOT print to stdout_file
// This function promises **NOTHING** on failure
void Utilities::log_error(const char * const format, ...) {

	// What files to write to (the set eliminates duplicates)
	const std::set<FILE *> fs { error_file, log_file };

	// Write to the files
	for ( auto i = fs.begin(); i != fs.end(); ++i ) {
		if ( *i != nullptr ) {
			fprintf(*i, "ERROR: ");
			va_list args;
			va_start(args, format);
			write_log(*i, format, args);
			va_end(args);
		}
	}
}
