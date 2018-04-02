#include "utilities.hpp"
#include "constants.hpp"
#include "get_tid.hpp"
#include "group.hpp"

#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <set>


/*********************************************************/
/*                                                       */
/*             		   Initalizations					 */	
/*                                                       */
/*********************************************************/


// Notes whether or not any threading / forking has happened yet
bool Utilities::is_multi_thread_or_proccess = false;

// Error checking
FILE * Utilities::log_file = nullptr;
FILE * const Utilities::error_file = ERROR_FILE;
FILE * const Utilities::stdout_file= STDOUT_FILE;

/// A singleton constructor
/** This exists so that if a global Utilities is declared
 *  it will setup everyhing needed for the class */
void Utilities::setup(const bool clear_log) {
	if ( LOG_FILE != nullptr ) {
		log_file = fopen(LOG_FILE, clear_log ? "w":"a");
		assert( log_file != nullptr, "fopen() failed." );
	}
}


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


// Once this is called, TIDs will be printed with each message
void Utilities::enable_multi_thread_or_process_mode() {
	log("Multi-process/threading logging enabled");
	is_multi_thread_or_proccess = true;
}

// This function does nothing but return
inline static void no_op(const char * const, ...) {}

// The helper that writes args in the format of format to f
// Ends the printed line with a newline then flushes the buffer
// This function promises NOTHING on failure
// If the process is multithreaded or has forked, prints the TID first
void Utilities::write_log(FILE * f, const char * const format, va_list args) {
	if (is_multi_thread_or_proccess) {
		fprintf(f, "TID %jd: ", (intmax_t) get_tid());
	}
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
