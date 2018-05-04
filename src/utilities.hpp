/** @file */
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <unistd.h>
#include <sstream>


// Undefine macro assert, it clobbers the class member function assert
#undef assert


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


/** A general class for utilities */
class Utilities {

	/** Notes whether or not any threading / forking has happened yet */
	static bool is_multi_thread_or_proccess;

	/** The log file descriptor
	 *  This must be defined before main() */
	static int log_fd;

	/** The stdout file descriptor
	 *  This must be defined before main() */
	static const int stdout_fd;

	/** The error file descriptor
	 *  This must be defined before main() */
	static const int error_fd;

  public:

	/** A known invalid file descriptor */
	static constexpr const int invalid_fd = -2;

	/** Sets up the utilities class
	 *  If clear_log, the log will be cleared on setup
	 *  If this function fails, it exists *ONLY* the current process */
	static void setup( const bool clear_log );

	/** Define a gettid function
	 *  On failure, disables multi_threaded / functionality
	 *  (to continue logging) then terminates the group */
	static pid_t get_tid();

	/*********************************************************/
	/*                                                       */
	/*                     Error checking                    */
	/*                                                       */
	/*********************************************************/


	/** To be called in case of an error
	 *  Prints log s to the error file, perrors,
	 *  then kills the process group */
	[[noreturn]] static void err( const char *const s );

	/** assert b, if false call program_err(s) */
	static void assert( const bool b, const char *const s );


	/*********************************************************/
	/*                                                       */
	/*                        Logging                        */
	/*                                                       */
	/*********************************************************/


	/** Logs the arguments as cout would to the log file if not null
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  On faliure, silently fails (since it cannot write out) */
	template <typename... Args> static void log( Args &&... args ) {
		write_log( log_fd, std::forward<Args>( args )... );
	}

#ifdef DEBUG_MODE
#	ifdef VERBOSE
	/** Logs the arguments as cout would to the log file if not null
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  This function only runs if DEBUG_MODE and VERBOSE are defined */
	template <typename... Args> static void verbose_log(Args &&... args ) {
		log( std::forward<Args>( args )... );
	}
#else
	template <typename... Args> static void verbose_log( Args &&... ) { }
#	endif
#endif

	/** Prints the arguments as cout would to the stdout and log files
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  If either file descriptor is invalied, that file is skipped
	 *  On faliure, silently fails (since it cannot write out) */
	template <typename... Args> static void message( Args &&... args ) {
		write_log( log_fd, std::forward<Args>( args )... );
		write_log( stdout_fd, std::forward<Args>( args )... );
	}

	/** Prints the arguments as cout would to the error and log files
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  If either file descriptor is invalied, that file is skipped
	 *  On failure, silently fails (since it cannot write out) */
	template <typename... Args> static void log_error( Args &&... args ) {
		write_log( log_fd, std::forward<Args>( args )... );
		write_log( error_fd, std::forward<Args>( args )... );
	}

#ifdef DEBUG_MODE
	/** The same thing as log_error
	 *  This function is easy to seach for and will not
	 *  work for a release build, ensure no clutter remains */
	template <typename... Args> static void debug( Args &&... args ) {
		log_error( std::forward<Args>( args )... );
	}
#endif

	/** Once this is called, TIDs will be printed with each message */
	static void enable_multi_thread_or_process_mode();

  private:
	/** A wrapper that writes args to f if f is not null
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  On failure, silently fails (since it cannot write out)
	 *  If the process is multithreaded or has forked, prints the TID first */
	template <typename... Args> static void write_log( const int fd, Args &&... args );

	/** Adds the arguments to the stream
	 *  Ends the printed line(s) with a newline then flushes the buffer
	 *  If the process is multithreaded or has forked, prints the TID first */
	template <typename... Args>
	static void write_log_helper( std::stringstream &stream, Args &&... args );


	/*********************************************************/
	/*                                                       */
	/*         Template specialization implementations       */
	/*                                                       */
	/*********************************************************/


	/** A specialization of write_log_helper that has only one template argument */
	template <typename T>
	static void write_log_helper( std::stringstream &stream, T &&val ) {
		stream << val;
	}

	/** A specialization of write_log_helper that has multiple template arguments */
	template <typename Head, typename... Tail>
	static void write_log_helper( std::stringstream &stream, Head &&head,
	                              Tail &&... tail ) {
		stream << head;
		write_log_helper( stream, std::forward<Tail>( tail )... );
	}
};

/*********************************************************/
/*                                                       */
/*      Template non-specialization implementations      */
/*                                                       */
/*********************************************************/


// Write the arguments to f if f is not null
// On failure, silently fails (since it cannot write out)
template <typename... Args> void Utilities::write_log( const int fd, Args &&... args ) {
	if ( fd != invalid_fd ) {

		// Create the stream and note the tid if is_multi_thread_or_proccess
		std::stringstream stream;
		if ( is_multi_thread_or_proccess ) {
			stream << get_tid() << ": ";
		}

		write_log_helper( stream, std::forward<Args>( args )... );
		stream << '\n';

		// Write the string then flush the buffer
		// Assert message errors must be nullptr to prevent a loop
		const std::string str = stream.str();
		const ssize_t len = str.size();
		Utilities::assert( write(fd, str.c_str(), len ) == (ssize_t) len, nullptr );
	}
}


#endif /* utilities_h */
