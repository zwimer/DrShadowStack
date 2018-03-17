#include "run_shadow_stack.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "stack.hpp"
#include "group.hpp"

#include <unistd.h>
#include <signal.h>
#include <string>

// TODO: delete
#include "delete_me.hpp"
#include <iostream>
using std::cout; using std::endl;

// Start's the program passed in via drrun
void start_program(char ** argv, int sock) {

	// TODO: write
	delete_me(sock);
}

// Wraps std::tmpnam(0)
// std::tmpnam is warned against due to security reasons
// these reasons, however, are of no concern for it's use
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
std::string temp_name() {
	const char * const str = std::tmpnam(nullptr);
	ss_assert( str != nullptr, "std::tmpnam() failed." );
	return str;
}
#pragma GCC diagnostic pop

// Main function
int main(int argc, char * argv[]) {

	// TODO: run everything in 1, try catch block and 2, with exception handlers to kill all if error

	// Create a new process group by starting a new session
	// Many terminals will automatically do this, but just in case...
	// Also changes many default signal handlers to kill the process group
	setup_group();

	// We check for the return statuses of functions, so ignore sigpipe
	ss_assert( signal(SIGPIPE, SIG_IGN) != SIG_ERR, "signal() failed." );

	// Setup a unix server
	const std::string server_name = temp_name();
	const int sock = create_server(server_name.c_str());

	// Create a client then accept the client
	const int client_sock = create_client(server_name.c_str());
	const int server_sock = accept_client(sock);

	// Just in case an exception occurs, setup a class
	// whose desctructor will terminate the group
	TerminateOnDestruction tod;

	// Fork
	// Note that the server belongs to the parent, the client is the child
	const pid_t pid = fork();
	ss_assert( pid != -1, "fork() failed" );

	// If this is the child process,
	// start the program to be protected
	if (pid == 0) {
		ss_assert( close(server_sock) != -1 , "close() failed" );
		start_program(argv, client_sock);
	}

	// Otherwise, this is the parent process,
	// start the shadow stack
	else {
		ss_assert( close(client_sock) != -1 , "close() failed" );
		start_shadow_stack(server_sock);
	}

	// If the program made it to this point, nothing
	// went wrong, gracefully exit
	tod.disable();
	exit(EXIT_SUCCESS);
}
