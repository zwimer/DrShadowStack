#include "shadow_stack.hpp"
#include "stack_server.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "group.hpp"

#include <unistd.h>
#include <signal.h>
#include <string>

// TODO: delete
#include "delete_me.hpp"
#include <iostream>


// Start's the program passed in via drrun
void start_program(char ** argv, const char * const socket_path) {

	// TODO: write
	const int sock = create_client(socket_path);
	delete_me(sock);
}

// TODO: maybe boost_uniquepath, and getsockopt(SO_REUSEADDR) ?
// Wraps std::tmpnam(0)
// std::tmpnam is warned against due to security reasons
// these reasons, however, are of no concern for it's use
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
std::string temp_name() {
	const char * const str = std::tmpnam(nullptr);
	std::cout << "Socket = \n" << str << std::endl;
	ss_assert( str != nullptr, "std::tmpnam() failed." );
	return str;
}
#pragma GCC diagnostic pop

// Main function
int main(int argc, char * argv[]) {

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
		start_program(argv, server_name.c_str());
	}

	// Otherwise, this is the parent process,
	// wait for hte client then start the shadow stack
	else {
		start_shadow_stack(accept_client(sock));

		// If the program made it to this point, nothing
		// went wrong, gracefully exit
		tod.disable();
		exit(EXIT_SUCCESS);
	}
}
