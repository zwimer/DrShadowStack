#include "utilities.hpp"
#include "constants.hpp"
#include "group.hpp"

#include <sys/syscall.h>
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

// Sets up the utility class
void Utilities::setup(const bool clear_log) {
#ifdef LOG_FILE

		// Note: if anything fails, log_file is nullptr, so the
		// logging functions will ignore it - It is safe to call them

		// If log should be remove, try to unlink it
		if (clear_log) {
			const auto old = errno;
			if ((unlink(LOG_FILE) != 0) && (errno != ENOENT)) {
				log_error("unlink() failed.");
				exit(EXIT_FAILURE);
			}
			errno = old;
		}

		// Open the log file
		log_file = fopen(LOG_FILE, "a");
		if ( log_file == nullptr ) {
			log_error("fopen() failed.");
			exit(EXIT_FAILURE);
		}
#endif
}

// Get the system page size
inline size_t get_page_size() {
	static const size_t page_size = (size_t) sysconf(_SC_PAGESIZE);
	Utilities::assert( page_size != -1, "sysconf() failed.");
	return page_size;
}

// Returns the system page size
size_t Utilities::get_page_size() {
	static const size_t page_size = (size_t) sysconf(_SC_PAGESIZE);
	return page_size;
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


// Define a gettid function
// On failure, disables multi_threaded / functionality 
// (to continue logging) then terminates the group
pid_t Utilities::get_tid() {
	const pid_t ret = syscall(SYS_gettid);
	if ( ret == -1 ) {
		is_multi_thread_or_proccess = false;
		Group::terminate("get_tid() failed.");
	}
}

// Once this is called, TIDs will be printed with each message
void Utilities::enable_multi_thread_or_process_mode() {
	log("Multi-process/threading logging enabled");
	is_multi_thread_or_proccess = true;
}
