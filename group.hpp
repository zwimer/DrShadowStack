#ifndef __GROUP_HPP__
#define __GROUP_HPP__


/*********************************************************/
/*                                                       */
/*                    Class Declarations                 */
/*                                                       */
/*********************************************************/


// A class that terminates the 
// group when its destructor is called
// This is called even if a C++ exception is thrown
// Note: this should be used as a backup, terminate_group
// should still be called if termination is desired!
class TerminateOnDestruction final {
private:

	// Delete un-needed constructors
	TerminateOnDestruction(TerminateOnDestruction && ) = delete;
	TerminateOnDestruction(const TerminateOnDestruction & ) = delete;
	TerminateOnDestruction & operator=(const TerminateOnDestruction & ) = delete;

	// If enabled, terminate_group on destruction
	bool enabled;
public:

	// Constructor
	TerminateOnDestruction();

	// On destruction, terminate the group
	~TerminateOnDestruction();

	// Disable terminate on destruction
	void disable();
};


/*********************************************************/
/*                                                       */
/*                  Function Declarations                */
/*                                                       */
/*********************************************************/


// Start the group, then change the default signal handlers
// of common program killing signals to terminate the group
// Also sets up the group refrence counter
// This function is NOT thread safe, and should never be 
// run after threading / forking has occured!
void setup_group();

// Ends the group
void terminate_group();

// This function increases the reference count
void valid_inc_proc_count();

// This function decreases the reference count
// If the count hits 0, the group is terminated
void valid_dec_proc_count();


#endif
