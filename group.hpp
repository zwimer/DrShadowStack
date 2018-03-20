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


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


/// Setup the group
/** This function should NOT be called by the dynamorio client.
 *  By then the group will already be set up!
 *  This function first sets up the process group
 *  Then change the default signal handlers of all non-excluded
 *  signals to terminate the group when called. Also sets up 
 *  the group refrence counter. This function is **NOT** thread 
 *  safe, and should never be run after threading / forking has occured! */
void setup_group();

/** Terminates the process group via SIGKILL */
void terminate_group();


#endif
