#include "group.h"

#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


// Used to tell if the process group has started
static int setup_complete = 0;


/*********************************************************/
/*                                                       */
/*                	  Helper Functions					 */
/*                                                       */
/*********************************************************/


// A default signal handler
void default_signal_handler(int sig) {
	fprintf(stderr, "\nSignal %d caught. Terminating process group...\n", sig);
	terminate_group();
}

// Return false if sig is in no_change, else: true
int should_change(const int * const no_change, const int sig) {
	for ( int i = 0; no_change[i] != 0; ++i ) {
		if ( no_change[i] == sig ) {
			return 0;
		}
	}
	return (sig != 0);
}

// Change the default signal handler of common program killing signals
void set_default_signal_handler(void (*handler) (int sig)) {
	
	// TODO: make better...
	const int no_change[] = {
		SIGKILL,
		SIGSTOP,
		SIGURG,
		SIGCONT,
		SIGCHLD,
		SIGPOLL, /* SIGIO */
		SIGWINCH,
		0
	};	

	// For all possible signals, if it should be changed...
	for ( int i = 1; i <= _NSIG; ++i ) {
		if ( should_change(no_change, i) ) {

			// Try to change the handler. If signal fails 
			// for any reason other than EINVAL, terminate the group
			if ( (signal(i, handler) == SIG_ERR) && (errno != EINVAL) ) {
				fprintf(stderr, "Error on signal: %d\n", i);
				perror("signal() failed.\n");
				terminate_group();
			}
		}
	}
}


/*********************************************************/
/*                                                       */
/*                Functions from header file			 */
/*                                                       */
/*********************************************************/


// Start the group, then change the default signal handlers
// of common program killing signals to terminate the group
void setup_group() {

	// No issues, setup the group
	if ( setup_complete == 0) {
		setup_complete = 1;
		setsid();
		set_default_signal_handler(default_signal_handler);
	}

	// setup_group() was already called
	else {
		fprintf(stderr, "ERROR: setup is complete already. "
			"Terminating program...\n");
		terminate_group();
	}
}


// Terminate the group
void terminate_group() {

	// Flush buffers
	fflush(NULL);

	// If setup was complete, kill the group
	if ( setup_complete ) {
		fprintf(stderr, "Terminating process group\n");
		killpg(0, 9);
	}

	// Otherwise, something major went wrong
	else {
		fprintf(stderr, "ERROR: Program called terminate_group before setup_group\n");
		fprintf(stderr, "This program is too cowardly to kill the group... exiting program.\n");
		exit(EXIT_FAILURE);
	}
}
