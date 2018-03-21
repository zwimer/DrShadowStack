#include "group.hpp"
#include "utilities.hpp"
#include "constants.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <set>


// Used to tell if the process group has started
static bool setup_complete = false;

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
	ss_log_error("\nSignal %d caught. Terminating process group...", sig);
	terminate_group();
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
				ss_log_error("Error on signal: %d\n", i);
				perror("signal() failed.\n");
				terminate_group();
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


// Setup the group
void setup_group() {
	TerminateOnDestruction tod;

	// No issues, continue
	if ( ! setup_complete ) {
		setup_complete = true;

		// Set up the group
		setsid();
		ss_log("Setup groupd with group id: %d", getpgrp());

		// Remap signal handlers
		set_default_signal_handler(default_signal_handler);
		ss_log("Remapped signal handlers");
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


// Terminates the process group via SIGKILL
void terminate_group() {

	// If this is ever true coming in, temrinate_group()
	// caused an error. Take no chances, kill everything instantly.
	static bool terminate_already_called = false;
	if ( terminate_already_called ) {
		killpg(0, SIGKILL);
	}
	terminate_already_called = true;
	TerminateOnDestruction tod;

	// Flush buffers
	fflush(nullptr);

	// If setup was complete, this is a server process, kill the group
	if ( setup_complete ) {
		ss_log_error("Terminating process group\n");
		killpg(0, SIGKILL);
	}

	// If setup was incomplete, this was a DynamoRIO 
	// client process, just exit the process
	else {
		ss_log_error("Client called terminate. Terminating current process only.");
		kill(0, SIGKILL);
	}
}
