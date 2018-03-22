#include "group.hpp"
#include "utilities.hpp"
#include "constants.hpp"
#include "get_tid.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <set>


// My default the callback should be null
void (* Group::delete_proc_rc) () = nullptr;

// Used to tell if the process group has started
bool Group::setup_complete = false;


// Signals whose handlers should not be changed
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


/*********************************************************/
/*                                                       */
/*                	  Helper Functions					 */
/*                                                       */
/*********************************************************/


// A default signal handler
// This handler will terminate the process group 
void default_signal_handler(int sig) {
	Utilities::log_error("\nTID %d: Signal %d caught. Terminating "
							"process group...", get_tid(), sig);
	Group::terminate(nullptr);
}

// Change the default signal handler of most signals
// Sets the signal handler of each to the argument handler
void set_default_signal_handler(void (*handler) (int sig)) {

	// For all possible signals, if it should be changed...
	for ( int i = 1; i <= _NSIG; ++i ) {
		if ( no_change.find(i) == no_change.end() ) {

			// Try to change the handler. If signal fails 
			// for any reason other than EINVAL, terminate the group
			if ( (signal(i, handler) == SIG_ERR) && (errno != EINVAL) ) {
				Utilities::log_error("Error changing signal handler of signal : %d\n", i);
				Utilities::log_error("strerror returns: %s", strerror(errno));
				Group::terminate(nullptr);
			}
		}
	}
}


/*********************************************************/
/*                                                       */
/*                     Class functions				 	 */
/*                                                       */
/*********************************************************/


// Constructor. Enabled terminateion on destruction by default
TerminateOnDestruction::TerminateOnDestruction() : enabled(true) {}

// On destruction, terminate the group if enabled
TerminateOnDestruction::~TerminateOnDestruction() { 
	if (enabled) {
		Group::terminate("TerminateOnDestruction destructor called");
	}
}

// Disable termination of the group on destruction
void TerminateOnDestruction::disable() {
	enabled = false;
}


// Note: DynamoRIO does NOT play well with pthreads.
// Since group.cpp is compiled into the DynamoRIO client
// group.cpp may NOT include proc_rc.hpp. This leads
// to a few oddities of the Group class


// Setup the group
void Group::setup() {
	TerminateOnDestruction tod;

	// No issues, continue
	if ( ! Group::setup_complete ) {
		Group::setup_complete = true;

		// Set up the group
		setsid();
		Utilities::log("Setup groupd with group id: %d", getpgrp());

		// Remap signal handlers
		set_default_signal_handler(default_signal_handler);
		Utilities::log("Remapped signal handlers");
	}

	// setup_group() was already called
	else {
		terminate("ERROR: setup is complete already. "
			"Terminating program...\n");
	}

	// Nothing went wrong
	tod.disable();
}

// Terminates the process group via SIGKILL
// Frees shared memeory via delete_proc_rc
// If is_error is set to true, msg is logged to the
// ERROR file, otherwise it is logged via Utilities::message
// If msg is nullptr, no message is passed.
// If this function ends up calling itself, 
// immediate process group termination will occur
void Group::terminate(const char * const msg, bool is_error) {

	// If this is ever true coming in, temrinate_group()
	// caused an error. Take no chances, kill everything instantly.
	static bool terminate_already_called = false;
	if ( terminate_already_called ) {
		killpg(0, SIGKILL);
	}
	terminate_already_called = true;
	TerminateOnDestruction tod;

	// If the delete_proc_rc callback was registered, call it
	if ( delete_proc_rc != nullptr ) {
		delete_proc_rc();
	}

	// Print the message via the correct function then flush the buffers
	if (msg != nullptr) {
		(is_error ? Utilities::log_error : Utilities::message)("%s", msg);
	}
	fflush(nullptr);

	// If setup was complete, this is a server process, kill the group
	if ( Group::setup_complete ) {
		killpg(0, SIGKILL);
	}

	// Otherwise, this was a DynamoRIO client process, just kill this process
	else {
		_Exit(EXIT_SUCCESS);
	}
}

// Registers the proc_rc destructor
// This function exists as group.cpp may NOT include
// proc_rc.hpp, as proc_rc.hpp generally uses pthreads
// internally. DynamoRIO does not play nice with pthreads.
// Thus to free allocated memory, a callback registration is required */
void Group::register_delete_proc_rc( void (* call) () ) {
	Utilities::assert( delete_proc_rc == nullptr, 
		"delete_proc_rc already registered!" );
	delete_proc_rc = call;
}
