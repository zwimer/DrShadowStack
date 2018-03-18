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
/** Then change the default signal handlers of all non-excluded
 *  signals to terminate the group when called. Also sets up 
 *  the group refrence counter. This function is **NOT** thread 
 *  safe, and should never be run after threading / forking has occured! */
void setup_group();

/** Terminates the process group via SIGKILL */
void terminate_group();

/** This function increases the reference count */
void valid_inc_proc_count();

/// This function decreases the reference count
/** If the count hits 0, the group is terminated */
void valid_dec_proc_count();


#endif
