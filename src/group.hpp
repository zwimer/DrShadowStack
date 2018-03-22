/** @file */
#ifndef __GROUP_HPP__
#define __GROUP_HPP__


/*********************************************************/
/*                                                       */
/*                    Class Declarations                 */
/*                                                       */
/*********************************************************/


/// A class that terminates the group when its destructor is called
/** This is called even if a C++ exception is thrown
 *  Note: this should be used as a backup, terminate_group
 *  should still be called if termination is desired! */
class TerminateOnDestruction final {
private:

	// Delete unwanted 'constructors'
	TerminateOnDestruction(TerminateOnDestruction && ) = delete;
	TerminateOnDestruction(const TerminateOnDestruction & ) = delete;
	TerminateOnDestruction & operator=(const TerminateOnDestruction & ) = delete;

	// If enabled, terminate_group on destruction
	bool enabled;
public:

	/// Constructor
	/** Enabled terminateion on destruction by default */
	TerminateOnDestruction();

	/// Destructor
	/** On destruction, terminate the group if enabled */
	~TerminateOnDestruction();

	/** Disable termination of the group on destruction */
	void disable();
};


// Note: DynamoRIO does NOT play well with pthreads.
// Since group.cpp is compiled into the DynamoRIO client
// group.cpp may NOT include proc_rc.hpp. This leads
// to a few oddities of the Group class


/** A static class holding the group functions */
struct Group {

	/// Setup the group
	/** This function should NOT be called by the dynamorio client.
	 *  By then the group will already be set up!
	 *  This function first sets up the process group
	 *  Then change the default signal handlers of all non-excluded
	 *  signals to terminate the group when called. Also sets up 
	 *  the group refrence counter. This function is **NOT** thread 
	 *  safe, and should never be run after threading / forking has occured! */
	static void setup();

	/// Terminates the process group via SIGKILL
	/** Frees shared memeory via delete_proc_rc
	 *  If is_error is set to true, msg is logged to the
	 *  ERROR file, otherwise it is logged via Utilities::message
	 *  If msg is nullptr, no message is passed.
	 *  If this function ends up calling itself, 
	 *  immediate process group termination will occur */
	static void terminate(const char * const msg, bool is_error = false);
 
	/// Registers the proc_rc destructor
	/** This function exists as group.cpp may NOT include
	 *  proc_rc.hpp, as proc_rc.hpp generally uses pthreads
	 *  internally. DynamoRIO does not play nice with pthreads.
	 *  Thus to free allocated memory, a callback registration is required */
	static void register_delete_proc_rc( void (* call) () );

	/** The recorded callback to the proc_rc destructor */
	static void (* delete_proc_rc) ();

private:

	/** Used to tell if the process group has started */
	static bool setup_complete;
};


#endif
