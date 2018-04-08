/** @file */
#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <stdio.h>
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

	/// The log file
	/** This must be defined before main() */
	static FILE * log_file;

	/// The stdout file
	/** This must be defined before main() */
	static FILE * const stdout_file;

	/// The error file
	/** This must be defined before main() */
	static FILE * const error_file;

	/// Define a gettid function
	/** On failure, disables multi_threaded / functionality 
	 *  (to continue logging) then terminates the group */
	static pid_t get_tid();

public:

	/// Sets up the utilities class 
	/** If clear_log, the log will be cleared op setup
	 *  If s is null, just terminates the group */
	static void setup(const bool clear_log);

	/** Returns the system page size */
	static size_t get_page_size();

	/*********************************************************/
	/*                                                       */
	/*                		Error checking		             */
	/*                                                       */
	/*********************************************************/


	/// To be called in case of an error
	/** Prints log s to the error file, perrors,
	 *  then kills the process group */
	[[noreturn]] static void err(const char * const s);

	/** assert b, if false call program_err(s) */
	static void assert(const bool b, const char * const s);


	/*********************************************************/
	/*                                                       */
	/*                  	 for logging					 */
	/*                                                       */
	/*********************************************************/


	/// Logs the arguments as cout would to the log file if not null
	/** Ends the printed line(s) with a newline then flushes the buffer
	 *  This function only runs if VERBOSE is defined */
	template<typename... Args> static void verbose_log(Args && ... args) {
#ifdef VERBOSE
		log(std::forward<Args>(args)... );
#endif
	}

	/// Logs the arguments as cout would to the log file if not null
	/** Ends the printed line(s) with a newline then flushes the buffer
	 *  On faliure, silently fails (since it cannot write out) */
	template<typename... Args> static void log(Args && ... args) {
		write_log(log_file, std::forward<Args>(args)... );
	}

	/// Prints the arguments as cout would to the stdout and log files 
	/** Ends the printed line(s) with a newline then flushes the buffer
	 *  If either file pointer is null, that file is skipped
	 *  On faliure, silently fails (since it cannot write out) */
	template<typename... Args> static void message(Args && ... args) {
		write_log(log_file, std::forward<Args>(args)... );
		write_log(stdout_file, std::forward<Args>(args)... );
	}

	/// Prints the arguments as cout would to the error and log files 
	/** Ends the printed line(s) with a newline then flushes the buffer
	 *  If either file pointer is null, that file is skipped
	 *  On faliure, silently fails (since it cannot write out) */
	template<typename... Args> static void log_error(Args && ... args) {
		write_log(log_file, std::forward<Args>(args)... );
		write_log(error_file, std::forward<Args>(args)... );
	}

	/** Once this is called, TIDs will be printed with each message */
	static void enable_multi_thread_or_process_mode();

private:

	/// A wrapper that writes args to f if f is not null
	/** Ends the printed line(s) with a newline then flushes the buffer
	 *  On faliure, silently fails (since it cannot write out)
	 *  If the process is multithreaded or has forked, prints the TID first */
	template<typename... Args> static void write_log(FILE * const f, Args && ... args);

    /// Adds the arguments to the stream
    /** Ends the printed line(s) with a newline then flushes the buffer
     *  If the process is multithreaded or has forked, prints the TID first */
    template<typename... Args> static void write_log_helper(std::stringstream & stream,
                                                            Args && ... args);


	/*********************************************************/
	/*                                                       */
	/*         Template specialization implementations		 */
	/*                                                       */
	/*********************************************************/


	/** A specialiation of write_log_helper that has only one template argument */
	template <typename T> static void write_log_helper(std::stringstream & stream, T && val) {
		stream << val;
	}

	/** A specialiation of write_log_helper that has multiple template arguments */
	template <typename Head, typename... Tail>
	static void write_log_helper(std::stringstream & stream, Head && head, Tail && ... tail) {
		stream << head;
		write_log_helper( stream, std::forward<Tail>(tail)... );
	}
};

/*********************************************************/
/*                                                       */
/*      Template non-specialization implementations 	 */
/*                                                       */
/*********************************************************/


// Write the arguments to f if f is not null
// On faliure, silently fails (since it cannot write out)
template<typename... Args> void Utilities::write_log(FILE * const f, Args && ... args) {
	if ( f != nullptr ) {

		// Create the stream and note the tid if is_multi_thread_or_proccess
		std::stringstream stream;
		if ( is_multi_thread_or_proccess ) {
			stream << get_tid() << ": ";
		}

		write_log_helper( stream, std::forward<Args>(args)... );
		stream << '\n';

		// Write the string then flush the buffer
		const std::string str = stream.str();
		const auto len = str.size();
		Utilities::assert( fwrite(str.c_str(), len, 1, f) == 1, nullptr );
		Utilities::assert( fflush(f) == 0, nullptr );
	}
}


#endif /* utilities_h */
