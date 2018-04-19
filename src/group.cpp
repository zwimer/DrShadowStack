#include "group.hpp"
#include "utilities.hpp"
#include "constants.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <set>


// Used to tell if the process group has started
bool Group::setup_complete = false;


// Signals whose handlers should not be changed
/* clang-format off */
const static std::set<int> no_change{ 
	SIGKILL,
	SIGSTOP,
	SIGURG,
	SIGCONT,
	SIGCHLD,
	SIGIO, /* SIGPOLL */
	SIGWINCH,
	0
};
/* clang-format on */


/*********************************************************/
/*                                                       */
/*                    Helper Functions                   */
/*                                                       */
/*********************************************************/


// A default signal handler
// This handler will terminate the process group
void default_signal_handler( int sig ) {
	Utilities::log_error( "\nSignal ", sig, " (", strsignal( sig ),
	                      ") caught.\nTerminating process group..." );
	Group::terminate( nullptr );
}

// Change the default signal handler of most signals
// Sets the signal handler of each to the argument handler
void set_default_signal_handler( void ( *handler )( int sig ) ) {

	// For all possible signals, if it should be changed...
	for ( int i = 1; i <= _NSIG; ++i ) {
		if ( no_change.find( i ) == no_change.end() ) {

			// Try to change the handler. If signal fails
			// for any reason other than EINVAL, terminate the group
			if ( ( signal( i, handler ) == SIG_ERR ) && ( errno != EINVAL ) ) {
				Utilities::log_error( "failed to change signal handler of signal: ", i,
				                      "\nstrerror returns: ", strerror( errno ) );
				Group::terminate( nullptr );
			}
		}
	}
}

// Kill -SIGKILL the process group
// Since we may return before kernel kills everything
// we enter an infinite loop to wait for death
[[noreturn]] void kill_9_group() {
	killpg( 0, SIGKILL );
	while ( true ) {
	}
}


/*********************************************************/
/*                                                       */
/*                     Class function                    */
/*                                                       */
/*********************************************************/


// Constructor. Enabled terminateion on destruction by default
TerminateOnDestruction::TerminateOnDestruction()
    : enabled( true ) {}

// On destruction, terminate the group if enabled
TerminateOnDestruction::~TerminateOnDestruction() {
	if ( enabled ) {
		Group::terminate( "TerminateOnDestruction destructor called" );
	}
}

// Disable termination of the group on destruction
void TerminateOnDestruction::disable() { enabled = false; }


// Note: DynamoRIO does NOT play well with pthreads.
// Since group.cpp is compiled into the DynamoRIO client
// group.cpp may NOT include proc_rc.hpp. This leads
// to a few oddities of the Group class


// Setup the group
void Group::setup() {
	TerminateOnDestruction tod;

	// No issues, continue
	if ( !Group::setup_complete ) {
		Group::setup_complete = true;

		// Set up the group
		setsid();
		Utilities::log( "Setup group with group id: ", getpgrp() );

		// Remap signal handlers
		set_default_signal_handler( default_signal_handler );
		Utilities::log( "Remapped signal handlers" );
	}

	// setup_group() was already called
	else {
		terminate( "ERROR: setup is complete already.\n"
		           "\tTerminating program...\n" );
	}

	// Nothing went wrong
	tod.disable();
}

// Terminates the process group via SIGKILL
// If is_error is set to true, msg is logged to the
// ERROR file, otherwise it is logged via Utilities::message
// If msg is nullptr, no message is passed.
// If this function ends up calling itself,
// immediate process group termination will occur
// setup() **DOES NOT** have to be called before this function
// in every process. However, it **MUST** be called by DrShadowStack once first
[[noreturn]] void Group::terminate( const char *const msg, bool is_error ) {
	// If this is ever true coming in, temrinate_group()
	// caused an error. Take no chances, kill everything instantly.
	static bool terminate_already_called = false;
	if ( terminate_already_called ) {
		kill_9_group();
	}
	terminate_already_called = true;
	TerminateOnDestruction tod;

	// Print the message via the correct function then flush the buffers
	if ( msg != nullptr ) {
		if ( is_error ) {
			Utilities::log_error( msg );
		}
		else {
			Utilities::message( msg );
		}
	}
	fflush( nullptr );

	// Kill the process group
	kill_9_group();
}
