#include "utilities.hpp"
#include "constants.hpp"
#include "group.hpp"

#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <set>


/*********************************************************/
/*                                                       */
/*                     Initalizations                    */
/*                                                       */
/*********************************************************/


// Notes whether or not any threading / forking has happened yet
bool Utilities::is_multi_thread_or_proccess = false;

// Error checking
int get_fd( FILE *const f );
int Utilities::log_fd = Utilities::invalid_fd;
const int Utilities::error_fd = get_fd( ERROR_FILE );
const int Utilities::stdout_fd = get_fd( STDOUT_FILE );


// fileno but with error checking
// On failure, exits the process, not the group
int get_fd( FILE *const f ) {
	if ( f == nullptr ) {
		return Utilities::invalid_fd;
	}
	const int ret = fileno( f );
	if ( ret == -1 ) {
		exit( EXIT_FAILURE );
	}
	return ret;
}


// Sets up the utility class
void Utilities::setup( const bool clear_log ) {
#ifdef LOG_FILE
	// Note: if anything fails, log_file is nullptr, so the
	// logging functions will ignore it - It is safe to call them

	// If log should be remove, try to unlink it
	if ( clear_log ) {
		const auto old = errno;
		if ( ( unlink( LOG_FILE ) != 0 ) && ( errno != ENOENT ) ) {
			log_error( "unlink() failed." );
			exit( EXIT_FAILURE );
		}
		errno = old;
	}

	// Open the log file
	const int fd =
	    open( LOG_FILE, O_SYNC | O_CREAT | O_APPEND | O_CLOEXEC | O_WRONLY, 0640 );
	if ( fd == -1 ) {
		log_error( "open() failed." );
		exit( EXIT_FAILURE );
	}
	log_fd = fd;
#endif
}


/*********************************************************/
/*                                                       */
/*                      Error checking                   */
/*                                                       */
/*********************************************************/


// To be called in case of an error
// Error logs s, perrors, then kills the process group
// If s is null, just terminates the group
[[noreturn]] void Utilities::err( const char *const s ) {
	TerminateOnDestruction tod;
	if ( s != nullptr ) {
		log_error( s, "\nMessage from strerror: ", strerror( errno ) );
	}
	Group::terminate( nullptr );
}

// assert b, if false call err(s)
void Utilities::assert( const bool b, const char *const s ) {
	if ( !b ) {
		err( s );
	}
}


/*********************************************************/
/*                                                       */
/*                        Logging                        */
/*                                                       */
/*********************************************************/


// Define a gettid function
// On failure, disables multi_threaded / functionality
// (to continue logging) then terminates the group
pid_t Utilities::get_tid() {
	const pid_t ret = syscall( SYS_gettid );
	if ( ret == -1 ) {
		is_multi_thread_or_proccess = false;
		Group::terminate( "get_tid() failed." );
	}
	return ret;
}

// Once this is called, TIDs will be printed with each message
void Utilities::enable_multi_thread_or_process_mode() {
	is_multi_thread_or_proccess = true;
	log( "Multi-process/threading logging enabled" );
}
