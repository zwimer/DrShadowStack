#include "utilities.hpp"
#include "constants.hpp"
#include "get_tid.hpp"
#include "group.hpp"

#include <unistd.h>
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
#ifdef LOG_FILE

		// If log should be remove, try to unlink it
		if (clear_log) {
			const int rv = unlink(LOG_FILE);
			assert( (rv == 0) || (rv == ENOENT), "unlink() failed." );
		}

		// Open the log file
		log_file = fopen(LOG_FILE, "a");
		assert( log_file != nullptr, "fopen() failed." );
#endif
}


/*********************************************************/
/*                                                       */
/*                		Error checking		             */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Error logs s, perrors, then kills the process group
// If s is null, just terminates the group
[[ noreturn ]] void Utilities::err(const char * const s) {
	TerminateOnDestruction tod;
	if ( s != nullptr ) {
		log_error(s, "\nMessage from strerror: ", strerror(errno));
	}
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
