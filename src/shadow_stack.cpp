#include "external_stack_server.hpp"
#include "quick_socket.hpp"
#include "parse_args.hpp"
#include "temp_name.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include <unistd.h>
#include <signal.h>
#include <vector>


// Undefine macro assert, it clobbers the class member function assert
#undef assert


// Calls setup functions.
// The order of these functions matters !
static inline void run_before_everything() {

	// Setup utilities
	Utilities::setup( true );
	Utilities::log( "DrShadowStack initalized" );

	// Create a new process group by starting a new session
	// Many terminals will automatically do this, but just in case...
	// Also changes many default signal handlers to kill the process group
	Utilities::log( "Setting up the group now..." );
	Group::setup();
}

// Start's the program passed in via drrun
[[noreturn]] void start_program( const Args input_args, char *socket_path ) {
	// Construct args to give to exec
	std::vector<const char *> exec_args;

	// Drrun a client
	exec_args.push_back( DRRUN_PATH );
	exec_args.push_back( "-c" );

	// ShadowStack dynamorio client + args
	exec_args.push_back( DYNAMORIO_CLIENT_SO );
	exec_args.push_back( input_args.mode.str );
	exec_args.push_back( socket_path );

	// Specify target a.out
	exec_args.push_back( "--" );
	exec_args.push_back( input_args.target.c_str() );

	// Add a.out's args
	for ( int i = 0; i < input_args.target_args.size(); ++i ) {
		exec_args.push_back( input_args.target_args[i].c_str() );
	}

	// Null terminate the array
	exec_args.push_back( nullptr );

	// Log the action then flush the buffers
	std::stringstream pnt;
	pnt << "Starting dr_run\nCalling execvp on: ";
	for ( unsigned long i = 0; i < exec_args.size() - 1; ++i ) {
		pnt << exec_args[i] << ' ';
	}
	Utilities::log( pnt.str() );
	fflush( NULL );

	// Start drrun
	execvp( DRRUN_PATH, (char **) exec_args.data() );
	Utilities::err( "execvp() failed." );
}

    // Setup and start the external client
    [[noreturn]] void start_external_client( const Args &args ) {

	// Setup a unix server
	// Technically, between generating the name name and the
	// server starting, the file could have been created.
	// However, this is safe as the program will crash if so
	const std::string server_name = temp_name();
	const int sock = QS::create_server( server_name.c_str() );

	// Just in case an exception occurs, setup a class
	// whose desctructor will terminate the group
	TerminateOnDestruction tod;

	// Fork
	// Note that the server belongs to the parent, the client is the child
	Utilities::log( "Starting initial fork..." );
	const pid_t pid = fork();
	Utilities::assert( pid != -1, "fork() failed" );

	// If this is the child process,
	// start the program to be protected
	if ( pid == 0 ) {
		Utilities::log( "Forming drrun args..." );
		start_program( args, (char *) server_name.c_str() );
	}

	// Otherwise, this is the parent process
	else {

		// Wait for the client then start the shadow stack
		Utilities::log( "Waiting for client" );
		const int client_sock = QS::accept_client( sock );

		// Start the shadow stack server
		start_external_shadow_stack( client_sock );

		// If the program made it to this point, nothing
		// went wrong, gracefully exit
		tod.disable();
		Group::terminate( "Program exited. Killing group", false );
	}
}

// Main function
int main( int argc, char *argv[] ) {

	// Setup then handle arguments
	run_before_everything();
	const Args args = parse_args( argc, argv );

	// We check for the return statuses of functions, so ignore sigpipe
	Utilities::assert( signal( SIGCHLD, SIG_IGN ) != SIG_ERR, "signal() failed." );
	Utilities::assert( signal( SIGPIPE, SIG_IGN ) != SIG_ERR, "signal() failed." );
	Utilities::log( "Sigpipe and Sigchild ignored" );

	// Setup complete, allow forking / threading beyond this point
	Utilities::log( "DrShadowStack initalized" );
	Utilities::enable_multi_thread_or_process_mode();

	// If the shadow stack should be internal, start it
	if ( args.mode.is_internal ) {
		char buf[10];
		start_program( args, buf );
	}

	// If the shadow stack should be external
	else if ( args.mode.is_external ) {
		start_external_client( args );
	}

	// Otherwise an unknown mode was called
	else {
		Group::terminate( "Unimplemented ss_mode called." );
	}
}
