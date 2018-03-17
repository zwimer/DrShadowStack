#include "group.hpp"
#include "utilities.hpp"
#include "constants.hpp"

#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <set>

#include <boost/interprocess/sync/interprocess_mutex.hpp>


// The type used as a rc for the number of processes
typedef long proc_rc;


// Used to tell if the process group has started
static bool setup_complete = false;

// A reference counter for the number of 
// processes and the mutex that protects it
static proc_rc * num_proc_rc = nullptr;
static boost::interprocess::interprocess_mutex rc_lock;


/*********************************************************/
/*                                                       */
/*                	  Helper Functions					 */
/*                                                       */
/*********************************************************/


// A default signal handler
void default_signal_handler(int sig) {
	ss_log_error("\nSignal %d caught. Terminating process group...", sig);
	terminate_group();
}

// Change the default signal handler of common program killing signals
void set_default_signal_handler(void (*handler) (int sig)) {
	
	// Signals not to change the handlers of
	const static std::set<int> no_change {
		SIGKILL,
		SIGSTOP,
		SIGURG,
		SIGCONT,
		SIGCHLD,
		SIGIO, /* SIGPOLL */
		SIGWINCH,
		0
	};	

	// For all possible signals, if it should be changed...
	for ( int i = 1; i <= _NSIG; ++i ) {
		if ( no_change.find(i) == no_change.end() ) {

			// Try to change the handler. If signal fails 
			// for any reason other than EINVAL, terminate the group
			if ( (signal(i, handler) == SIG_ERR) && (errno != EINVAL) ) {
				ss_log_error("Error on signal: %d\n", i);
				perror("signal() failed.\n");
				terminate_group();
			}
		}
	}
}

// Create a chunk of chared memory of size size
void * create_shared_memory(const size_t size) {

	// Our memory buffer will be readable and writable:
	const int protection = PROT_READ | PROT_WRITE;

	// The buffer will be shared (meaning other processes can access it), but
	// anonymous (meaning third-party processes cannot obtain an address for it),
	// so only this process and its children will be able to use it:
	const int visibility = MAP_ANONYMOUS | MAP_SHARED;

	// Allocate the memory and return it
	void * const ret = mmap(nullptr, size, protection, visibility, 0, 0);
	ss_assert( (ret != MAP_FAILED), "mmap() failed." );
	return ret;
}


/*********************************************************/
/*                                                       */
/*                     Class functions				 	 */
/*                                                       */
/*********************************************************/


// Constructor
TerminateOnDestruction::TerminateOnDestruction() : enabled(true) {}

// On destruction, terminate the group if enabled
TerminateOnDestruction::~TerminateOnDestruction() { 
	if (enabled) {
		ss_log_error("TerminateOnDestruction destructor called");
		terminate_group(); 
	}
}

// Disable termination of the group on destruction
void TerminateOnDestruction::disable() {
	enabled = false;
}


/*********************************************************/
/*                                                       */
/*                Functions from header file			 */
/*                                                       */
/*********************************************************/


// Start the group, then change the default signal handlers
// of common program killing signals to terminate the group
// Also sets up the group refrence counter
// This function is NOT thread safe, and should never be 
// run after threading / forking has occured!
void setup_group() {
	TerminateOnDestruction tod;

	// No issues, setup the group
	if ( ! setup_complete ) {
		setup_complete = true;
		setsid();

		// Remap signal handlers
		set_default_signal_handler(default_signal_handler);

		// Setup the reference counter
		num_proc_rc = (proc_rc *) create_shared_memory(sizeof(proc_rc));	
		valid_inc_proc_count();
	}

	// setup_group() was already called
	else {
		ss_log_error("ERROR: setup is complete already. "
			"Terminating program...\n");
		terminate_group();
	}

	// Nothing went wrong
	tod.disable();
}


// Terminate the group
void terminate_group() {
	TerminateOnDestruction tod;

	// If this is ever true coming in, temrinate_group()
	// caused an error. Take no chances, kill everything instantly.
	static bool terminate_already_called = false;
	if ( terminate_already_called ) {
		killpg(0, 9);
	}
	terminate_already_called = true;

	// Flush buffers
	fflush(nullptr);

	// If setup was complete, kill the group
	if ( setup_complete ) {
		ss_log_error("Terminating process group\n");
		killpg(0, SIGKILL);
	}

	// Otherwise, something major went wrong
	// Setup was never called...
	else {
		ss_log_error("ERROR: Program called terminate_group before setup_group\n");
		ss_log_error("This program is too cowardly to kill the group... exiting program.\n");
		exit(EXIT_FAILURE);
	}
}

// This function increases the reference count
void valid_inc_proc_count() {
	TerminateOnDestruction tod;
	rc_lock.lock();
	*num_proc_rc += 1;
	rc_lock.unlock();
	tod.disable();
}

// This function decreases the reference count
// If the count hits 0, the group is terminated
void valid_dec_proc_count() {
	TerminateOnDestruction tod;

	// Decrement the rc
	rc_lock.lock();
	*num_proc_rc -= 1;

	// If the rc is 0, kill everything
	if ( *num_proc_rc == 0 ) {
		ss_log("Valid process reference counter "
				"hit 0, terminating process group");
		terminate_group();
	}

	// Otherwise, release the lock
	rc_lock.unlock();
	tod.disable();
}
