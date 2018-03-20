#include "shadow_stack.hpp"
#include "stack_server.hpp"
#include "quick_socket.hpp"
#include "utilities.hpp"
#include "get_tid.hpp"
#include "group.hpp"

#include <unistd.h>
#include <signal.h>
#include <vector>
#include <string>


// Start's the program passed in via drrun
void start_program( char * drrun, char * a_out,
					char ** client_argv, char * socket_path ) {

	// Construct args to give to exec
	std::vector<const char *> args;

	// Add DynamoRIO args
	args.push_back(drrun);
	args.push_back("-c");
// TODO:
#define DYNAMORIO_CLIENT_SO "/home/vagrant/ShadowStack/scratch_work/" \
							"build/libshadow_stack_dr_client.so"
	args.push_back(DYNAMORIO_CLIENT_SO);
// TODO: pass in socket
	args.push_back("--");
	args.push_back(a_out);

	// Add a.out's args
	for(int i = 0; client_argv[i] != NULL; ++i ) {
		args.push_back(client_argv[i]);
	}

	// Null terminate the array
	args.push_back(nullptr);

	// Flush IO buffers then exec
	fflush(NULL);
	execvp(drrun, (char **) args.data());
	program_err("execvp() failed.");
}

// TODO: maybe boost_uniquepath, and getsockopt(SO_REUSEADDR) ?
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

	// TODO: use actual arg parser
	if ( argc < 3 ) {
		ss_log("Error: usage: ./a.out <drrun> <a.out> ...");
		exit(EXIT_FAILURE);
	}

	// Create a new process group by starting a new session
	// Many terminals will automatically do this, but just in case...
	// Also changes many default signal handlers to kill the process group
	setup_group();

	// We check for the return statuses of functions, so ignore sigpipe
	ss_assert( signal(SIGCHLD, SIG_IGN) != SIG_ERR, "signal() failed." );
	ss_assert( signal(SIGPIPE, SIG_IGN) != SIG_ERR, "signal() failed." );
	ss_log("Sigpipe and Sigchild ignored");

	// Setup a unix server
	const std::string server_name = temp_name();
	const int sock = create_server(server_name.c_str());

	// Just in case an exception occurs, setup a class
	// whose desctructor will terminate the group
	TerminateOnDestruction tod;

	// Fork
	// Note that the server belongs to the parent, the client is the child
	ss_log("Starting initial fork...");
	const pid_t pid = fork();
	ss_assert( pid != -1, "fork() failed" );
	
	// If this is the child process,
	// start the program to be protected
	if (pid == 0) {
		ss_log("%llu: starting drrun", get_tid());
		start_program( argv[1], argv[2], & argv[3], (char *) server_name.c_str());
	}

	// Otherwise, this is the parent process
	else {

		// Wait for the client then start the shadow stack
		ss_log("%llu: waiting for client", get_tid());
		start_shadow_stack(accept_client(sock));

		// If the program made it to this point, nothing
		// went wrong, gracefully exit
		tod.disable();
		exit(EXIT_SUCCESS);
	}
}
